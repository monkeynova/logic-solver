
def make_in_rule(name, hdr):
    native.genrule(
        name = name,
        srcs = [hdr + ".in"],
        outs = [hdr],
        cmd  = (
            "awk '{ " +
            "gsub(/@GFLAGS_NAMESPACE@/, \"google\"); " +
            "gsub(/@(HAVE_STDINT_H|HAVE_SYS_TYPES_H|HAVE_INTTYPES_H|GFLAGS_INTTYPES_FORMAT_C99)@/, \"1\"); " +
            "gsub(/@([A-Z0-9_]+)@/, \"0\"); " +
            "print; }' $(<) > $(@)"
        ),
    )
