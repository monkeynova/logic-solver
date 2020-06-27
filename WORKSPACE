workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "859bfe8981d6724c4ea06e73d29accd8588f3230",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "2069dc796aa255f7c82861c6e83b82d001ceef4a",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "addd749114fab4f24b7ea1e0f2f837584389e52c",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "e492e5a4ef16f59010283befbde6112f1995fa0f",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolobuffers/protobuf.git",
    commit = "e492e5a4ef16f59010283befbde6112f1995fa0f",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "6746c65bcfa49110bfe6642b8a47735637817be4",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "bceab40c239be18a947fd37281bd20a378717f69",
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
