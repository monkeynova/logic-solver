load("@com_monkeynova_base_workspace//:default_rules.bzl", "default_rules")
load("//puzzle:puzzle.bzl", "puzzle_test")

default_rules(workspace_dep = "update_workspace.date")

cc_binary(
    name = "dracula_and_friends",
    deps = [
        ":dracula_and_friends_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "dracula_and_friends_lib",
    srcs = ["dracula_and_friends.cc"],
    deps = [
        ":dracula_and_friends_cc_proto",
        "//puzzle:proto_problem",
    ],
    alwayslink = 1,
)

cc_proto_library(
    name = "dracula_and_friends_cc_proto",
    deps = [":dracula_and_friends_proto"],
)

proto_library(
    name = "dracula_and_friends_proto",
    srcs = ["dracula_and_friends.proto"],
)

puzzle_test(
    name = "dracula_and_friends_test",
    deps = [
        ":dracula_and_friends_lib",
    ],
)

cc_binary(
    name = "six_fearsome_heroes",
    deps = [
        ":six_fearsome_heroes_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_proto_library(
    name = "six_fearsome_heroes_cc_proto",
    deps = [":six_fearsome_heroes_proto"],
)

proto_library(
    name = "six_fearsome_heroes_proto",
    srcs = ["six_fearsome_heroes.proto"],
)

cc_library(
    name = "six_fearsome_heroes_lib",
    srcs = ["six_fearsome_heroes.cc"],
    deps = [
        ":six_fearsome_heroes_cc_proto",
        "//puzzle:proto_problem",
    ],
    alwayslink = 1,
)

cc_binary(
    name = "swimming_pool",
    deps = [
        ":swimming_pool_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "swimming_pool_lib",
    srcs = ["swimming_pool.cc"],
    deps = [
        ":swimming_pool_cc_proto",
        "//puzzle:proto_problem",
    ],
    alwayslink = 1,
)

cc_proto_library(
    name = "swimming_pool_cc_proto",
    deps = [":swimming_pool_proto"],
)

proto_library(
    name = "swimming_pool_proto",
    srcs = ["swimming_pool.proto"],
)

puzzle_test(
    name = "swimming_pool_test",
    deps = [
        ":swimming_pool_lib",
    ],
)

puzzle_test(
    name = "swimming_pool_brute_test",
    args = ["--puzzle_brute_force=true"],
    deps = [
        ":swimming_pool_lib",
    ],
)

puzzle_test(
    name = "swimming_pool_noprune_test",
    args = ["--puzzle_prune_class_iterator=false"],
    deps = [
        ":swimming_pool_lib",
    ],
)

puzzle_test(
    name = "six_fearsome_heroes_test",
    deps = [
        ":six_fearsome_heroes_lib",
    ],
)

puzzle_test(
    name = "six_fearsome_heroes_noprune_test",
    args = ["--puzzle_prune_class_iterator=false"],
    tags = ["slowbenchmark"],
    deps = [
        ":six_fearsome_heroes_lib",
    ],
)

puzzle_test(
    name = "six_fearsome_heroes_noreorder_test",
    args = [
        "--puzzle_prune_reorder_classes=false",
    ],
    deps = [
        ":six_fearsome_heroes_lib",
    ],
)

cc_binary(
    name = "nytkenken20240504_small",
    deps = [
        ":nytkenken20240504_small_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nytkenken20240504_small_lib",
    srcs = ["nytkenken20240504_small.cc"],
    deps = [
        "//puzzle:problem",
        "@com_monkeynova_gunit_main//:vlog"
    ],
    alwayslink = 1,
)

puzzle_test(
    name = "nytkenken20240504_small_test",
    deps = [
        ":nytkenken20240504_small_lib",
    ],
)

cc_binary(
    name = "nytkenken20240504_large",
    deps = [
        ":nytkenken20240504_large_lib",
        "//puzzle:puzzle_main",
    ],
)

cc_library(
    name = "nytkenken20240504_large_lib",
    srcs = ["nytkenken20240504_large.cc"],
    deps = [
        "//puzzle:problem",
        "@com_monkeynova_gunit_main//:vlog"
    ],
    alwayslink = 1,
)

puzzle_test(
    name = "nytkenken20240504_large_test",
    deps = [
        ":nytkenken20240504_large_lib",
    ],
)
