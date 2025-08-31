-![Tuiuti Image](https://raw.githubusercontent.com/zLuuuck/ed1_fundcomp/main/imgs/tuiti.png)

# 🎮 Jogo da Forca em C

![C](https://img.shields.io/badge/Linguagem-C-00599C?style=for-the-badge&logo=c)
![Terminal](https://img.shields.io/badge/Interface-Terminal-4D4D4D?style=for-the-badge)
![License](https://img.shields.io/badge/Licença-EDUCACIONAL-blue?style=for-the-badge)

> Um jogo da forca completo desenvolvido em C com sistema de ranking e interface visual

## ✨ Funcionalidades

### 🎯 **Características Principais**
- ✅ **Jogo da Forca tradicional** com mecânica completa
- ✅ **Sistema de dicas** contextual para cada palavra
- ✅ **Cronômetro integrado** com cálculo de tempo real
- ✅ **Interface ASCII** com representação visual da forca
- ✅ **Entrada não-bloqueante** para melhor experiência

### ⚡ **Solução Técnica Avançada**
- 🔄 **Terminal não-canônico** implementado para entrada simultânea
- ⏰ **Sistema de tempo real** usando `select()` e `time.h`
- 🎮 **Input não-bloqueante** para atualização contínua do cronômetro
- 📊 **Atualização em tempo real** da interface durante o jogo

### 🏆 **Sistema de Pontuação Avançado**
| Fator de Pontuação | Valor | Descrição |
|-------------------|-------|-----------|
| **Tamanho da palavra** | 100-180 pts | Palavras maiores valem mais |
| **Eficiência** | até 80 pts | Acertos vs tentativas |
| **Jogo Perfeito** | 100 pts | Zero erros cometidos |
| **Tempo** | até 150 pts | Bônus por rapidez |
| **Penalidades** | -15 pts/erro | Redução por cada erro |

### 📊 **Sistema de Ranking**
- 🥇 **Top 10 melhores pontuações**
- 💾 **Persistência em arquivo CSV**
- 📅 **Registro de data e hora**
- 🔄 **Ordenação automática** por pontuação

## 🛠️ Tecnologias Utilizadas

```c
#include <stdio.h>      // Entrada/saída
#include <stdlib.h>     // Funções padrão
#include <unistd.h>     // Chamadas de sistema UNIX
#include <string.h>     // Manipulação de strings
#include <ctype.h>      // Manipulação de caracteres
#include <time.h>       // Funções de tempo
#include <sys/select.h> // Entrada não-bloqueante
#include <termios.h>    // Configuração do terminal
```

## 🔧 **Desafios Técnicos Superados**

### ⏰ Problema do Relógio e Input
**Problema:** O cronômetro não atualizava durante a entrada do usuário

**Solução Implementada:**
```c
// Configuração do terminal em modo não-canônico
void configurarTerminal(int config) {
    static struct termios old, new;
    if (config) {
        tcgetattr(STDIN_FILENO, &old);
        new = old;
        new.c_lflag &= ~(ICANON | ECHO);  // Remove modo canônico e echo
        tcsetattr(STDIN_FILENO, TCSANOW, &new);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }
}

// Verificação de entrada disponível sem bloquear
int entradaDisponivel() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

// Leitura de caractere sem bloquear a execução
char lerCharNaoBloqueante() {
    if (entradaDisponivel()) {
        char c;
        read(STDIN_FILENO, &c, 1);
        return c;
    }
    return '\0';
}
```

### 🎯 Sistema de Tempo Real
**Problema:** Atualizar cronômetro enquanto aguarda input

**Solução:**
```c
while (erros < 7 && letras_encontradas < tamanho_palavra) {
    // Atualiza o tempo continuamente (300ms)
    time_t agora = time(NULL);
    tempo_decorrido = difftime(agora, inicio_jogo);
    
    if (precisa_atualizar || difftime(agora, ultima_update) >= 0.3) {
        // Atualiza interface a cada 300ms
        limparTela();
        forca(erros);
        mostrarTempo(tempo_decorrido); // ← Tempo atualizado
        // ... resto da interface
        ultima_update = agora;
        precisa_atualizar = 0;
    }
    
    // Verifica input sem bloquear
    char input = lerCharNaoBloqueante();
    if (input != '\0') {
        // Processa input...
    }
    
    usleep(50000); // Pequena pausa de 50ms
}
```

## 🚀 Como Executar

### 📦 Compilação

**Linux/macOS:**
```bash
gcc -o forca forca.c && ./forca
```

**Windows:** (Requer compatibilidade POSIX)
```bash
gcc -o forca.exe forca.c && forca.exe
```

### 🎮 Controles
- **Letras A-Z**: Tentar adivinhar letras
- **Enter**: Confirmar entrada
- **Menu**: Navegação por números

## 🎯 Palavras do Jogo

| Palavra | Dica |
|---------|------|
| `COMPUTADOR` | "Tu usa todo dia" |
| `MEMORIA` | "Se cair a luz, esquece kkk" |
| `PROCESSADOR` | "Esquenta que é uma beleza" |
| `SOFTWARE` | "Esse você xinga" |
| `HARDWARE` | "Esse você chuta" |
| `PROGRAMADOR` | "Coisa que Bill Gates não era kk" |
| `ALGORITMO` | "Receita de bolo" |
| `DIOGENES` | "Professor top esse kkkkk" |
| `VARIAVEL` | "É.. o negócio varia" |
| `ESTRUTURADA` | "Se não for assim, é orientada a objetos" |

## 📊 Estrutura de Arquivos

```
forca/
├── 📄 forca.c                 # Código fonte principal
├── 📊 ranking.csv            # Arquivo de ranking (auto-gerado)
├── 📁 saves/                 # Pasta de saves (futuro)
└── 📖 README.md             # Documentação
```

## 🎨 Preview do Jogo

```
  _______ 
 |/      |
 |     ('-')
 |      /|\ 
 |       | 
 |      / \ 
_|___

Palavra: C _ M P _ T _ D _ R
Letras chutadas: A B C D E M P R T 
Erros: 3/7
Dica: Tu usa todo dia
Tempo: 02:45  ← Atualizando em tempo real!
```

## ⚙️ Personalização

### 🔧 Adicionar Novas Palavras

Edite a função `palavra_forca()`:

```c
static const Palavra palavras[num_palavras] = {
    {"NOVA_PALAVRA", "Sua dica aqui"},
    // ... outras palavras
};
```

### ⚡ Ajustar Dificuldade

Modifique as constantes:

```c
#define num_palavras 10    // Total de palavras
#define tamanho 30         // Tamanho máximo por palavra
```

## 🐛 Solução de Problemas

### 🔄 Palavras Repetidas
**Problema:** Palavras se repetem entre partidas
**Solução:** `srand(time(NULL))` é chamado a cada jogo novo

### 📁 Arquivo de Ranking
**Problema:** `ranking.csv` não existe
**Solução:** O sistema cria automaticamente na primeira pontuação

### ⌨️ Entrada de Teclado
**Problema:** Terminal travado
**Solução:** Sistema de entrada não-bloqueante implementado

## 📈 Estruturas de Dados

### 🏗️ Estrutura Palavra
```c
typedef struct {
    char palavra[30];     // Palavra a ser adivinhada
    char dica[100];       // Dica correspondente
} Palavra;
```

### 🏆 Estrutura Player
```c
typedef struct {
    char nome[50];        // Nome do jogador
    int pontos;           // Pontuação final
    char palavra[30];     // Palavra jogada
    int erros;            // Número de erros
    double tempo;         // Tempo levado
    char data_str[20];    // Data e hora
} Player;
```

## 🎯 Sistema de Arquivos

### 📊 Formato do CSV
```csv
Nome,Pontos,Palavra,Erros,Tempo,Data
Joao,450,COMPUTADOR,2,45.5,2024-03-15 14:30:25
Maria,320,MEMORIA,4,60.2,2024-03-15 15:45:12
```

## 🔮 Melhorias Futuras

- [ ] **Sistema de salvamento** de progresso
- [ ] **Multiplos níveis** de dificuldade
- [ ] **Modo multiplayer** por rede
- [ ] **Sons e efeitos** sonoros
- [ ] **Interface gráfica** alternativa

## 👨‍💻 Autor

**Lucas Toterol Rodrigues** - 2025  
Desenvolvido como parte do Estudo Dirigido de Programação Estruturada

## 📄 Licença

Este projeto é para **fins educacionais**.  
Sinta-se livre para usar e modificar para aprendizado!

---
