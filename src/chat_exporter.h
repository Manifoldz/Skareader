#ifndef CHAT_EXPORTER_H
#define CHAT_EXPORTER_H

#include <stdio.h>

#include "cJSON.h"

typedef struct {
  const char *author;
  const char *time;
  char *content;  // будет аллоцировано
} Msg;

typedef struct {
  void (*Start)(FILE *f);
  void (*PrintMessage)(FILE *f, const Msg *msg);
  void (*End)(FILE *f);
  int is_html;
} Formatter;

void WriteConversation(const cJSON *conv, Formatter *formatter,
                       const char *ext);
void ParseMsgFromJson(Msg *msg, const cJSON *msgItem);

extern Formatter TxtFormatter;
extern Formatter HtmlFormatter;

#endif
