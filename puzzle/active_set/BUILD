cc_library(
    name = "active_set",
    hdrs = ["active_set.h"],
    visibility = [
        "//puzzle:__subpackages__",
    ],
    deps = [
        ":bit_vector",
        ":run_length",
        ":run_position",
    ],
)

cc_library(
    name = "bit_vector",
    srcs = ["bit_vector.cc"],
    hdrs = ["bit_vector.h"],
    deps = [
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "run_length",
    srcs = ["run_length.cc"],
    hdrs = ["run_length.h"],
    deps = [
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@com_monkeynova_gunit_main//:vlog",
    ],
)

cc_library(
    name = "run_position",
    srcs = ["run_position.cc"],
    hdrs = ["run_position.h"],
    deps = [
        "@abseil-cpp//absl/container:flat_hash_set",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
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
    visibility = [
        "//puzzle:__subpackages__",
    ],
    deps = [
        ":active_set",
        "@abseil-cpp//absl/container:flat_hash_map",
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
