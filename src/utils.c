
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