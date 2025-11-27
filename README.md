
# 🌊 **Atlantis Dash**

**Atlantis Dash** é um jogo de ação estilo arcade onde cada movimento importa.  
Guie seu peixe através de rotas oceânicas cheias de perigos, evitando predadores e obstáculos.  
O objetivo é **sobreviver e completar os quatro níveis**.

---

## 🔧 **Instalação de dependências e RUN**

<details>
<summary><strong>Instalando Dependências (Raylib)</strong></summary>

Para compilar o jogo, você precisa instalar a biblioteca **Raylib**.

### **macOS**
Instale via Homebrew:
```sh
brew install raylib
````

### **Ubuntu / Linux**

```sh
sudo apt update
sudo apt install libraylib-dev libraylib-bin
```

### **Windows (WSL recomendado)**

#### **WSL**

Instale Raylib como no Ubuntu:

```sh
sudo apt update
sudo apt install libraylib-dev libraylib-bin
```

#### **Windows Nativo**

Baixe a Raylib:
👉 [https://github.com/raysan5/raylib](https://github.com/raysan5/raylib)

</details>

---

<details>
<summary><strong>Build e Execução</strong></summary>

Para limpar, compilar e executar:

```sh
make clean
make run
```

</details>

---

## ⚒️ **Ferramentas Utilizadas**

<details>
<summary><strong>Ver ferramentas</strong></summary>

**Linguagem:**

* C (C99)

**Biblioteca Gráfica:**

* Raylib

**IDE:**

* VS Code

**Ambiente:**

* Linux (WSL)
* macOS

**Controle de Versão:**

* GitHub

</details>

---

## 👥 **Equipe de Desenvolvedores**

<details>
<summary><strong>Ver equipe</strong></summary>

Projeto desenvolvido por estudantes da **Cesar School** para a disciplina de
**Programação Imperativa Funcional**.

**Desenvolvedores:**

* Rodrigo Paiva — [https://github.com/rodrigopaiva06](https://github.com/rodrigopaiva06)
* Maria Luísa Muniz — [https://github.com/mldm2-art](https://github.com/mldm2-art)
* Hélio de Moraes — [https://github.com/helioblaster](https://github.com/helioblaster)
* Pedro Coutinho — [https://github.com/Pedro-Coutinho2612](https://github.com/Pedro-Coutinho2612)

</details>

---

## 🎮 **Game Design Document (GDD)**

<details>
<summary><strong>Ver GDD</strong></summary>

### **1. Título e Introdução**

* **Nome:** Atlantis Dash
* **Inspiração:** Crossy Road
* **Objetivo:** Controlar um peixe que deve atravessar obstáculos móveis e fixos para alcançar o final de cada nível.

---

### **2. Mecânicas de Jogo**


#### **Obstáculos Fixos**

* Pedras
* Corais
* Conchas
* Algas

#### **Obstáculos Móveis**

* Tubarões
* Caranguejos
* Águas-vivas
* Baleias

#### **Níveis de Dificuldade**

* **Nível 1:** 1 obstáculo móvel, pouca quantidade
* **Nível 2:** 2 obstáculos móveis, quantidade moderada
* **Nível 3:** 3 obstáculos móveis, muitos obstáculos
* **Nível 4:** 4 obstáculos móveis, quantidade máxima

#### **Colisões**

* Obstáculo fixo → personagem fica preso
* Obstáculo móvel → perde 1 vida

---

### **3. Controles**

#### **Menus**

* **W** → opção de cima
* **S** → opção de baixo
* **Enter** → confirmar
* **Esc** → voltar
* **Q** → anvançar e salvar moedas
* **R** → zerar moedas

#### **Jogo**

* **W** → cima
* **S** → baixo
* **D** → direita
* **Esc** → sair do nível ou reiniciar após morrer

---

### **4. Interface e Feedback**

#### **Elementos exibidos**

* Obstáculos
* Vidas
* Moedas
* Personagem

#### **Estilo visual**

* Estética oceânica
* Sprites coloridos
* Layout limpo

---

### **5. Áudio e Efeitos**

* Música ambiente

---

### **6. Estados do Jogo**

* Menu inicial
* Tabela de níveis
* Jogando
* Instruções
* Game Over

</details>
```

