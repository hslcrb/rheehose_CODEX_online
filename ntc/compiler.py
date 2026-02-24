from __future__ import annotations

import os
import subprocess
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

from .cache import restore_cache, save_cache, stable_hash_for_compile
from .models import BuildRequest, BuildResult


def _object_name_for(source: Path) -> Path:
    return source.with_suffix(".o")


def _compile_flags(req: BuildRequest) -> list[str]:
    flags = [f"-{req.optimization}"]
    if req.debug:
        flags.append("-g")
    flags.extend(f"-I{d}" for d in req.include_dirs)
    flags.extend(f"-D{d}" for d in req.defines)
    flags.extend(req.warnings)
    if req.std:
        flags.append(f"-std={req.std}")
    flags.extend(req.passthrough)
    return flags


def _run(cmd: list[str]) -> int:
    proc = subprocess.run(cmd, check=False)
    return proc.returncode


def _compile_one(compiler: str, source: Path, req: BuildRequest, flags: list[str]) -> tuple[int, list[str], bool]:
    output_obj = _object_name_for(source)
    cache_key = stable_hash_for_compile(source, compiler, flags)
    if restore_cache(cache_key, output_obj):
        return 0, ["cache-hit", str(source), str(output_obj)], True

    cmd = [compiler, "-c", str(source), "-o", str(output_obj), *flags]
    rc = _run(cmd)
    if rc == 0 and output_obj.exists():
        save_cache(
            cache_key,
            output_obj,
            {"source": str(source), "compiler": compiler, "flags": flags},
        )
    return rc, cmd, False


def build(req: BuildRequest, compiler: str) -> BuildResult:
    flags = _compile_flags(req)
    commands: list[list[str]] = []
    used_cache = 0

    if req.preprocess_only:
        cmd = [compiler, "-E", *(str(s) for s in req.sources), *flags]
        return BuildResult(_run(cmd), [cmd])

    if req.assembly_only:
        cmd = [compiler, "-S", *(str(s) for s in req.sources), *flags]
        return BuildResult(_run(cmd), [cmd])

    compile_only = req.compile_only
    object_files: list[Path] = []

    max_workers = min(max(os.cpu_count() or 1, 1), max(len(req.sources), 1))
    with ThreadPoolExecutor(max_workers=max_workers) as ex:
        futures = [ex.submit(_compile_one, compiler, src, req, flags) for src in req.sources]
        for src, fut in zip(req.sources, futures):
            rc, cmd, from_cache = fut.result()
            commands.append(cmd)
            if from_cache:
                used_cache += 1
            if rc != 0:
                return BuildResult(rc, commands, used_cache=used_cache)
            object_files.append(_object_name_for(src))

    if compile_only:
        if req.output and len(object_files) == 1:
            object_files[0].replace(req.output)
        return BuildResult(0, commands, used_cache=used_cache)

    out = req.output or Path("a.out")
    link_cmd = [compiler, *(str(o) for o in object_files), "-o", str(out)]
    link_cmd.extend(f"-L{d}" for d in req.library_dirs)
    link_cmd.extend(f"-l{l}" for l in req.libraries)
    commands.append(link_cmd)
    rc = _run(link_cmd)
    return BuildResult(rc, commands, used_cache=used_cache)
