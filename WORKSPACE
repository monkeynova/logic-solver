workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "6e5d9f0dde4f3882b9a3bf48ca0f7adb169d9ba2",
)

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "9d21db9e0a60a1ea61ec19331c9bc0dd33e907b1",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "48f72c227b94b06387106f71d4450b31e88e283b",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    commit = "986e8eed00ded8168ef4eaa6f925dc6be50b40fa",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main",
    commit = "7f02c3b16bb76c6928f91b025cdce8f59fa8fc06",
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
    commit = "31de29b07785394bcbeccbb238a54d1814432e2f",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "https://github.com/gperftools/gperftools.git",
    commit = "fe85bbdf4cb891a67a8e2109c1c22a33aa958c7e",
    build_file = "@//third_party:BUILD.gperftools",
)

git_repository(
    name = "com_google_glog",
    remote = "https://github.com/google/glog.git",
    commit = "b33e3bad4c46c8a6345525fd822af355e5ef9446",
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
    commit = "6260193e468194afca94e6717ec76bced9b507a8",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
