package(default_visibility = ["//puzzle:__subpackages__"])

cc_library(
    name = "all_match",
    hdrs = ["all_match.h"],
    visibility = ["//puzzle:__subpackages__"],
    deps = [
        "//puzzle/base:solution_filter",
        "//puzzle/base:solution_view",
        "@abseil-cpp//absl/algorithm:container",
    ],
)

cc_test(
    name = "all_match_test",
    srcs = ["all_match_test.cc"],
    deps = [
        ":all_match",
        "//puzzle:solver",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "descriptors",
    srcs = [],
    hdrs = ["descriptors.h"],
    deps = [
        "@abseil-cpp//absl/container:flat_hash_map",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@protobuf//:protobuf",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "position",
    hdrs = ["position.h"],
    deps = [
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "profiler",
    srcs = ["profiler.cc"],
    hdrs = ["profiler.h"],
    deps = [
        ":position",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/memory",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "solution_view",
    srcs = ["solution_view.cc"],
    hdrs = ["solution_view.h"],
    deps = [
        ":descriptors",
        ":position",
    ],
)

cc_library(
    name = "owned_solution",
    hdrs = ["owned_solution.h"],
    deps = [
        ":descriptors",
        ":position",
    ],
)

cc_library(
    name = "solution_filter",
    hdrs = ["solution_filter.h"],
    deps = [
        ":solution_view",
    ],
)
