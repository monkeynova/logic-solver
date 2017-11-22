cc_library(
    name="solver",
    hdrs=["solver.h"],
    srcs=["solver.cc"],
    deps = [
        ":brute_solution_permuter",
        ":cropped_solution_permuter",
        ":solution",
    ],
    visibility=[
        "//visibility:public"
    ],
)

cc_library(
    name="brute_solution_permuter",
    hdrs=["brute_solution_permuter.h"],
    srcs=["brute_solution_permuter.cc"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name="cropped_solution_permuter",
    hdrs=["cropped_solution_permuter.h"],
    srcs=["cropped_solution_permuter.cc"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name="class_permuter",
    hdrs=["class_permuter.h"],
    srcs=["class_permuter.cc"],
    deps = [
        ":solution",
    ],
)

cc_library(
    name="solution",
    hdrs=["solution.h"],
    deps = [
    ],
)

 
cc_test(
    name = "class_permuter_test",
    srcs = ["class_permuter_test.cc"],
    deps = [
        ":class_permuter",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ]
)

cc_test(
    name = "brute_solution_permuter_test",
    srcs = ["brute_solution_permuter_test.cc"],
    deps = [
        ":brute_solution_permuter",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ]
)
