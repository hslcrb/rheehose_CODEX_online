#ifndef GITEX_COMMANDS_H
#define GITEX_COMMANDS_H

int cmd_init(int argc, char **argv);
int cmd_add(int argc, char **argv);
int cmd_commit(int argc, char **argv);
int cmd_status(int argc, char **argv);
int cmd_diff(int argc, char **argv);
int cmd_log(int argc, char **argv);
int cmd_branch(int argc, char **argv);
int cmd_checkout(int argc, char **argv);
int cmd_merge(int argc, char **argv);
int cmd_reset(int argc, char **argv);
int cmd_stash(int argc, char **argv);
int cmd_tag(int argc, char **argv);
int cmd_revert(int argc, char **argv);
int cmd_tui(int argc, char **argv);

#endif
