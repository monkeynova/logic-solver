cc_library(
    name = "pool",
    hdrs = ["pool.h"],
    deps = [
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
