cc_library(
    name = "grid",
    srcs = ["grid.cc"],
    hdrs = ["grid.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//puzzle:problem",
        "@abseil-cpp//absl/flags:flag",
        "@re2//:re2",
    ],
)

cc_library(
    name = "ken_ken",
    srcs = ["ken_ken.cc"],
    hdrs = ["ken_ken.h"],
    deps = [
        ":grid",
        "@abseil-cpp//absl/strings",
        "@re2//:re2",
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
        ":ken_ken",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

cc_test(
    name = "nytkenken20240504_small_test",
    tags = ["benchmark"],
    deps = [
        ":nytkenken20240504_small_lib",
        "//puzzle:puzzle_test",
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
        ":ken_ken",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

cc_test(
    name = "nytkenken20240504_large_test",
    tags = ["benchmark"],
    deps = [
        ":nytkenken20240504_large_lib",
        "//puzzle:puzzle_test",
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
        ":ken_ken",
        "@com_monkeynova_gunit_main//:vlog",
    ],
    alwayslink = 1,
)

cc_test(
    name = "conceptis_puzzles_test",
    tags = ["benchmark"],
    deps = [
        ":conceptis_puzzles_lib",
        "//puzzle:puzzle_test",
    ],
)
