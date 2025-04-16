# 컴파일러 및 플래그 설정
CC = gcc
CFLAGS = -Wall -g -std=c99

# 타겟 이름
TARGET = BitCalc

# 소스 파일 목록
SRCS = BitCalculator.c

# 헤더 파일 의존성
HEADERS = BigInt.h Adder.h func.h

# 오브젝트 파일 생성
OBJS = $(SRCS:.c=.o)

# 운영체제 감지
ifeq ($(OS),Windows_NT)
    # Windows 설정
    RM = del /Q
    TARGET_EXT = .exe
    CFLAGS += -D_CRT_SECURE_NO_WARNINGS
else
    # Linux/Unix 설정
    RM = rm -f
    TARGET_EXT = 
endif

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성 규칙
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@$(TARGET_EXT) $(OBJS)

# 오브젝트 파일 생성 규칙
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# clean 규칙
clean:
	$(RM) $(OBJS) $(TARGET)$(TARGET_EXT)

# run 규칙 (실행)
run: $(TARGET)
	./$(TARGET)$(TARGET_EXT)

# PHONY 타겟 선언
.PHONY: all clean run