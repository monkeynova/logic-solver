load("@com_monkeynova_base_workspace//:default_rules.bzl", "default_rules")
load("//puzzle:puzzle.bzl", "puzzle_test")

cc_library(
    name = "board",
    hdrs = ["board.h"],
    deps = [
        "//puzzle:problem",
        "@com_googlesource_code_re2//:re2",
    ],
)

cc_binary(
    name = "nytkenken20240504_small",
    deps = [
        ":nytkenken20240504_small_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nytkenken20240504_small_lib",
    srcs = ["nytkenken20240504_small.cc"],
    deps = [
        ":board",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

puzzle_test(
    name = "nytkenken20240504_small_test",
    deps = [
        ":nytkenken20240504_small_lib",
    ],
)

cc_binary(
    name = "nytkenken20240504_large",
    deps = [
        ":nytkenken20240504_large_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nytkenken20240504_large_lib",
    srcs = ["nytkenken20240504_large.cc"],
    deps = [
        ":board",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

puzzle_test(
    name = "nytkenken20240504_large_test",
    deps = [
        ":nytkenken20240504_large_lib",
    ],
)

cc_binary(
    name = "conceptis_puzzles",
    deps = [
        ":conceptis_puzzles_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "conceptis_puzzles_lib",
    srcs = ["conceptis_puzzles.cc"],
    deps = [
        ":board",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

# TODO: Unable once it runs in reasonable time.
#puzzle_test(
#    name = "conceptis_puzzles_test",
#    deps = [
#        ":conceptis_puzzles_lib",
#    ],
#)
