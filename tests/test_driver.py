from ntc.driver import _normalize_opt, make_parser


def test_normalize_opt_variants():
    assert _normalize_opt("2") == "O2"
    assert _normalize_opt("-O3") == "O3"
    assert _normalize_opt(" fast ") == "Ofast"


def test_parser_handles_gcc_like_flags():
    parser = make_parser()
    args = parser.parse_args(["main.c", "-c", "-O", "3", "-Wall", "-I", "inc", "-D", "X=1"])
    assert args.c is True
    assert args.opt == "3"
    assert args.Wall is True
    assert args.includes == ["inc"]
    assert args.defines == ["X=1"]
