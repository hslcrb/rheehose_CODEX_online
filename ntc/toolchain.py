from __future__ import annotations

import shutil


class ToolchainNotFoundError(RuntimeError):
    pass


def detect_c_compiler() -> str:
    for candidate in ("clang", "gcc"):
        found = shutil.which(candidate)
        if found:
            return found
    raise ToolchainNotFoundError("Neither clang nor gcc was found in PATH")
