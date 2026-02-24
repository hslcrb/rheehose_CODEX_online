from pathlib import Path

from ntc.cache import stable_hash_for_compile


def test_stable_hash_changes_with_flags(tmp_path: Path):
    src = tmp_path / "a.c"
    src.write_text("int main(){return 0;}\n", encoding="utf-8")

    h1 = stable_hash_for_compile(src, "gcc", ["-O2"])
    h2 = stable_hash_for_compile(src, "gcc", ["-O3"])
    assert h1 != h2
