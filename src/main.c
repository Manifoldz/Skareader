#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/cjson/cJSON.h"

#define INDENT_QUOTE "    "
#define QUOTE_MARK "&lt;&lt;&lt; "
#define TIMESTAMP "[1234567890] "
#define MAX_EMPTY_LINES 2

typedef struct {
  const char *author;
  const char *time;
  const char *content;
} Msg;

int is_html = 0;

int is_restricted_tag(const char *tag) {
  static char *restricted_tags[] = {"<legacyquote", "<quote ", "</quote",
                                    "</legacyquote"};
  size_t num_tags = sizeof(restricted_tags) / sizeof(restricted_tags[0]);

  for (size_t i = 0; i < num_tags; ++i) {
    if (strncmp(tag, restricted_tags[i], strlen(restricted_tags[i])) == 0) {
      return 1;
    }
  }
  return 0;
}

char *CleanMessage(const char *content) {
  size_t len = strlen(content);
  char *cleaned = malloc(len + 1);
  if (!cleaned) return NULL;

  const char *src = content;
  char *dst = cleaned;

  while (*src) {
    if (*src == '<' && is_restricted_tag(src)) {
      while (*src != '\0' && *src++ != '>') {
      }
    } else {
      *dst++ = *src++;
    }
  }

  *dst = '\0';
  return cleaned;
}

void reverseMsgList(cJSON *msgArrItems) {
  cJSON *current = msgArrItems->child;
  cJSON *prev = NULL;
  cJSON *next = NULL;

  while (current != NULL) {
    next = current->next;
    current->next = prev;
    current->prev = next;
    prev = current;
    current = next;
  }

  msgArrItems->child = prev;
}

void WriteСonversationHtml(const cJSON *conv) {
  cJSON *convNameItem = cJSON_GetObjectItemCaseSensitive(conv, "displayName");

  const char *convName = "UnnamedChat";
  if (cJSON_IsString(convNameItem) && convNameItem->valuestring)
    convName = convNameItem->valuestring;

  cJSON *msgArrItems = cJSON_GetObjectItemCaseSensitive(conv, "MessageList");
  if (!cJSON_IsArray(msgArrItems)) return;

  char filename[256];
  snprintf(filename, sizeof(filename), "%s.html", convName);

  FILE *resultFile = fopen(filename, "w");
  if (!resultFile) {
    perror("File open error");
    return;
  }

  // There is not actually a array in cJSON framework, it's a linked list
  // Reverse list, because it was saved from last to first in origin
  reverseMsgList(msgArrItems);

  Msg currMsg = {0};
  cJSON *msgItem = NULL;

  cJSON_ArrayForEach(msgItem, msgArrItems) {
    cJSON *authorItem =
        cJSON_GetObjectItemCaseSensitive(msgItem, "displayName");
    /* Use 'from' if 'displayName' is invalid */
    if (!cJSON_IsString(authorItem)) {
      authorItem = cJSON_GetObjectItemCaseSensitive(msgItem, "from");
    }

    if (cJSON_IsString(authorItem) && authorItem->valuestring) {
      currMsg.author = authorItem->valuestring;
    } else {
      currMsg.author = "Unknown";
    }

    cJSON *timeItem =
        cJSON_GetObjectItemCaseSensitive(msgItem, "originalarrivaltime");
    if (cJSON_IsString(timeItem) && timeItem->valuestring) {
      currMsg.time = timeItem->valuestring;
    } else {
      currMsg.time = "UnknownTime";
    }

    cJSON *contentItem = cJSON_GetObjectItemCaseSensitive(msgItem, "content");
    if (cJSON_IsString(contentItem) && contentItem->valuestring) {
      currMsg.content = contentItem->valuestring;
    } else {
      continue;
    }

    char formatted_time[17] = {0};
    if (currMsg.time && strlen(currMsg.time) >= 16) {
      strncpy(formatted_time, currMsg.time, 16);
      formatted_time[10] = ' ';
    } else {
      strcpy(formatted_time, "UnknownTime");
    }

    fprintf(resultFile, "<p><strong>[%s] %s:</strong><br>\n", formatted_time,
            currMsg.author);

    char *cleaned_content = CleanMessage(currMsg.content);
    if (cleaned_content) {
      char *quoteSeparator = strstr(cleaned_content, QUOTE_MARK);
      if (quoteSeparator) {
        // First show answer, without QUOTE MARKS and first space ' '
        // fprintf(resultFile, "%s<br>\n", quoteSeparator + strlen(QUOTE_MARK));

        const char *slider = quoteSeparator + strlen(QUOTE_MARK);
        int numEmptyLines = 0;

        // TODO 1: make separate function for this with flag
        // 1 for showing which part to write (quote or plain)
        // second flag for showing for html or txt formatting
        while (*slider) {
          if (*slider == '\n') {
            if (++numEmptyLines <= MAX_EMPTY_LINES) {
              fputs("<br>\n", resultFile);
            }
          } else {
            fputc(*slider, resultFile);
          }
          ++slider;
        }

        fputs(
            "<blockquote style=\"margin-left:20px; color:gray; border-left: "
            "2px solid #ccc; padding-left: 10px;\">\n",
            resultFile);

        // Show by lines
        const char *lineStart = cleaned_content + strlen(TIMESTAMP);
        const char *lineEnd = NULL;
        numEmptyLines = 0;

        // TODO 1: make separate function for this with flag
        // 1 for showing which part to write (quote or plain)
        // second flag for showing for html or txt formatting
        while (lineStart < quoteSeparator) {
          numEmptyLines = (*lineStart == '\n') ? numEmptyLines + 1 : 0;
          lineEnd = memchr(lineStart, '\n', quoteSeparator - lineStart);

          if (!lineEnd) {
            lineEnd = quoteSeparator;
          }

          // No more than MAX_EMPTY_LINES empty lines in a row
          if (numEmptyLines <= MAX_EMPTY_LINES) {
            fwrite(lineStart, 1, lineEnd - lineStart, resultFile);
            fputs("<br>\n", resultFile);
          }

          lineStart = (lineEnd < quoteSeparator) ? lineEnd + 1 : quoteSeparator;
          // Skip second last empty line
          if (lineStart + 1 == quoteSeparator) {
            break;
          }
        }

        fputs("</blockquote><br>\n", resultFile);
      } else {
        // Show whole message
        fprintf(resultFile, "%s<br>\n<br>\n", cleaned_content);
      }
    } else {
      // If empty or can't clean message, show raw
      fprintf(resultFile, "%s<br>\n<br>\n", currMsg.content);
    }

    free(cleaned_content);
  }

  fclose(resultFile);
}

void WriteСonversationTxt(const cJSON *conv) {
  cJSON *convNameItem = cJSON_GetObjectItemCaseSensitive(conv, "displayName");

  const char *convName = "UnnamedChat";
  if (cJSON_IsString(convNameItem) && convNameItem->valuestring)
    convName = convNameItem->valuestring;

  cJSON *msgArrItems = cJSON_GetObjectItemCaseSensitive(conv, "MessageList");
  if (!cJSON_IsArray(msgArrItems)) return;

  char filename[256];
  snprintf(filename, sizeof(filename), "%s.txt", convName);

  FILE *resultFile = fopen(filename, "w");
  if (!resultFile) {
    perror("File open error");
    return;
  }

  // There is not actually a array in cJSON framework, it's a linked list
  // Reverse list, because it was saved from last to first in origin
  reverseMsgList(msgArrItems);

  Msg currMsg = {0};
  cJSON *msgItem = NULL;

  cJSON_ArrayForEach(msgItem, msgArrItems) {
    cJSON *authorItem =
        cJSON_GetObjectItemCaseSensitive(msgItem, "displayName");
    /* Use 'from' if 'displayName' is invalid */
    if (!cJSON_IsString(authorItem)) {
      authorItem = cJSON_GetObjectItemCaseSensitive(msgItem, "from");
    }

    if (cJSON_IsString(authorItem) && authorItem->valuestring) {
      currMsg.author = authorItem->valuestring;
    } else {
      currMsg.author = "Unknown";
    }

    cJSON *timeItem =
        cJSON_GetObjectItemCaseSensitive(msgItem, "originalarrivaltime");
    if (cJSON_IsString(timeItem) && timeItem->valuestring) {
      currMsg.time = timeItem->valuestring;
    } else {
      currMsg.time = "UnknownTime";
    }

    cJSON *contentItem = cJSON_GetObjectItemCaseSensitive(msgItem, "content");
    if (cJSON_IsString(contentItem) && contentItem->valuestring) {
      currMsg.content = contentItem->valuestring;
    } else {
      continue;
    }

    char formatted_time[17] = {0};
    if (currMsg.time && strlen(currMsg.time) >= 16) {
      strncpy(formatted_time, currMsg.time, 16);
      formatted_time[10] = ' ';
    } else {
      strcpy(formatted_time, "UnknownTime");
    }

    fprintf(resultFile, "[%s] %s:\n", formatted_time, currMsg.author);

    char *cleaned_content = CleanMessage(currMsg.content);
    if (cleaned_content) {
      char *quoteSeparator = strstr(cleaned_content, QUOTE_MARK);
      if (quoteSeparator) {
        // First show answer, without QUOTE MARKS and first space ' '
        fprintf(resultFile, "%s\n", quoteSeparator + strlen(QUOTE_MARK));

        fputs(INDENT_QUOTE "--------------\n", resultFile);
        // Show by lines with indent
        const char *lineStart = cleaned_content + strlen(TIMESTAMP);
        int numEmptyLines = 0;

        while (lineStart < quoteSeparator) {
          numEmptyLines = (*lineStart == '\n') ? numEmptyLines + 1 : 0;

          const char *lineEnd =
              memchr(lineStart, '\n', quoteSeparator - lineStart);

          if (!lineEnd) {
            lineEnd = quoteSeparator;
          }

          // No more than MAX_EMPTY_LINES empty lines in a row
          if (numEmptyLines <= MAX_EMPTY_LINES) {
            fprintf(resultFile, INDENT_QUOTE);
            fwrite(lineStart, 1, lineEnd - lineStart + 1, resultFile);
          }

          lineStart = (lineEnd < quoteSeparator) ? lineEnd + 1 : quoteSeparator;
          // Skip second last empty line
          if (lineStart + 1 == quoteSeparator) {
            break;
          }
        }

        fputs(INDENT_QUOTE "--------------\n\n", resultFile);
      } else {
        // Show whole message
        fprintf(resultFile, "%s\n\n", cleaned_content);
      }
    } else {
      // If empty or can't clean message, show raw
      fprintf(resultFile, "%s\n\n", currMsg.content);
    }

    free(cleaned_content);
  }

  fclose(resultFile);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [-h] <path_to_json_file>\n", argv[0]);
    return EXIT_FAILURE;
  }

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

  cJSON *conv = NULL;

  if (is_html) {
    cJSON_ArrayForEach(conv, convArr) { WriteСonversationHtml(conv); }
  } else {
    cJSON_ArrayForEach(conv, convArr) { WriteСonversationTxt(conv); }
  }

  cJSON_Delete(root);
  free(data);
  return EXIT_SUCCESS;
}
