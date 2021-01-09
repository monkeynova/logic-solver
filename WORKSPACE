workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "389cb68b87193358358ae87cc56d257fd0d80189",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    # no_update = "https://github.com/abseil/abseil-cpp/issues/848",
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
    commit = "7165c34606aa00f3841a5e8a3f4586023bb59ade",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "7165c34606aa00f3841a5e8a3f4586023bb59ade",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "8df87f6c879cbcabd17c5cfcec7b89687df36953",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "9d274df38333479d5ae6acaddc29172fd134d885",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "d8eb315fb18f6fb0d6efa923401f166343727bc6",
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
    commit = "0123eef05b595cf41eb180f826304d9825789088",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "git://github.com/google/re2.git",
    # branch = "abseil",
    commit = "29e204fb8dccb2c1b5c957f2d23ed403cc5a92dc",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "5074f48f03c6a892edafab55410addc43f4a0546",
)
