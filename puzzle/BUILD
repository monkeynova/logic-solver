cc_library(
    name = "solver",
    srcs = ["solver.cc"],
    hdrs = ["solver.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":all_match",
        ":profiler",
        ":solution",
        ":solution_permuter",
        ":solution_permuter_factory",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "all_match",
    hdrs = ["all_match.h"],
    deps = [
        ":class_permuter",
        ":solution",
        ":solution_filter",
    ],
)

cc_test(
    name = "all_match_test",
    srcs = ["all_match_test.cc"],
    deps = [
        ":all_match",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "active_set",
    hdrs = ["active_set.h"],
    deps = [
        ":active_set_bit_vector",
        ":active_set_run_length",
        ":active_set_run_position",
    ],
)

cc_library(
    name = "active_set_bit_vector",
    srcs = ["active_set_bit_vector.cc"],
    hdrs = ["active_set_bit_vector.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "active_set_run_length",
    srcs = ["active_set_run_length.cc"],
    hdrs = ["active_set_run_length.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "active_set_run_position",
    srcs = ["active_set_run_position.cc"],
    hdrs = ["active_set_run_position.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "active_set_test",
    srcs = ["active_set_test.cc"],
    deps = [
        ":active_set",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "active_set_pair",
    srcs = ["active_set_pair.cc"],
    hdrs = ["active_set_pair.h"],
    deps = [
        ":active_set",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
)

cc_test(
    name = "active_set_pair_test",
    srcs = ["active_set_pair_test.cc"],
    deps = [
        ":active_set_pair",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "filter_to_active_set",
    srcs = ["filter_to_active_set.cc"],
    hdrs = ["filter_to_active_set.h"],
    deps = [
        ":active_set",
        ":active_set_pair",
        ":all_match",
        ":class_permuter",
        ":mutable_solution",
        ":profiler",
        ":solution",
        ":solution_filter",
        ":value_skip_to_active_set",
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/functional:function_ref",
        "@com_google_absl//absl/status",
    ],
)

cc_binary(
    name = "filter_to_active_set_benchmark",
    srcs = ["filter_to_active_set_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":benchmark_main",
        ":class_permuter_factory",
        ":filter_to_active_set",
        ":solution_filter",
        "@com_google_benchmark//:benchmark",
    ],
)

cc_test(
    name = "filter_to_active_set_test",
    srcs = ["filter_to_active_set_test.cc"],
    deps = [
        ":class_permuter_factory",
        ":filter_to_active_set",
        ":mutable_solution",
        ":solution_filter",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "value_skip_to_active_set",
    srcs = ["value_skip_to_active_set.cc"],
    hdrs = ["value_skip_to_active_set.h"],
    deps = [
        ":active_set",
        ":class_permuter",
    ],
)

cc_test(
    name = "value_skip_to_active_set_test",
    srcs = ["value_skip_to_active_set_test.cc"],
    deps = [
        ":class_permuter_factory",
        ":value_skip_to_active_set",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "benchmark_main",
    srcs = ["benchmark_main.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_benchmark//:benchmark",
        "@com_google_glog//:glog",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_library(
    name = "brute_solution_permuter",
    srcs = ["brute_solution_permuter.cc"],
    hdrs = ["brute_solution_permuter.h"],
    deps = [
        ":class_permuter",
        ":class_permuter_factory",
        ":mutable_solution",
        ":solution_filter",
        ":solution_permuter",
    ],
)

cc_library(
    name = "class_pair_selectivity",
    hdrs = ["class_pair_selectivity.h"],
    deps = [
        ":class_permuter",
        ":solution_filter",
    ],
)

cc_library(
    name = "filtered_solution_permuter",
    srcs = ["filtered_solution_permuter.cc"],
    hdrs = ["filtered_solution_permuter.h"],
    deps = [
        ":all_match",
        ":class_pair_selectivity",
        ":class_permuter",
        ":class_permuter_factory",
        ":filter_to_active_set",
        ":mutable_solution",
        ":profiler",
        ":solution_filter",
        ":solution_permuter",
        "//thread:executor",
        "//thread:future",
        "//thread:inline_executor",
        "//thread:pool",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "solution_permuter",
    srcs = ["solution_permuter.cc"],
    hdrs = ["solution_permuter.h"],
    deps = [
        ":mutable_solution",
        ":solution",
        ":solution_filter",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "solution_permuter_factory",
    srcs = ["solution_permuter_factory.cc"],
    hdrs = ["solution_permuter_factory.h"],
    deps = [
        ":brute_solution_permuter",
        ":filtered_solution_permuter",
        ":profiler",
        ":solution",
        ":solution_permuter",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_library(
    name = "class_permuter",
    srcs = ["class_permuter.cc"],
    hdrs = ["class_permuter.h"],
    deps = [
        ":active_set",
        ":solution",
        "@com_google_absl//absl/container:inlined_vector",
    ],
)

cc_library(
    name = "class_permuter_steinhaus_johnson_trotter",
    srcs = ["class_permuter_steinhaus_johnson_trotter.cc"],
    hdrs = ["class_permuter_steinhaus_johnson_trotter.h"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "class_permuter_factorial_radix",
    srcs = ["class_permuter_factorial_radix.cc"],
    hdrs = ["class_permuter_factorial_radix.h"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "class_permuter_factorial_radix_delete_tracking",
    srcs = ["class_permuter_factorial_radix_delete_tracking.cc"],
    hdrs = ["class_permuter_factorial_radix_delete_tracking.h"],
    deps = [
        ":class_permuter",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "class_permuter_factory",
    srcs = ["class_permuter_factory.cc"],
    hdrs = ["class_permuter_factory.h"],
    deps = [
        ":class_permuter",
        ":class_permuter_factorial_radix",
        ":class_permuter_factorial_radix_delete_tracking",
        ":class_permuter_steinhaus_johnson_trotter",
        ":solution",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_library(
    name = "mutable_solution",
    hdrs = ["mutable_solution.h"],
    deps = [
        ":class_permuter",
        ":solution",
    ],
)

cc_library(
    name = "problem",
    srcs = ["problem.cc"],
    hdrs = ["problem.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":solver",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "proto_problem",
    srcs = ["proto_problem.cc"],
    hdrs = ["proto_problem.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":problem",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "descriptors",
    srcs = [],
    hdrs = ["descriptors.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_google_protobuf//:protobuf",
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

cc_library(
    name = "profiler",
    srcs = ["profiler.cc"],
    hdrs = ["profiler.h"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "class_permuter_test",
    srcs = ["class_permuter_test.cc"],
    deps = [
        ":class_permuter",
        ":class_permuter_factorial_radix",
        ":class_permuter_factorial_radix_delete_tracking",
        ":class_permuter_steinhaus_johnson_trotter",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_binary(
    name = "class_permuter_benchmark",
    srcs = ["class_permuter_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":benchmark_main",
        ":class_permuter_factorial_radix",
        ":class_permuter_factorial_radix_delete_tracking",
        ":class_permuter_steinhaus_johnson_trotter",
        "@com_google_absl//absl/strings",
        "@com_google_benchmark//:benchmark",
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

cc_test(
    name = "filtered_solution_permuter_test",
    srcs = ["filtered_solution_permuter_test.cc"],
    deps = [
        ":filtered_solution_permuter",
        ":solution_filter",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "puzzle_main",
    srcs = ["puzzle_main.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":problem",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_library(
    name = "puzzle_test",
    testonly = 1,
    srcs = ["puzzle_test.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":problem",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
