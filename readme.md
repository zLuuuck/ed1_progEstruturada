![Tuiuti Image](https://raw.githubusercontent.com/zLuuuck/ed1_fundcomp/main/imgs/tuiti.png)

# 📝 README - Jogo da Forca em C

## 🎮 Descrição do Projeto

Este é um **Jogo da Forca** desenvolvido em linguagem C como parte de um Estudo Dirigido de Programação Estruturada. O jogo apresenta interface em terminal com sistema completo de pontuação, ranking e recursos visuais.

## ✨ Funcionalidades

### 🎯 Principais
- **Jogo da Forca completo** com mecânica tradicional
- **Sistema de dicas** para cada palavra
- **Cronômetro** que calcula tempo de jogo
- **Interface visual** com representação ASCII da forca
- **Entrada não-bloqueante** para melhor experiência

### 📊 Sistema de Pontuação
- Pontuação baseada no **tamanho da palavra**
- Bônus por **eficiência** (acertos vs tentativas)
- Penalidades por **erros**
- Bônus por **tempo** de conclusão
- Bônus **perfeito** por zero erros

### 🏆 Sistema de Ranking
- **Top 10 pontuações** salvas em arquivo CSV
- **Persistência** de dados entre execuções
- Exibição organizada por ordem de pontuação

### 🎨 Interface
- **Menu interativo** com arte ASCII
- **Feedback visual** em tempo real
- **Display** de letras já tentadas
- **Barra de progresso** visual da forca

## 🛠️ Tecnologias Utilizadas

- **Linguagem**: C (ISO C99)
- **Bibliotecas**: 
  - stdio.h, stdlib.h, unistd.h
  - string.h, ctype.h, time.h
  - sys/select.h, termios.h

## 📦 Estrutura do Projeto

```
forca/
├── forca.c              # Código principal do jogo
├── ranking.csv         # Arquivo de ranking (gerado automaticamente)
└── README.md           # Este arquivo
```

## 🚀 Como Compilar e Executar

### Compilação (Linux/macOS)
```bash
gcc -o forca forca.c
```

### Compilação (Windows)
```bash
gcc -o forca.exe forca.c
```

### Execução
```bash
./forca
```

## 🎮 Como Jogar

1. **Inicie o jogo** selecionando a opção 1 no menu
2. **Digite seu nome** para registro no ranking
3. **Tente adivinhar** a palavra digitando letras
4. **Use a dica** para ajudar na descoberta
5. **Evite 7 erros** para não ser enforcado!
6. **Consulte o ranking** para ver suas pontuações

## 📋 Regras de Pontuação

- **Pontuação base**: 100-180 pontos (dependendo do tamanho da palavra)
- **Bônus de eficiência**: Até 80 pontos
- **Bônus perfeito**: 100 pontos por zero erros
- **Bônus de tempo**: Até 150 pontos por rapidez
- **Penalidade por erros**: -15 pontos por erro

## 🎯 Palavras do Jogo

O jogo contém 10 palavras relacionadas à computação:
- `COMPUTADOR`, `MEMORIA`, `PROCESSADOR`, `SOFTWARE`, `HARDWARE`
- `PROGRAMADOR`, `ALGORITMO`, `DIOGENES`, `VARIAVEL`, `ESTRUTURADA`

Cada palavra possui uma **dica humorística** para ajudar o jogador.

## 📊 Arquivo de Ranking

As pontuações são salvas em `ranking.csv` no formato:
```csv
Nome,Pontos,Palavra,Erros,Tempo,Data
João,450,COMPUTADOR,2,45.5,2024-03-15 14:30:25
```

## 🎨 Estruturas de Dados

### Palavra
```c
typedef struct {
    char palavra[tamanho];
    char dica[100];
} Palavra;
```

### Player (Ranking)
```c
typedef struct {
    char nome[50];
    int pontos;
    char palavra[30];
    int erros;
    double tempo;
    time_t data;
    char data_str[20];
} Player;
```

## ⚙️ Personalização

### Adicionar Novas Palavras
Modifique a função `palavra_forca()`:
```c
static const Palavra palavras[num_palavras] = {
    {"NOVA_PALAVRA", "Dica da nova palavra"},
    // ... outras palavras
};
```

### Ajustar Dificuldade
Modifique as constantes:
```c
#define num_palavras 10    // Número de palavras
#define tamanho 30         // Tamanho máximo das palavras
```

## 🐛 Solução de Problemas

### Problema: Palavras repetidas
**Solução**: Foi implementado `srand(time(NULL))` na função `jogo()` para garantir aleatoriedade entre partidas.

### Problema: Arquivo de ranking não existe
**Solução**: O sistema cria automaticamente o arquivo `ranking.csv` na primeira pontuação.

## 📝 Autor

**Lucas Toterol Rodrigues** - 2025  
Desenvolvido como parte do Estudo Dirigido de Programação Estruturada.

## 📄 Licença

Este projeto é para fins educacionais.
