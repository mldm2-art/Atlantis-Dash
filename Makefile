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

# Bibliotecas a serem linkadas
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Regra principal: compilar tudo
all: $(EXEC_NAME)

# Como gerar o executável
$(EXEC_NAME): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LIBS) -o $(EXEC_NAME)

# Comando para rodar o jogo diretamente
run: all
	./$(EXEC_NAME)

# Limpeza do projeto
clean:
	rm -f $(EXEC_NAME)
