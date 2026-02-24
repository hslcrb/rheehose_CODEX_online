#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

#define MAX_ARGS 4096
#define MAX_SOURCES 1024
#define MAX_OBJECTS 1024
#define CACHE_DIR ".ntc-cache"

typedef struct {
    char *sources[MAX_SOURCES];
    int source_count;
    char *objects[MAX_OBJECTS];
    int object_count;

    char *output;
    bool compile_only;
    bool preprocess_only;
    bool assembly_only;
    bool debug;

    char *compiler;

    char *cflags[MAX_ARGS];
    int cflag_count;

    char *linkflags[MAX_ARGS];
    int linkflag_count;

    int jobs;
} BuildConfig;

typedef struct {
    BuildConfig *cfg;
    int index;
    int rc;
    bool cache_hit;
} Task;

typedef struct {
    Task *tasks;
    int total;
    int next;
    int failed;
    pthread_mutex_t lock;
} TaskQueue;

static uint64_t fnv1a_update(uint64_t hash, const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

static bool mkdir_p(const char *path) {
    char tmp[PATH_MAX];
    size_t len = strlen(path);
    if (len >= sizeof(tmp)) return false;
    strcpy(tmp, path);
    for (size_t i = 1; i < len; ++i) {
        if (tmp[i] == '/') {
            tmp[i] = '\0';
            if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return false;
            tmp[i] = '/';
        }
    }
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST) return false;
    return true;
}

static bool file_copy(const char *src, const char *dst) {
    int in = open(src, O_RDONLY);
    if (in < 0) return false;
    int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) {
        close(in);
        return false;
    }

    char buf[65536];
    ssize_t r;
    while ((r = read(in, buf, sizeof(buf))) > 0) {
        ssize_t off = 0;
        while (off < r) {
            ssize_t w = write(out, buf + off, (size_t)(r - off));
            if (w < 0) {
                close(in);
                close(out);
                return false;
            }
            off += w;
        }
    }
    close(in);
    close(out);
    return r == 0;
}

static int run_cmd(char *const argv[]) {
    pid_t pid;
    int rc = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
    if (rc != 0) {
        fprintf(stderr, "ntc: failed to spawn '%s': %s\n", argv[0], strerror(rc));
        return 127;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) return 127;
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
    return 127;
}

static bool detect_compiler(char *out, size_t out_sz) {
    const char *path = getenv("PATH");
    if (!path) return false;

    const char *candidates[] = {"clang", "gcc"};
    for (size_t c = 0; c < 2; ++c) {
        const char *cur = path;
        while (*cur) {
            const char *sep = strchr(cur, ':');
            size_t len = sep ? (size_t)(sep - cur) : strlen(cur);
            char probe[PATH_MAX];
            if (len + 1 + strlen(candidates[c]) + 1 < sizeof(probe)) {
                memcpy(probe, cur, len);
                probe[len] = '/';
                strcpy(probe + len + 1, candidates[c]);
                if (access(probe, X_OK) == 0) {
                    snprintf(out, out_sz, "%s", candidates[c]);
                    return true;
                }
            }
            if (!sep) break;
            cur = sep + 1;
        }
    }
    return false;
}

static bool is_source(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return false;
    return strcmp(dot, ".c") == 0 || strcmp(dot, ".cc") == 0 || strcmp(dot, ".cpp") == 0 || strcmp(dot, ".cxx") == 0;
}

static void obj_name(const char *src, char *out, size_t out_sz) {
    snprintf(out, out_sz, "%s", src);
    char *dot = strrchr(out, '.');
    if (dot) strcpy(dot, ".o");
    else strncat(out, ".o", out_sz - strlen(out) - 1);
}

static uint64_t hash_compile_inputs(const BuildConfig *cfg, const char *source) {
    uint64_t h = 1469598103934665603ULL;
    h = fnv1a_update(h, (const unsigned char *)cfg->compiler, strlen(cfg->compiler));
    h = fnv1a_update(h, (const unsigned char *)"\0", 1);
    for (int i = 0; i < cfg->cflag_count; ++i) {
        h = fnv1a_update(h, (const unsigned char *)cfg->cflags[i], strlen(cfg->cflags[i]));
        h = fnv1a_update(h, (const unsigned char *)"\0", 1);
    }
    int fd = open(source, O_RDONLY);
    if (fd >= 0) {
        unsigned char buf[65536];
        ssize_t r;
        while ((r = read(fd, buf, sizeof(buf))) > 0) {
            h = fnv1a_update(h, buf, (size_t)r);
        }
        close(fd);
    }
    return h;
}

static int compile_source(BuildConfig *cfg, const char *src, bool *cache_hit) {
    char obj[PATH_MAX];
    obj_name(src, obj, sizeof(obj));

    uint64_t key = hash_compile_inputs(cfg, src);
    char cache_obj[PATH_MAX];
    snprintf(cache_obj, sizeof(cache_obj), "%s/%016llx.o", CACHE_DIR, (unsigned long long)key);

    if (access(cache_obj, R_OK) == 0 && file_copy(cache_obj, obj)) {
        *cache_hit = true;
        return 0;
    }

    char *argv[MAX_ARGS];
    int n = 0;
    argv[n++] = cfg->compiler;
    argv[n++] = "-c";
    argv[n++] = (char *)src;
    argv[n++] = "-o";
    argv[n++] = obj;
    for (int i = 0; i < cfg->cflag_count; ++i) argv[n++] = cfg->cflags[i];
    argv[n] = NULL;

    int rc = run_cmd(argv);
    if (rc == 0) {
        mkdir_p(CACHE_DIR);
        (void)file_copy(obj, cache_obj);
    }

    *cache_hit = false;
    return rc;
}

static void *worker(void *arg) {
    TaskQueue *q = (TaskQueue *)arg;
    while (1) {
        pthread_mutex_lock(&q->lock);
        if (q->next >= q->total || q->failed) {
            pthread_mutex_unlock(&q->lock);
            return NULL;
        }
        int idx = q->next++;
        Task *t = &q->tasks[idx];
        pthread_mutex_unlock(&q->lock);

        t->rc = compile_source(t->cfg, t->cfg->sources[t->index], &t->cache_hit);
        if (t->rc != 0) {
            pthread_mutex_lock(&q->lock);
            q->failed = 1;
            pthread_mutex_unlock(&q->lock);
            return NULL;
        }
    }
}

static int compile_all(BuildConfig *cfg, int *cache_hits) {
    Task tasks[MAX_SOURCES];
    TaskQueue q = {.tasks = tasks, .total = cfg->source_count, .next = 0, .failed = 0};
    pthread_mutex_init(&q.lock, NULL);

    for (int i = 0; i < cfg->source_count; ++i) {
        tasks[i].cfg = cfg;
        tasks[i].index = i;
        tasks[i].rc = 0;
        tasks[i].cache_hit = false;
    }

    int worker_count = cfg->jobs > 0 ? cfg->jobs : 1;
    if (worker_count > cfg->source_count) worker_count = cfg->source_count;
    if (worker_count < 1) worker_count = 1;

    pthread_t th[MAX_SOURCES];
    for (int i = 0; i < worker_count; ++i) pthread_create(&th[i], NULL, worker, &q);
    for (int i = 0; i < worker_count; ++i) pthread_join(th[i], NULL);

    int rc = 0;
    *cache_hits = 0;
    for (int i = 0; i < cfg->source_count; ++i) {
        if (tasks[i].cache_hit) (*cache_hits)++;
        if (tasks[i].rc != 0) rc = tasks[i].rc;
        char obj[PATH_MAX];
        obj_name(cfg->sources[i], obj, sizeof(obj));
        cfg->objects[cfg->object_count++] = strdup(obj);
    }

    pthread_mutex_destroy(&q.lock);
    return rc;
}

static int link_objects(BuildConfig *cfg) {
    char *argv[MAX_ARGS];
    int n = 0;
    argv[n++] = cfg->compiler;
    for (int i = 0; i < cfg->object_count; ++i) argv[n++] = cfg->objects[i];
    argv[n++] = "-o";
    argv[n++] = cfg->output ? cfg->output : "a.out";
    for (int i = 0; i < cfg->linkflag_count; ++i) argv[n++] = cfg->linkflags[i];
    argv[n] = NULL;
    return run_cmd(argv);
}

static int run_passthrough_mode(BuildConfig *cfg, const char *mode) {
    char *argv[MAX_ARGS];
    int n = 0;
    argv[n++] = cfg->compiler;
    argv[n++] = (char *)mode;
    for (int i = 0; i < cfg->source_count; ++i) argv[n++] = cfg->sources[i];
    for (int i = 0; i < cfg->cflag_count; ++i) argv[n++] = cfg->cflags[i];
    if (cfg->output) {
        argv[n++] = "-o";
        argv[n++] = cfg->output;
    }
    argv[n] = NULL;
    return run_cmd(argv);
}

static void print_help(void) {
    puts("ntc - New Technology Compiler (C implementation)");
    puts("usage: ntc [options] file...\n"
         "  -c            Compile only\n"
         "  -E            Preprocess only\n"
         "  -S            Assemble only\n"
         "  -o <file>     Output\n"
         "  -O* -g -std=* -I* -D* -W* -f* -m* supported\n"
         "  -L* -l*       Linker options\n"
         "  -j <n>        Parallel compile jobs");
}

static int parse_args(int argc, char **argv, BuildConfig *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->jobs = (int)sysconf(_SC_NPROCESSORS_ONLN);

    char comp[64];
    if (!detect_compiler(comp, sizeof(comp))) {
        fprintf(stderr, "ntc: clang/gcc not found in PATH\n");
        return 127;
    }
    cfg->compiler = strdup(comp);

    for (int i = 1; i < argc; ++i) {
        char *a = argv[i];
        if (strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0) {
            print_help();
            return 1;
        } else if (strcmp(a, "-c") == 0) {
            cfg->compile_only = true;
        } else if (strcmp(a, "-E") == 0) {
            cfg->preprocess_only = true;
        } else if (strcmp(a, "-S") == 0) {
            cfg->assembly_only = true;
        } else if (strcmp(a, "-g") == 0 || strncmp(a, "-O", 2) == 0 || strncmp(a, "-W", 2) == 0 || strncmp(a, "-f", 2) == 0 || strncmp(a, "-m", 2) == 0 || strncmp(a, "-std=", 5) == 0) {
            cfg->cflags[cfg->cflag_count++] = a;
        } else if ((strcmp(a, "-I") == 0 || strcmp(a, "-D") == 0 || strcmp(a, "-include") == 0 || strcmp(a, "-isystem") == 0) && i + 1 < argc) {
            cfg->cflags[cfg->cflag_count++] = a;
            cfg->cflags[cfg->cflag_count++] = argv[++i];
        } else if (strncmp(a, "-I", 2) == 0 || strncmp(a, "-D", 2) == 0) {
            cfg->cflags[cfg->cflag_count++] = a;
        } else if (strcmp(a, "-L") == 0 || strcmp(a, "-l") == 0 || strcmp(a, "-Wl") == 0) {
            if (i + 1 >= argc) return 2;
            cfg->linkflags[cfg->linkflag_count++] = a;
            cfg->linkflags[cfg->linkflag_count++] = argv[++i];
        } else if (strncmp(a, "-L", 2) == 0 || strncmp(a, "-l", 2) == 0 || strncmp(a, "-Wl,", 4) == 0) {
            cfg->linkflags[cfg->linkflag_count++] = a;
        } else if (strcmp(a, "-o") == 0 && i + 1 < argc) {
            cfg->output = argv[++i];
        } else if (strcmp(a, "-j") == 0 && i + 1 < argc) {
            cfg->jobs = atoi(argv[++i]);
            if (cfg->jobs < 1) cfg->jobs = 1;
        } else if (a[0] == '-') {
            cfg->cflags[cfg->cflag_count++] = a;
        } else if (is_source(a)) {
            cfg->sources[cfg->source_count++] = a;
        } else {
            cfg->linkflags[cfg->linkflag_count++] = a;
        }
    }

    if (cfg->source_count == 0) {
        fprintf(stderr, "ntc: no input files\n");
        return 2;
    }
    if (cfg->compile_only && cfg->output && cfg->source_count > 1) {
        fprintf(stderr, "ntc: cannot use -o with -c and multiple source files\n");
        return 2;
    }
    return 0;
}

int main(int argc, char **argv) {
    BuildConfig cfg;
    int parse_rc = parse_args(argc, argv, &cfg);
    if (parse_rc == 1) return 0;
    if (parse_rc != 0) return parse_rc;

    if (cfg.preprocess_only) return run_passthrough_mode(&cfg, "-E");
    if (cfg.assembly_only) return run_passthrough_mode(&cfg, "-S");

    int cache_hits = 0;
    int rc = compile_all(&cfg, &cache_hits);
    if (rc != 0) return rc;

    if (cfg.compile_only) {
        if (cfg.output && cfg.object_count == 1) {
            if (rename(cfg.objects[0], cfg.output) != 0) {
                perror("ntc: rename");
                return 1;
            }
        }
        printf("NTC compile succeeded (cache hits: %d)\n", cache_hits);
        return 0;
    }

    rc = link_objects(&cfg);
    if (rc == 0) printf("NTC build succeeded (cache hits: %d)\n", cache_hits);
    return rc;
}
