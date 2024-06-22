cc_library(
    name = "class_permuter",
    srcs = ["class_permuter.cc"],
    hdrs = ["class_permuter.h"],
    visibility = [
        "//puzzle:__subpackages__",
    ],
    deps = [
        "//puzzle/active_set",
        "//puzzle/base:all_match",
        "//puzzle/base:solution",
        "@com_google_absl//absl/container:inlined_vector",
    ],
)

cc_library(
    name = "steinhaus_johnson_trotter",
    srcs = ["steinhaus_johnson_trotter.cc"],
    hdrs = ["steinhaus_johnson_trotter.h"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "factorial_radix",
    srcs = ["factorial_radix.cc"],
    hdrs = ["factorial_radix.h"],
    deps = [
        ":class_permuter",
    ],
)

cc_library(
    name = "factorial_radix_delete_tracking",
    srcs = ["factorial_radix_delete_tracking.cc"],
    hdrs = ["factorial_radix_delete_tracking.h"],
    deps = [
        ":class_permuter",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "factory",
    srcs = ["factory.cc"],
    hdrs = ["factory.h"],
    visibility = [
        "//puzzle:__subpackages__",
    ],
    deps = [
        ":class_permuter",
        ":factorial_radix",
        ":factorial_radix_delete_tracking",
        ":steinhaus_johnson_trotter",
        "@com_google_absl//absl/flags:flag",
    ],
)

cc_test(
    name = "class_permuter_test",
    srcs = ["class_permuter_test.cc"],
    deps = [
        ":class_permuter",
        ":factorial_radix",
        ":factorial_radix_delete_tracking",
        ":steinhaus_johnson_trotter",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_test(
    name = "class_permuter_benchmark",
    srcs = ["class_permuter_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":factorial_radix",
        ":factorial_radix_delete_tracking",
        ":steinhaus_johnson_trotter",
        "@com_github_google_benchmark//:benchmark",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "value_skip_to_active_set",
    srcs = ["value_skip_to_active_set.cc"],
    hdrs = ["value_skip_to_active_set.h"],
    visibility = [
        "//puzzle:__subpackages__",
    ],
    deps = [
        "//puzzle/active_set",
        "//puzzle/class_permuter",
    ],
)

cc_test(
    name = "value_skip_to_active_set_test",
    srcs = ["value_skip_to_active_set_test.cc"],
    deps = [
        ":value_skip_to_active_set",
        "//puzzle/class_permuter:factory",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
