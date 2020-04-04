workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "61f010d703b32de9bfb20ab90ece38ab2f25977f",
    shallow_since = "1585697018 -0400",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "62f05b1f57ad660e9c09e02ce7d591dcc4d0ca08",
    shallow_since = "1585768791 -0400",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    commit = "addd749114fab4f24b7ea1e0f2f837584389e52c",
    shallow_since = "1584534678 +0000",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    commit = "cf601047ebf87cf7f443753ded41132eb689cb10",
    shallow_since = "1585843636 -0700"
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolobuffers/protobuf.git",
    commit = "422053f3bcb39cac483d2769e936c473e7c8bcdb",
)

git_repository(
    name = "com_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    commit = "b23d35573bb9f33f9b76cadc652bb7ef88c6c64d",
    shallow_since = "1585549357 +0300",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "https://github.com/gperftools/gperftools.git",
    commit = "e5f77d6485bd2f6ce43862e3e57118b1bb97d30a",
    build_file = "third_party/BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "https://github.com/google/glog.git",
    commit = "3ba8976592274bc1f907c402ce22558011d6fc5e",
    shallow_since = "1585127859 +0100",
)
