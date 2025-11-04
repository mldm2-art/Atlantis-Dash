# --- Variáveis de Configuração ---
# (Apelidos para facilitar)

# 1. O nome do seu jogo final
EXEC_NAME = atlantis_dash

# 2. O compilador
CC = gcc

# 3. As pastas do projeto
SRC_DIR = src
INCLUDE_DIR = include

# 4. Encontra TODOS os ficheiros .c dentro da pasta 'src'
# (ex: src/main.c, src/player.c, etc.)
SRCS = $(wildcard $(SRC_DIR)/*.c)

# 5. Opções do compilador
# -Wall = Mostra todos os avisos
# -I$(INCLUDE_DIR) = Diz ao GCC para procurar os "Cardápios" (.h) na pasta 'include'
CFLAGS = -Wall -I$(INCLUDE_DIR)

# 6. As Bibliotecas que o jogo precisa (a lista mágica do nosso teste!)
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# --- Receitas (O que o 'make' pode fazer) ---

# A 'all' é a receita principal (o que 'make' faz por defeito)
# Ela "depende" do nosso jogo final
all: $(EXEC_NAME)

# Esta é a receita que constrói o jogo
# Ela "depende" de todos os ficheiros .c
$(EXEC_NAME): $(SRCS)
	# O comando para construir (TEM DE TER UM 'TAB' NO INÍCIO)
	$(CC) $(SRCS) -o $(EXEC_NAME) $(CFLAGS) $(LIBS)

# Receita 'run': Compila E executa o jogo
# (Depende de 'all', ou seja, compila primeiro)
run: all
	./$(EXEC_NAME)

# Receita 'clean': Apaga o jogo compilado para "limpar"
clean:
	# (TEM DE TER UM 'TAB' NO INÍCIO)
	rm -f $(EXEC_NAME)