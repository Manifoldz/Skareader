#ifndef UTILS_H
#define UTILS_H

#define INDENT_QUOTE "    "
#define QUOTE_MARK "&lt;&lt;&lt; "
#define TIMESTAMP "[1234567890] "
#define MAX_EMPTY_LINES 2

char *CleanMessage(const char *content);
void reverseMsgList(cJSON *msgArrItems);

#endif
