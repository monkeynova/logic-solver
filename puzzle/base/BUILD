package(default_visibility = ["//puzzle:__subpackages__"])

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
    name = "profiler",
    srcs = ["profiler.cc"],
    hdrs = ["profiler.h"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "solution",
    srcs = ["solution.cc"],
    hdrs = ["solution.h"],
    deps = [
        ":descriptors",
    ],
)

cc_library(
    name = "solution_filter",
    hdrs = ["solution_filter.h"],
    deps = [
        ":solution",
    ],
)
