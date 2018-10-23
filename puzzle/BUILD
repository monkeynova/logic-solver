cc_library(
    name = "solver",
    hdrs = ["solver.h"],
    srcs = ["solver.cc"],
    deps = [
        ":brute_solution_permuter",
        ":cropped_solution_permuter",
	":profiler",
        ":solution",
	"@com_github_gflags_gflags//:gflags",
    ],
    visibility = [
        "//visibility:public"
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
        ":class_permuter",
        ":mutable_solution",
        ":solution",
        "@com_google_absl//absl/container:flat_hash_set",
    ],
)

cc_binary(
    name = "active_set_builder_benchmark",
    srcs = ["active_set_builder_benchmark.cc"],
    deps = [
        ":active_set_builder",
	":benchmark_main",
        "@com_google_benchmark//:benchmark",
    ]
)

cc_test(
    name = "active_set_builder_test",
    srcs = ["active_set_builder_test.cc"],
    deps = [
        ":active_set_builder",
        ":mutable_solution",
        ":test_main",
    ]
)   

cc_library(
    name = "benchmark_main",
    srcs = ["benchmark_main.cc"],
    deps = [
	"@com_github_gflags_gflags//:gflags",
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
        ":mutable_solution",
    ],
)

cc_library(
    name = "cropped_solution_permuter",
    hdrs = ["cropped_solution_permuter.h"],
    srcs = ["cropped_solution_permuter.cc"],
    deps = [
        ":active_set_builder",
        ":class_permuter",
        ":mutable_solution",
        ":profiler",
	"@com_google_glog//:glog",
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
    name = "profiler",
    hdrs = ["profiler.h"],
    srcs = ["profiler.cc"],
    deps = [ 
	"@com_github_gflags_gflags//:gflags",
        "@com_google_absl//absl/memory",
   ],
)
 
cc_test(
    name = "class_permuter_test",
    srcs = ["class_permuter_test.cc"],
    deps = [
        ":class_permuter",
        ":test_main",
        "@com_google_googletest//:gtest",
    ]
)

cc_binary(
    name = "class_permuter_benchmark",
    srcs = ["class_permuter_benchmark.cc"],
    deps = [
        ":benchmark_main",
        ":class_permuter",
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
    name = "cropped_solution_permuter_test",
    srcs = ["cropped_solution_permuter_test.cc"],
    deps = [
        ":cropped_solution_permuter",
        ":test_main",
        "@com_google_googletest//:gtest",
    ]
)

cc_library(
    name = "puzzle_main",
    srcs = ["puzzle_main.cc"],
    deps = [
        ":problem",
        "@com_google_absl//absl/memory",
    ],
    visibility = [
        "//visibility:public",
    ]    
)

cc_library(
    name = "puzzle_test",
    srcs = ["puzzle_test.cc"],
    deps = [
        ":problem",
        ":test_main",
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
    deps = [
	"@com_github_gflags_gflags//:gflags",
        "@com_google_benchmark//:benchmark",
	"@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
    visibility = [
        "//visibility:public",
    ]
)
