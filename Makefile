# 编译选项
CC = gcc
CFLAGS = -std=c11 -Wall -I$(INC_DIR)

# 源文件和头文件路径
SRC_DIR = source
INC_DIR = include
# 构建目录定义
BUILD_DIR   = build
OBJ_DIR     = $(BUILD_DIR)/obj
BIN_DIR     = $(BUILD_DIR)/bin

# 创建目录命令
MKDIR_P     = mkdir -p

# 1. 单独处理根目录的 main.c
ROOT_SOURCES = main.c
ROOT_OBJECTS = $(addprefix $(OBJ_DIR)/, $(ROOT_SOURCES:.c=.o))
# 2. 处理 source/ 子目录的源文件
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
SUB_OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# 3. 合并所有对象文件
ALL_OBJECTS = $(ROOT_OBJECTS) $(SUB_OBJECTS)

# 最终目标重定向
TARGET := $(BIN_DIR)/ccom

# 主构建目标
all: | $(OBJ_DIR) $(BIN_DIR) $(TARGET)

# 目录创建规则
$(OBJ_DIR) $(BIN_DIR):
	@echo "创建目录: $@"
	$(MKDIR_P) $@

# 4. 根目录源文件编译规则（特殊处理）
$(OBJ_DIR)/%.o: %.c
	@echo "编译根目录文件: $<"
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 5. 子目录源文件编译规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "编译子目录文件: $<"
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# 最终可执行文件链接
$(TARGET): $(ALL_OBJECTS)
	@echo "链接可执行文件: $@"
	$(CC) $^ -o $@

.PHONY:all clean

# 清理
clean:
	@echo "清理构建产物"
	rm -rf $(BUILD_DIR)