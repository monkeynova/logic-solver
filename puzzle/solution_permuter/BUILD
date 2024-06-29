cc_library(
    name = "allowed_value_solution_permuter",
    srcs = ["allowed_value_solution_permuter.cc"],
    hdrs = ["allowed_value_solution_permuter.h"],
    deps = [
        ":mutable_solution",
        ":solution_permuter",
        "//puzzle/base:solution_filter",
    ],
)

cc_library(
    name = "brute_solution_permuter",
    srcs = ["brute_solution_permuter.cc"],
    hdrs = ["brute_solution_permuter.h"],
    deps = [
        ":mutable_solution",
        ":solution_permuter",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter",
        "//puzzle/class_permuter:factory",
    ],
)

cc_test(
    name = "brute_solution_permuter_test",
    srcs = ["brute_solution_permuter_test.cc"],
    deps = [
        ":brute_solution_permuter",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "class_pair_selectivity",
    hdrs = ["class_pair_selectivity.h"],
    deps = [
        ":filter_to_active_set",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter",
    ],
)

cc_library(
    name = "filter_to_active_set",
    srcs = ["filter_to_active_set.cc"],
    hdrs = ["filter_to_active_set.h"],
    deps = [
        ":mutable_solution",
        "//puzzle/active_set",
        "//puzzle/active_set:pair",
        "//puzzle/base:all_match",
        "//puzzle/base:profiler",
        "//puzzle/base:solution",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter",
        "//puzzle/class_permuter:value_skip_to_active_set",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "filter_to_active_set_benchmark",
    srcs = ["filter_to_active_set_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":filter_to_active_set",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter:factory",
        "@com_github_google_benchmark//:benchmark",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "filter_to_active_set_test",
    srcs = ["filter_to_active_set_test.cc"],
    deps = [
        ":filter_to_active_set",
        ":mutable_solution",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter:factory",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "pair_filter_burn_down",
    srcs = ["pair_filter_burn_down.cc"],
    hdrs = ["pair_filter_burn_down.h"],
    deps = [
        ":class_pair_selectivity",
        ":filter_to_active_set",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter",
        "//thread:executor",
        "//thread:future",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "filtered_solution_permuter",
    srcs = ["filtered_solution_permuter.cc"],
    hdrs = ["filtered_solution_permuter.h"],
    deps = [
        ":filter_to_active_set",
        ":mutable_solution",
        ":pair_filter_burn_down",
        ":solution_permuter",
        "//puzzle/base:all_match",
        "//puzzle/base:profiler",
        "//puzzle/base:solution_filter",
        "//puzzle/class_permuter",
        "//puzzle/class_permuter:factory",
        "//thread:executor",
        "//thread:future",
        "//thread:inline_executor",
        "//thread:pool",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_test(
    name = "filtered_solution_permuter_test",
    srcs = ["filtered_solution_permuter_test.cc"],
    deps = [
        ":filtered_solution_permuter",
        "//puzzle/base:solution_filter",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "solution_permuter",
    srcs = ["solution_permuter.cc"],
    hdrs = ["solution_permuter.h"],
    visibility = ["//puzzle:__subpackages__"],
    deps = [
        ":mutable_solution",
        "//puzzle/base:solution",
        "//puzzle/base:solution_filter",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "solution_permuter_factory",
    srcs = ["solution_permuter_factory.cc"],
    hdrs = ["solution_permuter_factory.h"],
    visibility = ["//puzzle:__subpackages__"],
    deps = [
        ":allowed_value_solution_permuter",
        ":brute_solution_permuter",
        ":filtered_solution_permuter",
        ":solution_permuter",
        "//puzzle/base:profiler",
        "//puzzle/base:solution",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_library(
    name = "mutable_solution",
    hdrs = ["mutable_solution.h"],
    deps = [
        "//puzzle/base:solution",
        "//puzzle/class_permuter",
    ],
)
