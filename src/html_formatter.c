#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chat_exporter.h"
#include "utils.h"

void Html_Start(FILE *f) {
  fputs(
      "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n"
      "<meta charset=\"UTF-8\">\n"
      "<meta name=\"viewport\" content=\"width=device-width, "
      "initial-scale=1.0\">\n"
      "<style>\n"
      "body { font-family: Arial, sans-serif; background-color: #f5f5f5; "
      "color: #222; padding: 20px; }\n"
      ".msg { margin-bottom: 20px; padding: 10px; border-left: 4px solid #ccc; "
      "background-color: #fff; }\n"
      ".meta { font-size: 0.9em; color: #555; margin-bottom: 5px; }\n"
      ".quote { margin-top: 10px; padding-left: 10px; border-left: 3px solid "
      "#aaa; color: #666; font-style: italic; white-space: pre-wrap; }\n"
      ".text { white-space: pre-wrap; }\n"
      "</style>\n"
      "<title>Chat Export</title>\n"
      "</head>\n<body>\n",
      f);
}

void Html_PrintMessage(FILE *f, const Msg *msg) {
  char *clean = CleanMessage(msg->content);

  const char *author = msg->author ? msg->author : "Unknown";
  const char *time = msg->time ? msg->time : "UnknownTime";

  fprintf(f, "<div class=\"msg\">\n");
  fprintf(f, "<div class=\"meta\">[%.*s %.*s] <strong>%s</strong></div>\n", 10,
          time, 5, time + 11, author);

  if (!clean) {
    fprintf(f, "<div class=\"text\">%s</div>\n</div>\n", msg->content);
    return;
  }

  char *quoteSeparator = strstr(clean, QUOTE_MARK);
  if (quoteSeparator) {
    fprintf(f, "<div class=\"text\">%s</div>\n",
            quoteSeparator + strlen(QUOTE_MARK));

    fprintf(f, "<div class=\"quote\">");

    const char *start = clean + strlen(TIMESTAMP);
    const char *end = quoteSeparator;

    while (start < end) {
      const char *newline = memchr(start, '\n', end - start);
      if (!newline) newline = end;

      fwrite(start, 1, newline - start, f);
      fputs("<br>", f);

      start = (newline < end) ? newline + 1 : end;
    }

    fputs("</div>\n</div>\n");
  } else {
    fprintf(f, "<div class=\"text\">%s</div>\n</div>\n", clean);
  }

  free(clean);
}

void Html_End(FILE *f) { fputs("</body>\n</html>\n", f); }

Formatter HtmlFormatter = {
    .Start = Html_Start, .PrintMessage = Html_PrintMessage, .End = Html_End};
