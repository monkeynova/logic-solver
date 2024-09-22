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
        "@abseil-cpp//absl/status",
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
        "@abseil-cpp//absl/status:statusor",
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
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:usage",
        "@abseil-cpp//absl/memory",
        "@abseil-cpp//absl/strings",
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
        "@google_benchmark//:benchmark",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/memory",
        "@googletest//:gtest",
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
