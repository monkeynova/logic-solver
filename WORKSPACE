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
    commit = "b5e5fc85f70cf6bbef66c69b679c86168ad1bea6",
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "71140c3ca7a87bb1b5b9c9f1500fea8858cce344",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "f7affaf32a6a396465507dd10520a3fe183d4e40",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main",
    # no_update = "https://github.com/protocolbuffers/protobuf/issues/11409",
    commit = "66ed6dd75b0b5f0d62a9bf90509638f380cbcd7f",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main"
    # no_update = "https://github.com/protocolbuffers/protobuf/issues/11409",
    commit = "66ed6dd75b0b5f0d62a9bf90509638f380cbcd7f",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main"
    commit = "62edc4fb00e1aeab86cc69c70eafffb17219d047",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "https://github.com/google/tcmalloc.git",
    commit = "927c1433141daa1f0bcf920e6d71bf64795cc2c2",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "12e3a4ffb0ae2c2e4d1f65fbf41cd073ceedd59d",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil",
    commit = "d0b1f8f2ecc2ea74956c7608b6f915175314ff0e",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)
