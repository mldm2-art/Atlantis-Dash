EXEC_NAME = atlantis_dash

CC = gcc

SRC_DIR = src
INCLUDE_DIR = include

SRCS = $(wildcard $(SRC_DIR)/*.c)

CFLAGS = -Wall -I$(INCLUDE_DIR)

LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: $(EXEC_NAME)

$(EXEC_NAME): $(SRCS)
	# O comando para construir (TEM DE TER UM 'TAB' NO INÍCIO)
	$(CC) $(SRCS) -o $(EXEC_NAME) $(CFLAGS) $(LIBS)

run: all
	./$(EXEC_NAME)

clean:
	# (TEM DE TER UM 'TAB' NO INÍCIO)
	rm -f $(EXEC_NAME)