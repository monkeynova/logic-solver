workspace(name="com_monkeynova_logic_solver")

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # Latest on 2017.11.21
    commit = "8866af0386d73cddec01918f9448dd8bfebe4452",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    # Latest on 2017.11.21
    commit = "6a88b407715315e99b372baa82a01f26608091dc",
)

git_repository(
    name = "com_github_gflags_gflags",
    remote = "https://github.com/gflags/gflags.git",
    # Latest on 2017.11.23
    commit = "77592648e3f3be87d6c7123eb81cbad75f9aef5a",
)

http_archive(
    name = "com_google_protobuf",
    urls = ["https://github.com/google/protobuf/archive/b4b0e304be5a68de3d0ee1af9b286f958750f5e4.zip"],
    strip_prefix = "protobuf-b4b0e304be5a68de3d0ee1af9b286f958750f5e4",
    sha256 = "ff771a662fb6bd4d3cc209bcccedef3e93980a49f71df1e987f6afa3bcdcba3a",
)

http_archive(
    name = "com_google_protobuf_cc",
    urls = ["https://github.com/google/protobuf/archive/b4b0e304be5a68de3d0ee1af9b286f958750f5e4.zip"],
    strip_prefix = "protobuf-b4b0e304be5a68de3d0ee1af9b286f958750f5e4",
    sha256 = "ff771a662fb6bd4d3cc209bcccedef3e93980a49f71df1e987f6afa3bcdcba3a",
)

new_git_repository(
    name = "com_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # Latest on 2017.12.03
    commit = "0bbaeeaf7a557469c1f672ee3b7ca926e1a13e88",
    build_file = "external/BUILD.benchmark",
)

new_git_repository(
    name = "com_google_gperftools",
    remote = "https://github.com/gperftools/gperftools.git",
    # Latest on 2017.12.04
    commit = "6a4b0799975576c2722daa75b35f6f65220d22b6",
    build_file = "external/BUILD.gperftools",
)
