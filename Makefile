# コンパイラとオプションの設定
CC = gcc-14
CFLAGS = -fanalyzer -Wall -Wextra -g -Og

# 対象ファイル
TARGET = main
SRCS = main.c chunk.c common.c memory.c debug.c value.c
OBJS = $(SRCS:.c=.o)

# 依存関係
DEPS = chunk.h common.h memory.h debug.h value.h

# 実行ファイルの生成ルール
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# オブジェクトファイルの生成ルール
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# クリーンアップ
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# デフォルトのターゲット
all: $(TARGET)

