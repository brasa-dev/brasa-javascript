for dialect in ../brasa-javascript-dialects/dialects/*; do
  [ ! -d $dialect ] && continue

  DIALECT=${dialect##../brasa-javascript-dialects/dialects/}
  echo $DIALECT

  gcc -Wall -I tree-sitter/lib/include \
    -I ./libs/tree-sitter/lib/include                        \
    -I ./libs/tree-sitter/lib/src                            \
    -pg \
    -ggdb \
    roseta-javascript.c                                        \
    ./libs/tree-sitter-visitor/tree-sitter-visitor.c  \
    ../tree-sitter-javascript/dialects/tree-sitter-roseta-javascript-$DIALECT/src/parser.c        \
    ../tree-sitter-javascript/dialects/tree-sitter-roseta-javascript-$DIALECT/src/scanner.c       \
    ./libs/tree-sitter/lib/src/lib.c                  \
    -ljansson \
    -lm \
    -o roseta-javascript-$DIALECT

done
