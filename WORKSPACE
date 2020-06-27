workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "aee0f9d9b5b87796ee8a0ab26b7587ec30e8858e",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "b86fff162e15ad8ee534c25e58bf522330e8376d",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "f7388c6655e699f777a5a74a3c9880b9cfaabe59",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "093faebcdb007f5dfaf11a98d61149f89e8bac23",
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
    commit = "39b6e703f8cdf87284db2aaca2f9b214f02e5673",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "8738f271bd58a0decb358fd6047f3bfac1be3382",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "e5f77d6485bd2f6ce43862e3e57118b1bb97d30a",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "git://github.com/google/glog.git",
    commit = "0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
)
