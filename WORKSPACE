workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "21c3632e277633820f20a81b084e3c764c243a97",
)

git_repository(
    name = "rules_proto",
    remote = "https://github.com/bazelbuild/rules_proto.git",
    commit = "066581b14bcbf87206c89ff5fcdd6f9915fd30fb",
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "afd902e992b720d1b3e106bc5e425a5768872265",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "92fdbfb301f8b301b28ab5c99e7361e775c2fb8a",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    commit = "a738fdf9338412f83ab3f26f31ac11ed3f3ec4bd",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main",
    commit = "66ed6dd75b0b5f0d62a9bf90509638f380cbcd7f",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main"
    commit = "66ed6dd75b0b5f0d62a9bf90509638f380cbcd7f",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    commit = "e991355c02b93fe17713efe04cbc2e278e00fdbd",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "https://github.com/google/tcmalloc.git",
    commit = "30b53b78cbfbfd2e9adee5f51c7eeeb077b51fd7",
)

git_repository(
    name = "com_google_glog",
    remote = "https://github.com/google/glog.git",
    commit = "b70ea80433c2a8f20b832be97b90f1f82b0d29e9",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "537db9c23114a3d2c300ffd4681f194c1f1b547c",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil",
    commit = "8c0f7738d67b1808f9bb9a93a5cdc6d33d50ede9",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
