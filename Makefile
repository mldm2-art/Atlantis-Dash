# Nome do executável final
EXEC_NAME = atlantis_dash

# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include

# Pega todos os arquivos .c da pasta src
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Flags de compilação
CFLAGS = -Wall -I$(INCLUDE_DIR)

# Detecta o sistema operacional
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	# Linux (Ubuntu)
	LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S), Darwin)
	# macOS
	LIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit
endif

# Regra principal
all: $(EXEC_NAME)

$(EXEC_NAME): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(EXEC_NAME)

run: all
	./$(EXEC_NAME)

clean:
	rm -f $(EXEC_NAME)