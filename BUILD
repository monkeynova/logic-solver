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
        "//puzzle:solver",
	"@com_github_gflags_gflags//:gflags",
        "@com_google_absl//absl/memory",
    ],
)
