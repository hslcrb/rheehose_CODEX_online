from .driver import run


if __name__ == "__main__":
    raise SystemExit(run(__import__("sys").argv[1:]))
