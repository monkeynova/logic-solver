def puzzle_test(name, deps, tags = [], **kwargs):
  tags = tags + ["benchmark"]
  deps = deps + ["//puzzle:puzzle_test"]
  native.cc_test(
    name = name,
    deps = deps,
    tags = tags,
    **kwargs
  )
