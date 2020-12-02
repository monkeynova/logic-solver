workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "b1fbd33c06cdb0024c67733c6fdec2009d17b384",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "e9b9e38f67a008d66133535a72ada843bd66013f",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "9637a3b57cacda57187ef48b08d4bc8f83349faf",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "9637a3b57cacda57187ef48b08d4bc8f83349faf",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "bf585a2789e30585b4e3ce6baf11ef2750b54677",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "31d3e9f7bbf12225ae5c1248829655b137c6bc47",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "140e3481d0541044cdc27f45129b47f92d5940b7",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "git://github.com/google/glog.git",
    commit = "c8f8135a5720aee7de8328b42e4c43f8aa2e60aa",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "git://github.com/monkeynova/gunit-main.git",
    commit = "9614aad52ec336f344f719b02d057ee3b133b73c",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "git://github.com/google/re2.git",
    # Must be on branch 'abseil'.
    commit = "5a7c0c986ba0bea21e4a7e0e9c289ddf281fda65",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "5074f48f03c6a892edafab55410addc43f4a0546",
)
