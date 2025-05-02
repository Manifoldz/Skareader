#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "formatter.h"
#include "html_formatter.h"
#include "libs/cjson/cJSON.h"
#include "txt_formatter.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [-h] <path_to_json_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int is_html = 0;
  int arg_index = 1;

  if (strcmp(argv[1], "-h") == 0) {
    is_html = 1;
    arg_index++;
  }

  if (argc <= arg_index) {
    fprintf(stderr, "Missing file path after options\n");
    return EXIT_FAILURE;
  }

  const char *filePath = argv[arg_index];
  FILE *jsonFile = fopen(filePath, "rb");
  if (!jsonFile) {
    perror("Cannot open the provided file");
    return EXIT_FAILURE;
  }

  fseek(jsonFile, 0, SEEK_END);
  long len = ftell(jsonFile);
  fseek(jsonFile, 0, SEEK_SET);

  char *data = malloc(len + 1);
  if (!data) {
    perror("Memory allocation error");
    fclose(jsonFile);
    return EXIT_FAILURE;
  }

  if (fread(data, 1, len, jsonFile) != (size_t)len) {
    perror("File read error");
    free(data);
    fclose(jsonFile);
    return EXIT_FAILURE;
  }
  data[len] = '\0';
  fclose(jsonFile);

  cJSON *root = cJSON_Parse(data);
  if (!root) {
    fprintf(stderr, "JSON parse error\n");
    free(data);
    return EXIT_FAILURE;
  }

  cJSON *convArr = cJSON_GetObjectItemCaseSensitive(root, "conversations");
  if (!cJSON_IsArray(convArr)) {
    fprintf(stderr, "Error: 'conversations' field not found or not an array\n");
    cJSON_Delete(root);
    free(data);
    return EXIT_FAILURE;
  }

  const Formatter *formatter = is_html ? &html_formatter : &txt_formatter;

  cJSON *conv = NULL;
  cJSON_ArrayForEach(conv, convArr) { WriteConversation(conv, formatter); }

  cJSON_Delete(root);
  free(data);
  return EXIT_SUCCESS;
}
