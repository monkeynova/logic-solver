workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository",
     "new_git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "git://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "4cee9e04fe6ed61bb9c3a653a2d8ca23d987abc2",
)

git_repository(
    name = "com_google_googletest",
    remote = "git://github.com/google/googletest.git",
    commit = "609281088cfefc76f9d0ce82e1ff6c30cc3591e5",
)

git_repository(
    name = "com_google_absl",
    remote = "git://github.com/abseil/abseil-cpp.git",
    commit = "b343ac3a649f5713380cdbeee79d2ee6f94ddd93",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "git://github.com/gflags/gflags.git",
    commit = "827c769e5fc98e0f2a34c47cef953cc6328abced",
)

git_repository(
    name = "com_google_protobuf",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "aee143afe8c17a3d2c7e88b70ffa6e08a73e2683",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "git://github.com/protocolbuffers/protobuf.git",
    commit = "48234f5f012582843bb476ee3afef36cda94cb66",
)

git_repository(
    name = "com_google_benchmark",
    remote = "git://github.com/google/benchmark.git",
    commit = "ea5a5bbff491fd625c6e3458f6edd680b8bd5452",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "git://github.com/google/tcmalloc.git",
    commit = "3cc5f7a36137d8155b882b5171de803b61410c8c",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "git://github.com/gperftools/gperftools.git",
    commit = "3b1c60cc4ea737ff151977dbcd164c5e2eb859fb",
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
    commit = "d6bf30d9dda1785d3614393aea6dd171e04bf361",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "git://github.com/google/file-based-test-driver.git",
    commit = "5074f48f03c6a892edafab55410addc43f4a0546",
)
