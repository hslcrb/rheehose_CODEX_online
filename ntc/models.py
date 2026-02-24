from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class BuildRequest:
    sources: list[Path]
    output: Path | None
    compile_only: bool
    preprocess_only: bool
    assembly_only: bool
    optimization: str
    debug: bool
    warnings: list[str] = field(default_factory=list)
    include_dirs: list[str] = field(default_factory=list)
    defines: list[str] = field(default_factory=list)
    library_dirs: list[str] = field(default_factory=list)
    libraries: list[str] = field(default_factory=list)
    std: str | None = None
    passthrough: list[str] = field(default_factory=list)


@dataclass
class BuildResult:
    return_code: int
    commands: list[list[str]]
    used_cache: int = 0
