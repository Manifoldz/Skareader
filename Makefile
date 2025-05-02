# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -MMD
INCLUDES = -Ilibs/cjson -Isrc

# Каталоги
SRC_DIR = src
BUILD_DIR = bin
OBJ_DIR = build
LIB_DIR = libs/cjson

# Основной таргет
TARGET = $(BUILD_DIR)/skareader

# Исходники
SRC_FILES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/chat_exporter.c \
	$(SRC_DIR)/formatter_txt.c \
	$(SRC_DIR)/formatter_html.c \
	$(SRC_DIR)/utils.c \
	$(LIB_DIR)/cJSON.c

# Объектные и зависимые файлы
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))
DEP_FILES = $(OBJ_FILES:.o=.d)

# Основная цель
all: $(TARGET)

# Создание бинарника из объектов
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

# Правила компиляции .c в .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Чистка
clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR) *.txt *.html

# Автоматические зависимости
-include $(DEP_FILES)
