workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "a781fe29bcf73003559a3583167fe3d647518464",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "2c8a5b0d890cfbd2c1e70163e347f3e00b4ddb49",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "f7388c6655e699f777a5a74a3c9880b9cfaabe59",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "6b1d0f11493ef2317ea74aa00804835a10ebaf19",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolobuffers/protobuf.git",
    commit = "093faebcdb007f5dfaf11a98d61149f89e8bac23",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "37177a84b7e8d33696ea1e1854513cb0de3b4dc3",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "9dfdae4dabe1e6804ee60f519ac953e6836c81cc",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "bda3c82e11615ca9e7751d1f3cfb161026ee742a",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "git://github.com/google/glog.git",
    commit = "0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
)
