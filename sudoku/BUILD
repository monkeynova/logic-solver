cc_library(
    name = "base",
    srcs = ["base.cc"],
    hdrs = ["base.h"],
    deps = [
        "//puzzle:problem",
    ],
)

cc_binary(
    name = "nyt20171202",
    srcs = ["nyt20171202.cc"],
    deps = [
        "//puzzle:puzzle_main",
        ":base",
    ],
)

cc_binary(
    name = "nyt20180101easy",
    srcs = ["nyt20180101easy.cc"],
    deps = [
        "//puzzle:puzzle_main",
        ":base",
    ],
)

