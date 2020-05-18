load("sudoku.bzl", "sudoku_test")

cc_library(
    name = "base",
    srcs = ["base.cc"],
    hdrs = ["base.h"],
    deps = [
        "//puzzle:problem",
	"@com_google_absl//absl/flags:flag",
    ],
)

cc_binary(
    name = "multi_line",
    srcs = ["multi_line.cc"],
    deps = [
        ":line_board",
	"//puzzle:main_lib",
    ]
)

cc_binary(
    name = "single_line",
    srcs = ["single_line.cc"],
    deps = [
        ":line_board",
	"//puzzle:main_lib",
	"@com_google_absl//absl/flags:flag",
    ]
)

cc_test(
    name = "single_line_test",
    srcs = ["single_line_test.cc"],
    args = [
        "--sudoku_line_board=" +
        "......2.4" +
        "1..9....." +
        "........8" +
        "68.7....." +
        "...1..73." +
        ".4......." +
        "....25..." +
        "5......9." +
        "....4....",
        "--sudoku_line_answer=" +
        "978531264" +
        "124968357" +
        "356472918" +
        "681793542" +
        "295184736" +
        "743256189" +
        "439625871" +
        "562817493" +
        "817349625",
    ],
    tags = ["benchmark", "slowbenchmark"],
    deps = [
        ":line_board",
	"@com_google_absl//absl/flags:flag",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
        "//puzzle:test_main",
    ]
)

cc_test(
    name = "single_line_test_empirical",
    srcs = ["single_line_test.cc"],
    args = [
        "--sudoku_line_board=" +
        ".......13" +
	"...2....." +
	".......8." +
	"...76.2.." +
	"..8...4.." +
	".1......." +
	"2.....75." +
	"6..34...." +
	".....8...",
        "--sudoku_line_answer=" +
	"562874913" +
	"839215647" +
	"174639582" +
	"345761298" +
	"728953461" +
	"916482375" +
	"283196754" +
	"651347829" +
	"497528136",
	# TODO(keith@monkeynova.com): There's a bug if this is set to true.
	"--puzzle_prune_pair_class_iterators_mode_pair=false",
    ],
    tags = ["benchmark", "slowbenchmark"],
    deps = [
        ":line_board",
	"@com_google_absl//absl/flags:flag",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
        "//puzzle:test_main",
    ]
)

cc_library(
    name = "line_board",
    hdrs = ["line_board.h"],
    srcs = ["line_board.cc"],
    deps = [
        ":base",
    ],
)

cc_binary(
    name = "nyt20171202",
    deps = [
        ":nyt20171202_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nyt20171202_lib",
    srcs = ["nyt20171202.cc"],
    alwayslink = 1,
    deps = [
        ":base",
        "//puzzle:puzzle_main",
    ],
)

sudoku_test(
    name = "nyt20171202_test",
    deps = [
        ":nyt20171202_lib",
    ],
)

cc_binary(
    name = "nyt20180101easy",
    deps = [
        ":nyt20180101easy_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nyt20180101easy_lib",
    srcs = ["nyt20180101easy.cc"],
    alwayslink = 1,
    deps = [
        ":base",
    ],
)

sudoku_test(
    name = "nyt20180101easy_test",
    deps = [
        ":nyt20180101easy_lib",
    ],
)

sudoku_test(
    name = "nyt20180101easy_nopairs_test",
    args = [
	"--puzzle_prune_pair_class_iterators_mode_pair=false",
    ],
    deps = [
        ":nyt20180101easy_lib",
    ],
)

cc_binary(
    name = "nyt20180102medium",
    deps = [
        ":nyt20180102medium_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nyt20180102medium_lib",
    srcs = ["nyt20180102medium.cc"],
    alwayslink = 1,
    deps = [
        ":base",
    ],
)

sudoku_test(
    name = "nyt20180102medium_test",
    deps = [
        ":nyt20180102medium_lib",
    ],
)

cc_library(
    name = "sudoku_test",
    srcs = ["sudoku_test.cc"],
    testonly = 1,
    deps = [
        "//puzzle:problem",
        "//puzzle:test_main",
	"@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
    ],
    visibility = [
        "//visibility:public",
    ]
)

cc_binary(
    name = "nyt20181019easy",
    deps = [
        ":nyt20181019easy_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nyt20181019easy_lib",
    srcs = ["nyt20181019easy.cc"],
    alwayslink = 1,
    deps = [
        ":base",
    ],
)

sudoku_test(
    name = "nyt20181019easy_test",
    deps = [
        ":nyt20181019easy_lib",
    ],
)

cc_binary(
    name = "nyt20181019hard",
    deps = [
        ":nyt20181019hard_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nyt20181019hard_lib",
    srcs = ["nyt20181019hard.cc"],
    alwayslink = 1,
    deps = [
        ":base",
    ],
)

sudoku_test(
    name = "nyt20181019hard_test",
    deps = [
        ":nyt20181019hard_lib",
    ],
)
