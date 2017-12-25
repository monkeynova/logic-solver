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
	"@com_google_glog//:glog",
    ],    
)

cc_library(
    name = "brute_solution_permuter",
    hdrs = ["brute_solution_permuter.h"],
    srcs = ["brute_solution_permuter.cc"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "cropped_solution_permuter",
    hdrs = ["cropped_solution_permuter.h"],
    srcs = ["cropped_solution_permuter.cc"],
    deps = [
        ":class_permuter",
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
    name = "instance",
    hdrs = ["instance.h"],
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
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_binary(
    name = "class_permuter_benchmark",
    srcs = ["class_permuter_benchmark.cc"],
    deps = [
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
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ]
)

cc_test(
    name = "cropped_solution_permuter_test",
    srcs = ["cropped_solution_permuter_test.cc"],
    deps = [
        ":cropped_solution_permuter",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ]
)
