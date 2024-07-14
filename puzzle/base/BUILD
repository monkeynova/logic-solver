package(default_visibility = ["//puzzle:__subpackages__"])

cc_library(
    name = "all_match",
    hdrs = ["all_match.h"],
    visibility = ["//puzzle:__subpackages__"],
    deps = [
        "//puzzle/base:solution_filter",
        "//puzzle/base:solution_view",
        "@com_google_absl//absl/algorithm:container",
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
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "position",
    hdrs = ["position.h"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "profiler",
    srcs = ["profiler.cc"],
    hdrs = ["profiler.h"],
    deps = [
        ":position",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
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
