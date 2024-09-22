workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "079048a672da87d2cede89febac49f54a5cbb165",
)

git_repository(
    name = "rules_proto",
    remote = "https://github.com/bazelbuild/rules_proto.git",
    # branch = "main",
    commit = "c9731b887d4b897a6fe14367c8e3f4e225f45f53",
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "116b7e55281c4200151524b093ecc03757a4ffda",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "a0b72adc3576eb0b77efb7133207c354d0adb4bc",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main",
    commit = "77aa913e689304329b6e3df71eea9944acfbc62f",
)

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main"
    commit = "77aa913e689304329b6e3df71eea9944acfbc62f",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main"
    commit = "7495f83e2a6e1aa69592fcda6e5c6c1b0b4fa118",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "https://github.com/google/tcmalloc.git",
    commit = "7ce63022a776b369e776b248c76e157fda237ad0",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "8b0dec052e1aacd71f91aade4530aae6a452b0e5",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil",
    commit = "578843a516fd1da7084ae46209a75f3613b6065e",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)

git_repository(
    name = "aspect_bazel_lib",
    remote = "https://github.com/aspect-build/bazel-lib.git",
    # branch = "main",
    commit = "8cf7e6f995b43641f41e9dd9a6b314684a7c0c9a",
)

load("@protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

load("@googletest//:googletest_deps.bzl", "googletest_deps")
googletest_deps()

load("@google_benchmark//:bazel/benchmark_deps.bzl", "benchmark_deps")
benchmark_deps()
