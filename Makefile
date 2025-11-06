EXEC_NAME = atlantis_dash #nome do nosso executável

CC = gcc #definindo nosso compilador (o gcc é padrão da linguagem C)

SRC_DIR = src #diretório dos arquivos .c
INCLUDE_DIR = include #diretórios dos arquivos .h

SRCS = $(wildcard $(SRC_DIR)/*.c) #pega todos os arquivos .c da pasta src

CFLAGS = -Wall -I$(INCLUDE_DIR) #-Wall (ativa o compilador) -I(indica onde estão os arquivos .h)

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 #biblioteca que está sendo utilizada

all: $(EXEC_NAME) #comando para compilar todo o nosso executável

$(EXEC_NAME): $(SRCS) #faz a compilação de todos os arquivos .c e gera o executável
	# O comando para construir (TEM DE TER UM 'TAB' NO INÍCIO)
	$(CC) $(SRCS) -o $(EXEC_NAME) $(CFLAGS) $(LIBS)

run: all #comando 'make run' para já compilar e executar o jogo
	./$(EXEC_NAME)

clean: #comando 'make clean' que apaga o executável para recompilar do zero
	# (TEM DE TER UM 'TAB' NO INÍCIO)
	rm -f $(EXEC_NAME)