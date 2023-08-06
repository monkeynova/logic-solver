workspace(name="com_monkeynova_logic_solver")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
   name = "com_monkeynova_base_workspace",
   remote = "https://github.com/monkeynova/base-workspace.git",
   # branch = "main",
   commit = "079048a672da87d2cede89febac49f54a5cbb165",
)

git_repository(
    name = "rules_proto",
    remote = "https://github.com/bazelbuild/rules_proto.git",
    commit = "493169c1199dc21b9da860f7040a4502aa174676",
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

git_repository(
    name = "com_google_googletest",
    remote = "https://github.com/google/googletest.git",
    # branch = "main",
    commit = "455fcb7773dedc70ab489109fb12d8abc7fd59b6",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    commit = "70172ada858b8739ce07e8c2f1ecd8c11c8768c7",
)

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main",
    commit = "5e03386555544e39c21236dca0097123edec8769",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

git_repository(
    name = "com_google_protobuf_cc",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    # branch = "main"
    commit = "5e03386555544e39c21236dca0097123edec8769",
)

git_repository(
    name = "com_github_google_benchmark",
    remote = "https://github.com/google/benchmark.git",
    # branch = "main"
    commit = "02a354f3f323ae8256948e1dc77ddcb1dfc297da",
)

git_repository(
    name = "com_google_tcmalloc",
    remote = "https://github.com/google/tcmalloc.git",
    commit = "4c2f6e64e5345dc004023f2202ab358299634a0e",
)

git_repository(
    name = "com_monkeynova_gunit_main",
    remote = "https://github.com/monkeynova/gunit-main.git",
    # branch = "main"
    commit = "7008f2cc0f8639d49282bc6a5bd46cdd206c4ab7",
)

git_repository(
    name = "com_googlesource_code_re2",
    remote = "https://github.com/google/re2.git",
    # branch = "abseil",
    commit = "578843a516fd1da7084ae46209a75f3613b6065e",
)

git_repository(
    name = "com_google_file_based_test_driver",
    remote = "https://github.com/google/file-based-test-driver.git",
    commit = "fd7661b168f640f68da39f97dad26e426eb6c339",
)

git_repository(
    name = "aspect_bazel_lib",
    remote = "https://github.com/aspect-build/bazel-lib.git",
    # branch = "main",
    commit = "e54912501619f34b7b2172a428ba0c4ebc2331af",
)
