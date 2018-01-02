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
    srcs = ["nyt20180101easy.cc"],
    deps = [
        "//puzzle:puzzle_main",
        ":base",
    ],
)

cc_test(
    name = "nyt20180101easy_test",
    srcs = ["nyt20180101easy.cc"],
    args = [
        "--sudoku_problem_setup=pairwise",
        "--puzzle_prune_pair_class_iterators=true",
    ],
    deps = [
        ":base",
        "//puzzle:puzzle_test",
        "@com_google_googletest//:gtest",	
    ],
)
