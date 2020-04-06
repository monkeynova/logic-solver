def sudoku_test(name, deps, tags = [], **kwargs):
  tags = tags + ["benchmark"]
  deps = deps + ["//sudoku:sudoku_test"]
  native.cc_test(
    name = name,
    deps = deps,
    tags = tags,
    **kwargs
  )
