#include "chat_exporter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

/**
 * @brief Parses message item cJSON and save info in message struct
 * @param msg ptr at Msg struct, where to put parsed data
 * @param msgItem ptr at cJSON, which need to parse from
 * @note Can put NULL into msg->content in case not found or invalid data
 */
void ParseMsgFromJson(Msg *msg, const cJSON *msgItem) {
  if (!msgItem || !msg) return;

  cJSON *author = cJSON_GetObjectItemCaseSensitive(msgItem, "displayName");
  if (!cJSON_IsString(author))
    author = cJSON_GetObjectItemCaseSensitive(msgItem, "from");
  msg->author = cJSON_IsString(author) ? author->valuestring : "Unknown";

  cJSON *time =
      cJSON_GetObjectItemCaseSensitive(msgItem, "originalarrivaltime");
  msg->time = cJSON_IsString(time) ? time->valuestring : "UnknownTime";

  cJSON *content = cJSON_GetObjectItemCaseSensitive(msgItem, "content");
  msg->content = (cJSON_IsString(content) && content->valuestring)
                     ? content->valuestring
                     : NULL;
}

/**
 * @brief Parses JSON file with chats data and writes each of them in file
 * @param conv ptr at cJSON, which contains messages (by fact that is dialog)
 * @param formatter ptr at Formatter struct, which can format to create output
 */
void WriteConversation(const cJSON *conv, Formatter *formatter) {
  const char *convName = "UnnamedChat";
  cJSON *convNameItem = cJSON_GetObjectItemCaseSensitive(conv, "displayName");
  if (cJSON_IsString(convNameItem) && convNameItem->valuestring)
    convName = convNameItem->valuestring;

  cJSON *msgArrItems = cJSON_GetObjectItemCaseSensitive(conv, "MessageList");
  if (!cJSON_IsArray(msgArrItems)) return;

  char filename[256];
  // TODO: make here enum for output type (txt, html, ...)
  // for unrecognized types just return
  if (formatter->is_html) {
    snprintf(filename, sizeof(filename), "%s.html", convName);
  } else {
    snprintf(filename, sizeof(filename), "%s.txt", convName);
  }

  FILE *file = fopen(filename, "w");
  if (!file) {
    perror("File open error");
    return;
  }

  // There is not actually a array in cJSON framework, it's a linked list
  // Reverse list, because it was saved from last to first in origin skype file
  reverseMsgList(msgArrItems);

  formatter->Start(file);

  cJSON *msgItem = NULL;
  cJSON_ArrayForEach(msgItem, msgArrItems) {
    Msg msg = {0};
    ParseMsgFromJson(&msg, msgItem);
    if (msg.content) formatter->PrintMessage(file, &msg);
  }

  formatter->End(file);
  fclose(file);
}
