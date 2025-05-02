#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat_exporter.h"
#include "utils.h"  // CleanMessage, QUOTE_MARK, TIMESTAMP, INDENT_QUOTE

#define MAX_EMPTY_LINES 3

void Txt_Start(FILE *f) {}

void Txt_PrintMessage(FILE *f, const Msg *msg) {
  char formatted_time[17] = "UnknownTime";
  if (strlen(msg->time) >= 16) {
    strncpy(formatted_time, msg->time, 16);
    formatted_time[10] = ' ';
  }

  fprintf(f, "[%s] %s:\n", formatted_time, msg->author);

  char *clean = CleanMessage(msg->content);
  if (!clean) {
    fprintf(f, "%s\n\n", msg->content);
    return;
  }

  char *quoteSeparator = strstr(clean, QUOTE_MARK);
  if (quoteSeparator) {
    fprintf(f, "%s\n", quoteSeparator + strlen(QUOTE_MARK));
    fputs(INDENT_QUOTE "--------------\n", f);

    const char *lineStart = clean + strlen(TIMESTAMP);
    const char *end = quoteSeparator;
    int emptyCount = 0;

    while (lineStart < end) {
      const char *lineEnd = memchr(lineStart, '\n', end - lineStart);
      if (!lineEnd) lineEnd = end;

      size_t len = lineEnd - lineStart;
      if (len == 0) {
        if (++emptyCount > MAX_EMPTY_LINES) {
          lineStart = lineEnd + 1;
          continue;
        }
      } else {
        emptyCount = 0;
      }

      fprintf(f, INDENT_QUOTE);
      fwrite(lineStart, 1, len, f);
      fputc('\n', f);

      lineStart = (lineEnd < end) ? lineEnd + 1 : end;
    }

    fputs(INDENT_QUOTE "--------------\n\n", f);
  } else {
    fprintf(f, "%s\n\n", clean);
  }

  free(clean);
}

void Txt_End(FILE *f) {}

Formatter TxtFormatter = {
    .Start = Txt_Start, .PrintMessage = Txt_PrintMessage, .End = Txt_End};
