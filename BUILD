cc_binary(
    name = "six_fearsome_heroes",
    srcs = ["six_fearsome_heroes.cc"],
    deps = [
        ":puzzle_main",
        "//puzzle:solver",
    ]
)

cc_binary(
    name = "swimming_pool",
    srcs = ["swimming_pool.cc"],
    deps = [
        ":puzzle_main",
        ":swimming_pool_cc_proto",
        "//puzzle:solver",
    ]
)

cc_binary(
    name = "sudoku",
    srcs = ["sudoku.cc"],
    deps = [
        ":puzzle_main",
        "//puzzle:solver",
    ],
)

cc_proto_library(
    name = "swimming_pool_cc_proto",
    deps = [":swimming_pool_proto"],
)

proto_library(
    name = "swimming_pool_proto",
    srcs = ["swimming_pool.proto"],
)

cc_library(
    name = "puzzle_main",
    srcs = ["puzzle_main.cc"],
    deps = [
        "//puzzle:instance",
        "//puzzle:solver",
	"@com_github_gflags_gflags//:gflags",
        "@com_google_absl//absl/memory",
    ],
)

cc_test(
    name = "swimming_pool_test",
    srcs = ["swimming_pool.cc"],
    deps = [
        ":puzzle_test",
        ":swimming_pool_cc_proto",
        "@com_google_googletest//:gtest_main",	
    ],
)

cc_test(
    name = "swimming_pool_brute_test",
    srcs = ["swimming_pool.cc"],
    args = ["--puzzle_brute_force=true"],
    deps = [
        ":puzzle_test",
        ":swimming_pool_cc_proto",
        "@com_google_googletest//:gtest_main",	
    ],
)

cc_test(
    name = "six_fearsome_heroes_test",
    srcs = ["six_fearsome_heroes.cc"],
    deps = [
        ":puzzle_test",
        "@com_google_googletest//:gtest_main",	
    ],
)

cc_library(
    name = "puzzle_test",
    srcs = ["puzzle_test.cc"],
    deps = [
        "//puzzle:solver",
        "@com_google_googletest//:gtest",
        "@com_google_absl//absl/memory",
    ],
)
