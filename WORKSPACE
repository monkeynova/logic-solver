workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "dcc92d0ab6c4ce022162a23566d44f673251eee4",
    shallow_since = "1587058390 -0400"
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "cde2e2410e58c884b3bf5f67c6511e6266036249",
    shallow_since = "1587747195 -0400"
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
    commit = "dddeed298bf84505f4d123a068f911025b351661",
    shallow_since = "1587760575 -0400"
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
    commit = "56898e9a92fba537671d5462df9c5ef2ea6a823a",
    shallow_since = "1587640759 +0300"
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
