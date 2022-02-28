clang                                               \
  -I ./libs/tree-sitter/lib/include                        \
  -I ./libs/tree-sitter/lib/src                            \
  -ljansson \
  -lm \
  roseta.c                                        \
  ../tree-sitter-roseta-javascript/src/parser.c        \
  ../tree-sitter-roseta-javascript/src/scanner.c       \
  ./libs/tree-sitter-visitor/tree-sitter-visitor.c  \
  ./libs/tree-sitter/lib/src/lib.c                  \
  -o roseta-javascript

