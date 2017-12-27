cmake_defines_osx = {
    "OS_MACOS" : "1",
    "HAVE_DLADDR" : "1",
}

cmake_defines_linux = {
    "HAVE_LIB_UNWIND" : "1",
}

cmake_defines_generic = {
    "_START_GOOGLE_NAMESPACE_" : "namespace google {",
    "_END_GOOGLE_NAMESPACE_" : "}  // namespace google",
    "GOOGLE_NAMESPACE" : "google",
    "HAVE_DLADDR" : "1",
    "HAVE_EXECINFO_H" : "1",
    "HAVE_GLOB_H" : "1",
    "HAVE_LIB_GFLAGS" : "1",
    "HAVE_LIB_GMOCK" : "1",
    "HAVE_LIB_GTEST" : "1",
    "HAVE_PREAD" : "1",
    "HAVE_PWRITE" : "1",
    "HAVE_PTHREAD" : "1",
    "HAVE_SIGALTSTACK" : "1",
    "HAVE_SIGACTION" : "1",
    "HAVE_SYMBOLIZE" : "1",
    "HAVE_SYS_SYSCALL_H" : "1",
    "HAVE_SYS_TIME_H" : "1",
    "HAVE_SYS_UTSNAME_H" : "1",
    "HAVE_UNISTD_H" : "1",
    "TEST_SRC_DIR" : "\\\".\\\"",  # TODO: FixMe!
}

cmake_defines = cmake_defines_generic + cmake_defines_osx

cmake_vars = {
    "ac_google_start_namespace" : "_START_GOOGLE_NAMESPACE_",
    "ac_google_end_namespace" : "_END_GOOGLE_NAMESPACE_",
    "ac_google_namespace" : "GOOGLE_NAMESPACE",
    "ac_cv___attribute___noinline" : "__attribute__((noinline))",
    "ac_cv___attribute___noreturn" : "__attribute__((noreturn))",
    "ac_cv___attribute___printf_4_5" : "__attribute__ ((format (printf, 4, 5)))",
    "ac_cv_have___builtin_expect" : "1",
    "ac_cv_have_inttypes_h" : "1",
    "ac_cv_have_libgflags" : "1",
    "ac_cv_have_stdint_h" : "1",
    "ac_cv_have_systypes_h" : "1",
    "ac_cv_have_uint16_t" : "1",
    "ac_cv_have_unistd_h" : "1",
}

def cmake_config(name, hdr):
    native.genrule(
        name = name,
        srcs = [hdr + ".in"],
        outs = [hdr],
        cmd  = (
            "awk '{ " +
            "\n".join(
                ["gsub(/#undef " + t[0] + "/, \"#define " + t[0] + " " + t[1] + "\");"
                 for t in cmake_defines.items()]) +
            "print; }' $(<) > $(@)"
        )
    )

def cmake_header(name, hdr):
    native.genrule(
        name = name,
        srcs = [hdr + ".in"],
        outs = [hdr],
        cmd  = (
            "awk '{ " +
#            "gsub(/@([A-Za-z0-9_]+)@/, \"0\"); " +
            "\n".join(
                ["gsub(/@" + t[0] + "@/, \"" +
                 (cmake_defines.get(t[1], default=t[1])) + "\"); "
                 for t in cmake_vars.items()]) +
            "print; }' $(<) > $(@)"
        ),
    )
