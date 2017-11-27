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
        "//puzzle:solver",
    ]
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
