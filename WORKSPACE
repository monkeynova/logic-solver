workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "a4621888650aaae118de93f9faf8ef5a18cab303",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "60d00a5822bb98f18e40b294554f91ca14fb793a",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "5d5a6c550100fa9d19f18861b61be9343e94dbcb",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "ce66f6047db44df3234bccd6fcf468bccc14a760",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "8a71927d74a4ce34efe2d8769fda198f52d20d12",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "af72911f2fe6b8114564614d2db17a449f8c4af0",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "db3a28f45e7dbe8c3fbecc7712212db911b88fac",
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
    commit = "f39e808e2cc26a61abe0853b20bb2b079e150818",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "d0561d4d24117c2f3893e6fa947f1bde8c0719af",
)
