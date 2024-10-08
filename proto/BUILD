cc_library(
    name = "proto_problem",
    srcs = ["proto_problem.cc"],
    hdrs = ["proto_problem.h"],
    deps = [
        "//puzzle:problem",
        "@protobuf//:protobuf",
    ],
)

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
        ":proto_problem",
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

cc_test(
    name = "dracula_and_friends_test",
    tags = ["benchmark"],
    deps = [
        ":dracula_and_friends_lib",
        "//puzzle:puzzle_test",
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
        ":proto_problem",
        ":six_fearsome_heroes_cc_proto",
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
        ":proto_problem",
        ":swimming_pool_cc_proto",
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

cc_test(
    name = "swimming_pool_test",
    tags = ["benchmark"],
    deps = [
        ":swimming_pool_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "swimming_pool_brute_test",
    args = ["--puzzle_solution_permuter=brute"],
    tags = ["benchmark"],
    deps = [
        ":swimming_pool_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "swimming_pool_allowonly_test",
    args = ["--puzzle_solution_permuter=allowonly"],
    tags = ["benchmark"],
    deps = [
        ":swimming_pool_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "swimming_pool_noprune_test",
    args = ["--puzzle_prune_class_iterator=false"],
    tags = ["benchmark"],
    deps = [
        ":swimming_pool_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "six_fearsome_heroes_test",
    tags = ["benchmark"],
    deps = [
        ":six_fearsome_heroes_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "six_fearsome_heroes_noprune_test",
    args = ["--puzzle_prune_class_iterator=false"],
    tags = [
        "benchmark",
        "slowbenchmark",
    ],
    deps = [
        ":six_fearsome_heroes_lib",
        "//puzzle:puzzle_test",
    ],
)

cc_test(
    name = "six_fearsome_heroes_noreorder_test",
    args = [
        "--puzzle_prune_reorder_classes=false",
    ],
    tags = ["benchmark"],
    deps = [
        ":six_fearsome_heroes_lib",
        "//puzzle:puzzle_test",
    ],
)
