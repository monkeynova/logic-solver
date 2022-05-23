workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "c745dcf6695a3f64c8cc39adb3e3d1049ff678b0",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "3d81736c973fbcc938267cb296918bc3266dde12",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "fbbb5865a562c9a9167d71c1cf56b82025a8f065",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    commit = "986e8eed00ded8168ef4eaa6f925dc6be50b40fa",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    commit = "e554bd6cbf8d6325ce13bc0fe822124a81d36008",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    commit = "e554bd6cbf8d6325ce13bc0fe822124a81d36008",
)

git_repository(
    name = "com_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    commit = "e991355c02b93fe17713efe04cbc2e278e00fdbd",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "https://github.com/google/tcmalloc.git",
    commit = "651fe931f11f981bc7e9552dda0aa8faa297f042",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "https://github.com/gperftools/gperftools.git",
    commit = "e80652b6279a22bd4a4264e4403cd1981293e82f",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "https://github.com/google/glog.git",
    commit = "b0174b3dda95be3858ec54570c1375f2f00e984e",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    commit = "0123eef05b595cf41eb180f826304d9825789088",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil",
    commit = "e8cb5ecb8ee1066611aa937a42fa10514edf30fb",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
