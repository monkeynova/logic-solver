cc_library(
    name = "solver",
    hdrs = ["solver.h"],
    srcs = ["solver.cc"],
    deps = [
        ":all_match",
	":profiler",
        ":solution",
	":solution_permuter",
	":solution_permuter_factory",
    ],
    visibility = [
        "//visibility:public"
    ],
)

cc_library(
    name = "all_match",
    hdrs = ["all_match.h"],
    deps = [
        ":class_permuter",
        ":solution_filter",
        ":solution",
    ],
)

cc_library(
    name = "active_set",
    hdrs = ["active_set.h"],
    srcs = ["active_set.cc"],
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
        ":test_main",
    ]
)   

cc_library(
    name = "active_set_pair",
    hdrs = ["active_set_pair.h"],
    srcs = ["active_set_pair.cc"],
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
        ":test_main",
    ]
)   

cc_library(
    name = "active_set_builder",
    hdrs = ["active_set_builder.h"],
    srcs = ["active_set_builder.cc"],
    deps = [
        ":active_set",
	":active_set_pair",
        ":all_match",
        ":class_permuter",
        ":mutable_solution",
        ":solution",
        ":solution_filter",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_binary(
    name = "active_set_builder_benchmark",
    srcs = ["active_set_builder_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":active_set_builder",
	":benchmark_main",
        ":class_permuter_factory",
        ":solution_filter",
        "@com_google_benchmark//:benchmark",
    ]
)

cc_test(
    name = "active_set_builder_test",
    srcs = ["active_set_builder_test.cc"],
    deps = [
        ":active_set_builder",
        ":class_permuter_factory",
        ":mutable_solution",
        ":solution_filter",
        ":test_main",
    ]
)   

cc_library(
    name = "benchmark_main",
    srcs = ["benchmark_main.cc"],
    deps = [
	"@com_google_absl//absl/flags:flag",
	"@com_google_absl//absl/flags:parse",
        "@com_google_benchmark//:benchmark",
	"@com_google_glog//:glog",
    ],
    visibility = [
        "//visibility:public",
    ]
)

cc_library(
    name = "brute_solution_permuter",
    hdrs = ["brute_solution_permuter.h"],
    srcs = ["brute_solution_permuter.cc"],
    deps = [
        ":class_permuter",
        ":class_permuter_factory",
        ":mutable_solution",
	":solution_filter",
	":solution_permuter",
    ],
)

cc_library(
    name = "filtered_solution_permuter",
    hdrs = ["filtered_solution_permuter.h"],
    srcs = ["filtered_solution_permuter.cc"],
    deps = [
        ":active_set_builder",
        ":all_match",
        ":class_permuter",
        ":class_permuter_factory",
	":mutable_solution",
	":profiler",
        ":solution_filter",
        ":solution_permuter",
	"@com_google_absl//absl/memory",
	"@com_google_glog//:glog",
    ],
)

cc_library(
    name = "solution_permuter",
    hdrs = ["solution_permuter.h"],
    srcs = ["solution_permuter.cc"],
    deps = [
	":mutable_solution",
        ":solution",
	":solution_filter",
    ],
)

cc_library(
    name = "solution_permuter_factory",
    hdrs = ["solution_permuter_factory.h"],
    srcs = ["solution_permuter_factory.cc"],
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
    hdrs = ["class_permuter.h"],
    srcs = ["class_permuter.cc"],
    deps = [
        ":active_set",
        ":solution",
	"@com_google_absl//absl/container:inlined_vector",
    ],
)

cc_library(
    name = "class_permuter_steinhaus_johnson_trotter",
    hdrs = ["class_permuter_steinhaus_johnson_trotter.h"],
    srcs = ["class_permuter_steinhaus_johnson_trotter.cc"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "class_permuter_factorial_radix",
    hdrs = ["class_permuter_factorial_radix.h"],
    srcs = ["class_permuter_factorial_radix.cc"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "class_permuter_factorial_radix_delete_tracking",
    hdrs = ["class_permuter_factorial_radix_delete_tracking.h"],
    srcs = ["class_permuter_factorial_radix_delete_tracking.cc"],
    deps = [
        ":class_permuter",
	"@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "class_permuter_factory",
    hdrs = ["class_permuter_factory.h"],
    srcs = ["class_permuter_factory.cc"],
    deps = [
        ":class_permuter",
        ":class_permuter_factorial_radix",
        ":class_permuter_factorial_radix_delete_tracking",
        ":class_permuter_steinhaus_johnson_trotter",
        ":solution",
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
    hdrs = ["problem.h"],
    srcs = ["problem.cc"],
    deps = [
        ":solver",
    ],
    visibility = [
        "//visibility:public",
    ]
)

cc_library(
    name = "solution",
    hdrs = ["solution.h"],
    srcs = ["solution.cc"],
    deps = [ 
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_protobuf//:protobuf",
        "@com_google_glog//:glog",
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
    hdrs = ["profiler.h"],
    srcs = ["profiler.cc"],
    deps = [ 
	"@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
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
        ":test_main",
        "@com_google_googletest//:gtest",
    ]
)

cc_binary(
    name = "class_permuter_benchmark",
    srcs = ["class_permuter_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":benchmark_main",
        ":class_permuter",
        ":class_permuter_factorial_radix",
        ":class_permuter_factorial_radix_delete_tracking",
        ":class_permuter_steinhaus_johnson_trotter",
	"@com_google_absl//absl/strings",
        "@com_google_benchmark//:benchmark",
    ]
)

cc_test(
    name = "brute_solution_permuter_test",
    srcs = ["brute_solution_permuter_test.cc"],
    deps = [
        ":brute_solution_permuter",
        ":test_main",
        "@com_google_googletest//:gtest",
    ]
)

cc_test(
    name = "filtered_solution_permuter_test",
    srcs = ["filtered_solution_permuter_test.cc"],
    deps = [
        ":filtered_solution_permuter",
	":solution_filter",
        ":test_main",
        "@com_google_googletest//:gtest",
    ]
)

cc_library(
    name = "puzzle_main",
    srcs = ["puzzle_main.cc"],
    deps = [
        ":problem",
	"@com_google_absl//absl/flags:flag",
	"@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/memory",
    ],
    visibility = [
        "//visibility:public",
    ]    
)

cc_library(
    name = "puzzle_test",
    srcs = ["puzzle_test.cc"],
    testonly = 1,
    deps = [
        ":problem",
        ":test_main",
	"@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_benchmark//:benchmark",
        "@com_google_googletest//:gtest",
    ],
    visibility = [
        "//visibility:public",
    ]
)

cc_library(
    name = "test_main",
    srcs = ["test_main.cc"],
    testonly = 1,
    deps = [
	"@com_google_absl//absl/flags:flag",
	"@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "@com_google_benchmark//:benchmark",
	"@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
    visibility = [
        "//visibility:public",
    ]
)
