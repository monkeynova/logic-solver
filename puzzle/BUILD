cc_library(
    name = "solver",
    srcs = ["solver.cc"],
    hdrs = ["solver.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//puzzle/base:all_match",
        "//puzzle/base:owned_solution",
        "//puzzle/base:profiler",
        "//puzzle/base:solution_view",
        "//puzzle/solution_permuter",
        "//puzzle/solution_permuter:solution_permuter_factory",
        "@com_google_absl//absl/status",
    ],
)

cc_library(
    name = "problem",
    srcs = ["problem.cc"],
    hdrs = ["problem.h"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":solver",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "puzzle_main",
    srcs = ["puzzle_main.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":problem",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_monkeynova_gunit_main//:main_lib",
    ],
)

cc_library(
    name = "puzzle_test",
    testonly = 1,
    srcs = ["puzzle_test.cc"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":problem",
        "@com_github_google_benchmark//:benchmark",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest",
        "@com_monkeynova_gunit_main//:test_main",
    ],
    alwayslink = 1,
)

genquery(
    name = "bad_deps",
    # Some dependency path from //puzzle/base to elsewhere in //puzzle.
    expression =
        "let all_targets = deps(//puzzle:puzzle_main) in " +
        'let base_targets = filter("//puzzle/base", $all_targets) in ' +
        'somepath($base_targets, filter("//puzzle", $all_targets) - $base_targets)',
    scope = [":puzzle_main"],
)

sh_test(
    name = "bad_deps_test",
    srcs = ["bad_deps_test.sh"],
    data = [":bad_deps"],
)
