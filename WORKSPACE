workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "adeef192947fbc0f68fa14a6c494c8df32177508",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "d0c433455801e1c1fb6f486f0b447e22f946ab52",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "f7388c6655e699f777a5a74a3c9880b9cfaabe59",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "8a71927d74a4ce34efe2d8769fda198f52d20d12",
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
    commit = "1302d2ce094a9753b0f81a81ea74c0fa71fae582",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "e519fb74134b73557a1a0c8a0590abdce9fe883b",
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

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "git://github.com/monkeynova/gunit-main.git",
    commit = "e3e52e54ef31b01949ff3e4541397e9431d16dfe",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "git://github.com/google/re2.git",
    # Must be on branch 'abseil'.
    commit = "bd32b6b147832e87fb06d0600bb7de5d8a80b5c1",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "d0561d4d24117c2f3893e6fa947f1bde8c0719af",
)
