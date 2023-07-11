# brasa-javascript

Brasa is a project that intends to help people learn programming languages with
keywords and identifiers in their own mother language. brasa-javascript is a
javascript compiler build on top of tree-sitter and tree-sitter-javascript that
takes json files as configuration and returns code in another mother language,
which gets called as a dialect of the programming language that is being
translated.

As an example the dialect for javascript in brazilian portuguese is called
Brasascript so the compiler can be used to compile from Javascript to
Brasascipt and from Brasascript to Javascript, allowing any code in Brasacript
(or other dialect) to be executed by normal Javascript runtimes.

The project is not yet mature and is mainly intended for educational purposes.

**Use it with caution!**

## Dependencies

- [tree-sitter](https://github.com/tree-sitter/tree-sitter)
- [tree-sitter-javascript](https://github.com/tree-sitter/tree-sitter-javascript)
- [tree-sitter-visitor](https://github.com/marcel0ll/tree-sitter-visitor)
- jansson - libjansson-dev

## Build

When you run the build script it will create a compiler for each dialect.

`./build.sh`

## Usage

`./roseta-javasciprt-{dialect} [OPTIONS] [FILE]`

`./roseta-javascript-pt-br -c from_pt-br.json code.bs`

**FILE must always come last**

### OPTIONS

* -h, --help: For printing jsminify help
* -v, --version: For printing jsminify version
* -d, --debug: For debugging minification, also helpful for bug report
* -o, --output: Sets the output file

## Roadmap

- [ ] 1.0.0
    - [ ] Create translation mapping from Js to Js-pt-br

