gcc -Wall -I tree-sitter/lib/include \
  -I ./libs/tree-sitter/lib/include                        \
  -I ./libs/tree-sitter/lib/src                            \
  -pg \
  -ggdb \
  roseta.c                                        \
  ./libs/tree-sitter-visitor/tree-sitter-visitor.c  \
  ../tree-sitter-roseta-javascript/src/parser.c        \
  ../tree-sitter-roseta-javascript/src/scanner.c       \
  ./libs/tree-sitter/lib/src/lib.c                  \
  -ljansson \
  -lm \
  -o roseta-javascript

