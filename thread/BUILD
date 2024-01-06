package(default_visibility = ["//visibility:public"])

cc_library(
    name = "executor",
    hdrs = ["executor.h"],
    deps = [
        ":future",
    ],
)

cc_library(
    name = "inline_executor",
    hdrs = ["inline_executor.h"],
    deps = [
        ":executor",
    ],
)

cc_test(
    name = "inline_executor_test",
    srcs = ["inline_executor_test.cc"],
    deps = [
        ":inline_executor",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "pool",
    hdrs = ["pool.h"],
    deps = [
        ":executor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "pool_test",
    srcs = ["pool_test.cc"],
    deps = [
        ":pool",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)

cc_library(
    name = "future",
    hdrs = ["future.h"],
    deps = [
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/synchronization",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_test(
    name = "future_test",
    srcs = ["future_test.cc"],
    deps = [
        ":future",
        ":pool",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
