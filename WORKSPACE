workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "git://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "839d52e84fa707890a45b0dd9ed7b62ad30fa43e",
)

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "df7fee587d442b372ef43bd66c6a2f5c9af8c5eb",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "a09b5de0d57d7b2179210989ab63361c3c1894f5",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "aedb8d726c8d2a371d999354d44ca7e1bdbb1e73",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "aedb8d726c8d2a371d999354d44ca7e1bdbb1e73",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "cc9abfc8f12577ea83b2d093693ba70c3c0fd2c7",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "340588d3eb2c8f30597fdb43fd7692d2aa2de4e3",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "f7c6fb6c8e99d6b1b725e5994373bcd19ffdf8fd",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "git://github.com/google/glog.git",
    commit = "d4e8ebab7e295f20f86cae9557da0d5087a02f73",
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
    commit = "72f110e82ccf3a9ae1c9418bfb447c3ba1cf95c2",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "77e24638ad40ec67dcbf6e37fd57e20c5d98976e",
)
