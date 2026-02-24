from __future__ import annotations

import hashlib
import json
import shutil
from pathlib import Path

CACHE_DIR = Path(".ntc-cache")


def stable_hash_for_compile(source: Path, compiler: str, flags: list[str]) -> str:
    digest = hashlib.sha256()
    digest.update(source.read_bytes())
    digest.update(compiler.encode("utf-8"))
    digest.update("\0".join(flags).encode("utf-8"))
    return digest.hexdigest()


def cache_path_for_key(key: str) -> Path:
    return CACHE_DIR / f"{key}.o"


def manifest_path_for_key(key: str) -> Path:
    return CACHE_DIR / f"{key}.json"


def save_cache(key: str, object_file: Path, metadata: dict) -> None:
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    shutil.copy2(object_file, cache_path_for_key(key))
    manifest_path_for_key(key).write_text(
        json.dumps(metadata, ensure_ascii=False, indent=2), encoding="utf-8"
    )


def restore_cache(key: str, target_object_file: Path) -> bool:
    cached = cache_path_for_key(key)
    if not cached.exists():
        return False
    target_object_file.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(cached, target_object_file)
    return True
