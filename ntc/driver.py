from __future__ import annotations

import argparse
import sys
from pathlib import Path

from .compiler import build
from .models import BuildRequest
from .toolchain import ToolchainNotFoundError, detect_c_compiler


def make_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="ntc", description="New Technology Compiler")
    p.add_argument("sources", nargs="+", help="Input source files")
    p.add_argument("-o", "--output", help="Output file")
    p.add_argument("-c", action="store_true", help="Compile only")
    p.add_argument("-E", action="store_true", help="Preprocess only")
    p.add_argument("-S", action="store_true", help="Compile to assembly")
    p.add_argument("-g", action="store_true", help="Debug info")
    p.add_argument("-O", dest="opt", default="2", help="Optimization level (0~3, s, fast)")
    p.add_argument("-std", dest="std", help="Language standard")
    p.add_argument("-I", dest="includes", action="append", default=[])
    p.add_argument("-D", dest="defines", action="append", default=[])
    p.add_argument("-L", dest="lib_dirs", action="append", default=[])
    p.add_argument("-l", dest="libs", action="append", default=[])
    p.add_argument("-Wall", action="store_true")
    p.add_argument("-Wextra", action="store_true")
    p.add_argument("-Werror", action="store_true")
    p.add_argument("--passthrough", action="append", default=[], help="Raw compiler flag")
    return p


def _normalize_opt(raw: str) -> str:
    token = raw.strip()
    if token.startswith("-O"):
        token = token[2:]
    return f"O{token}"


def run(argv: list[str]) -> int:
    parser = make_parser()
    args = parser.parse_args(argv)

    warnings: list[str] = []
    if args.Wall:
        warnings.append("-Wall")
    if args.Wextra:
        warnings.append("-Wextra")
    if args.Werror:
        warnings.append("-Werror")

    request = BuildRequest(
        sources=[Path(s) for s in args.sources],
        output=Path(args.output) if args.output else None,
        compile_only=args.c,
        preprocess_only=args.E,
        assembly_only=args.S,
        optimization=_normalize_opt(args.opt),
        debug=args.g,
        warnings=warnings,
        include_dirs=args.includes,
        defines=args.defines,
        library_dirs=args.lib_dirs,
        libraries=args.libs,
        std=args.std,
        passthrough=args.passthrough,
    )

    try:
        compiler = detect_c_compiler()
    except ToolchainNotFoundError as exc:
        print(f"NTC error: {exc}", file=sys.stderr)
        return 127

    result = build(request, compiler)
    if result.return_code == 0:
        print(f"NTC build succeeded (cache hits: {result.used_cache})")
    return result.return_code
