cc_library(
    name = "base",
    srcs = ["base.cc"],
    hdrs = ["base.h"],
    deps = [
        "//puzzle:problem",
    ],
)

cc_binary(
    name = "nyt20171202",
    srcs = ["nyt20171202.cc"],
    deps = [
        "//puzzle:puzzle_main",
        ":base",
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
)

cc_test(
    name = "nyt20180101easy_test",
    args = [
        "--sudoku_problem_setup=pairwise",
        "--puzzle_prune_pair_class_iterators=true",
    ],
    deps = [
        ":nyt20180101easy_lib",
        "//puzzle:puzzle_test",
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
)