#include <string.h>
#include <stdio.h>
#include <math.h>
#include <jansson.h>
#include "libs/tree-sitter/lib/include/tree_sitter/api.h"
#include "libs/tree-sitter-visitor/tree-sitter-visitor.h"
/* #include <node_api.h> */

char * VERSION = "v0.0.1";
int debug = 0;
int BEAUTIFY = 0;
int KEEP_COMMENTS = 0;
FILE *new_stdout;
json_t *dialect;
json_t *lexicon;
json_t *identifiers;

void node_text (TSNode node, struct visit_context * context) {
  if (strcmp(ts_node_type(node), ts_node_text(node, context)) == 0) {
    const char * keyword = json_string_value(json_object_get(lexicon, ts_node_text(node, context)));
    if (keyword != NULL) {
      printf("%s ", keyword);
    } else {
      printf("%s", ts_node_text(node, context));
    }
  } else if (
      strcmp(ts_node_type(node), "identifier") == 0
  ) {
    const json_t * id = json_object_get(identifiers, ts_node_text(node, context));
    if (id != NULL) {
      const char * text = json_string_value(json_object_get(id, "value"));
      printf("%s", text);
    } else {
      printf("%s", ts_node_text(node, context));
    }
  } else if (
      strcmp(ts_node_type(node), "property_identifier") == 0
  ) {
    // go up until no more member_expression 
    TSNode member = node;
    TSNode seeker = ts_node_parent(node);
    while(!ts_node_is_null(seeker) && strcmp(ts_node_type(seeker), "member_expression") == 0) {
      member = seeker;
      seeker = ts_node_parent(seeker);
    };

    // go down until identifier
    seeker = ts_node_child(member, 0);
    while(!ts_node_is_null(seeker) && strcmp(ts_node_type(seeker), "identifier") != 0) {
      member = seeker;
      seeker = ts_node_child(member, 0);
    };

    // we have identifier obj
    json_t * curIdObj = json_object_get(identifiers, ts_node_text(seeker, context));

    // go up and right translating until reach node
    TSNode propId = seeker;

    do {
      member = ts_node_parent(seeker);
      propId = ts_node_child(member, 2);
      curIdObj = json_object_get(curIdObj, "properties");
      curIdObj = json_object_get(curIdObj, ts_node_text(propId, context));
      seeker = member;
    } while(!ts_node_eq(propId, node));

    if (curIdObj != NULL) {
      const char * text = json_string_value(json_object_get(curIdObj, "value"));
      printf("%s", text);
    } else {
      printf("%s", ts_node_text(node, context));
    }
  } else if (
      strcmp(ts_node_type(node), "regex_pattern") == 0
      || strcmp(ts_node_type(node), "statement_identifier") == 0
      || strcmp(ts_node_type(node), "shorthand_property_identifier") == 0
      || strcmp(ts_node_type(node), "regex_flags") == 0
      || strcmp(ts_node_type(node), "number") == 0
  ) {
    printf("%s", ts_node_text(node, context));
  }
}

int parse_file(int argc, char * argv[]) {
  int i;
  char * output_path = NULL;
  char * config_path = NULL;
  int last_arg = 0;
  for (i = 0; i < argc; i++) {
    char * arg = argv[i];
    if (strcmp("-v", arg) == 0 || strcmp("--version", arg) == 0) {
      printf("@lotuz/roseta-javascript%s\n", VERSION);
      return 0;
    } else if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0) {
      printf("Usage: roseta [OPTIONS] [FILE]\n");
      printf("\n");
      printf("Options: \n");
      printf("\t -h, --help: For printing roseta help\n");
      printf("\t -v, --version: For printing roseta version\n");
      printf("\t -d, --debug: For debugging, also helpful for bug report\n");
      printf("\t -o, --output: Sets the output file\n");
      printf("\t -c, --config: Sets the dialog config file\n");
      return 0;
    } else if (strcmp("-d", arg) == 0 || strcmp("--debug", arg) == 0) {
      debug = 1;
      last_arg = i;
    } else if (strcmp("-o", arg) == 0 || strcmp("--output", arg) == 0) {
      if (i + 1 < argc) {
        output_path = argv[i + 1];
        i++;
      } else {
        printf("Missing output file path\n");
        return 1;
      }
      last_arg = i;
    } else if (strcmp("-c", arg) == 0 || strcmp("--config", arg) == 0) {
      if (i + 1 < argc) {
        config_path = argv[i + 1];
        i++;
      } else {
        printf("Missing config file path\n");
        return 1;
      }
      last_arg = i;
    }
  }
  size_t buffer_size = BUFSIZ;
  char * buffer = malloc(buffer_size);
  if (output_path != NULL) {
    printf("Missing config file path\n");
    return 1;
  }

  if (output_path != NULL) {
    new_stdout = freopen(output_path, "w", stdout);
    if(new_stdout == NULL) {
       perror("fopen"); 
       return 1;
    }
  } else {
    new_stdout = stdout;
  }

  setvbuf(new_stdout, buffer, _IOFBF, buffer_size);

  char * file_path = argv[argc - 1];
  if (file_path == NULL) {
    printf("No files passed...\n");
    return 1;
  }
  const char *source_code = get_source(file_path);
  const char *source_json = get_source(config_path);

  json_error_t error;
  dialect = json_loads(source_json, 0, &error);

  if(!dialect) {
    fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    return 1;
  }

  lexicon = json_object_get(dialect, "lexicon");
  identifiers = json_object_get(dialect, "identifiers");

  TSLanguage *tree_sitter_roseta_javascript();
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_roseta_javascript());

  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  TSNode root_node = ts_tree_root_node(tree);

  struct visit_context *context = context_new(tree_sitter_roseta_javascript(), source_code, debug);

  context_set_global_visitor(context, node_text, NULL);

  visit_tree(root_node, context);
  printf("\n");

  context_delete(context);
  ts_tree_delete(tree);
  ts_parser_delete(parser);

  fflush(new_stdout);
  free(buffer);
  return 0;
}

int main (int argc, char * argv[]) {
  parse_file(argc, argv);
}

/* napi_value jsminify (napi_env env, napi_callback_info cbinfo) { */
/*   size_t argc = 0; */
/*   napi_get_cb_info(env, cbinfo, &argc, NULL, NULL, NULL); */

/*   napi_value *argv = (napi_value *) malloc(argc * sizeof(uintptr_t)); */
/*   napi_get_cb_info(env, cbinfo, &argc, argv, NULL, NULL); */

/*   if (argc < 1) { */
/*     napi_throw_error(env, "EINVAL", "Too few arguments"); */
/*     return NULL; */
/*   } */

/*   char ** args = malloc(sizeof(char *) * argc); */
/*   size_t str_len = 1024; */
/*   for (size_t i = 0; i < argc; i++) { */
/*     args[i] = malloc(str_len); */
/*     if(napi_get_value_string_utf8(env, argv[i], (char *) args[i], 1024, &str_len) != napi_ok) { */
/*       napi_throw_error(env, "EINVAL", "Expected string"); */
/*       return NULL; */
/*     } */
/*   } */

/*   parse_file(argc, args); */
/*   for (size_t i = 0; i < argc; i++) */
/*     free(args[i]); */
/*   free(args); */

/*   return NULL; */
/* } */

/* napi_value init_all (napi_env env, napi_value exports) { */
/*   napi_value jsminify_fn; */
/*   napi_create_function(env, NULL, 0, jsminify, NULL, &jsminify_fn); */
/*   napi_set_named_property(env, exports, "jsminify", jsminify_fn); */
/*   return exports; */
/* } */

/* NAPI_MODULE(NODE_GYP_MODULE_NAME, init_all) */
