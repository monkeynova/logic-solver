cc_library(
    name = "active_set",
    hdrs = ["active_set.h"],
    deps = [
        ":bit_vector",
        ":run_length",
        ":run_position",
    ],
    visibility = [
        "//puzzle:__subpackages__",
    ],
)

cc_library(
    name = "bit_vector",
    srcs = ["bit_vector.cc"],
    hdrs = ["bit_vector.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "run_length",
    srcs = ["run_length.cc"],
    hdrs = ["run_length.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "run_position",
    srcs = ["run_position.cc"],
    hdrs = ["run_position.h"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_set",
        "@com_google_absl//absl/log",
        "@com_google_absl//absl/log:check",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
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
    name = "pair",
    srcs = ["pair.cc"],
    hdrs = ["pair.h"],
    deps = [
        ":active_set",
        "@com_google_absl//absl/container:flat_hash_map",
    ],
    visibility = [
        "//puzzle:__subpackages__",
    ],
)

cc_test(
    name = "pair_test",
    srcs = ["pair_test.cc"],
    deps = [
        ":pair",
        "@com_monkeynova_gunit_main//:test_main",
    ],
)
