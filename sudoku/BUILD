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
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_library(
    name = "multi_line_test_lib",
    testonly = 1,
    srcs = ["multi_line_test.cc"],
    deps = [
        ":line_board",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "@com_google_file_based_test_driver//file_based_test_driver",
        "@com_google_file_based_test_driver//file_based_test_driver:run_test_case_result",
        "@com_google_file_based_test_driver//file_based_test_driver:test_case_options",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:main_lib",
        # TODO(@monkeynova): FBTD includes it's own main which interferes with
        # logging, flags etc.
        # "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "multi_line_test",
    srcs = [],
    args = ["--test_filename=sudoku17_sample.test"],
    data = ["sudoku17_sample.test"],
    deps = [":multi_line_test_lib"],
)

cc_test(
    name = "simple_test",
    srcs = [],
    args = ["--test_filename=simple.test"],
    data = ["simple.test"],
    deps = [":multi_line_test_lib"],
)

cc_binary(
    name = "single_line",
    srcs = ["single_line.cc"],
    deps = [
        ":line_board",
        "@com_google_absl//absl/flags:flag",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
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
    tags = [
        "benchmark",
        "slowbenchmark",
    ],
    deps = [
        ":line_board",
        "@com_google_absl//absl/flags:flag",
        "@com_google_benchmark//:benchmark",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "line_board",
    srcs = ["line_board.cc"],
    hdrs = ["line_board.h"],
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
    deps = [
        ":base",
        "//puzzle:puzzle_main",
    ],
    alwayslink = 1,
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
    deps = [
        ":base",
    ],
    alwayslink = 1,
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
    deps = [
        ":base",
    ],
    alwayslink = 1,
)

sudoku_test(
    name = "nyt20180102medium_test",
    deps = [
        ":nyt20180102medium_lib",
    ],
)

cc_library(
    name = "sudoku_test",
    testonly = 1,
    srcs = ["sudoku_test.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//puzzle:problem",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
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
    deps = [
        ":base",
    ],
    alwayslink = 1,
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
    deps = [
        ":base",
    ],
    alwayslink = 1,
)

sudoku_test(
    name = "nyt20181019hard_test",
    deps = [
        ":nyt20181019hard_lib",
    ],
)

cc_library(
    name = "killer_sudoku",
    srcs = ["killer_sudoku.cc"],
    hdrs = ["killer_sudoku.h"],
    deps = [
        ":base",
    ],
)

cc_library(
    name = "conceptis_puzzles_lib",
    srcs = ["conceptis_puzzles.cc"],
    deps = [
        ":killer_sudoku",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "conceptis_puzzles",
    deps = [
        ":conceptis_puzzles_lib",
        "//puzzle:puzzle_main",
    ],
)

sudoku_test(
    name = "conceptis_puzzles_test",
    deps = [
        ":conceptis_puzzles_lib",
    ],
)

cc_library(
    name = "greater_than_sudoku",
    srcs = ["greater_than_sudoku.cc"],
    hdrs = ["greater_than_sudoku.h"],
    deps = [
        ":base",
    ],
)

cc_library(
    name = "ksogt20200601_lib",
    srcs = ["ksogt20200601.cc"],
    deps = [
        ":greater_than_sudoku",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "ksogt20200601",
    deps = [
        ":ksogt20200601_lib",
        "//puzzle:puzzle_main",
    ],
)

sudoku_test(
    name = "ksogt20200601_test",
    deps = [
        ":ksogt20200601_lib",
    ],
)

cc_binary(
    name = "estimate_sudoku_boards",
    srcs = ["estimate_sudoku_boards.cc"],
    deps = [
        ":base",
        "@com_google_absl//absl/flags:flag",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)
