#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>

// Configurações específicas para Windows
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define sleep(x) Sleep((x) * 1000)  // Converte segundos para milissegundos
#define usleep(x) Sleep((x) / 1000) // Converte microssegundos para milissegundos
#else
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#endif

// Variáveis globais para o cronômetro
double tempo_decorrido = 0.0; // Tempo total decorrido em segundos
time_t inicio_jogo;           // Marca de tempo do início do jogo

// Função para configurar o terminal em modo não-canônico
void configurarTerminal(int config)
{
#ifdef _WIN32
    // No Windows, usamos _kbhit() e _getch() que não precisam de configuração especial
#else
    static struct termios old, new; // Estruturas para configuração do terminal

    if (config)
    {
        tcgetattr(STDIN_FILENO, &old); // Salva configurações atuais
        new = old;
        new.c_lflag &= ~(ICANON | ECHO);        // Desativa modo canônico e eco
        tcsetattr(STDIN_FILENO, TCSANOW, &new); // Aplica novas configurações
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old); // Restaura configurações originais
    }
#endif
}

// Função para verificar se há entrada disponível no teclado
int entradaDisponivel()
{
#ifdef _WIN32
    return _kbhit() > 0; // Windows: verifica se há tecla pressionada
#else
    struct timeval tv = {0L, 0L}; // Tempo zero para não bloquear
    fd_set fds;
    FD_ZERO(&fds);                               // Limpa conjunto de descritores
    FD_SET(STDIN_FILENO, &fds);                  // Adiciona stdin ao conjunto
    return select(1, &fds, NULL, NULL, &tv) > 0; // Verifica se há dados disponíveis
#endif
}

// Função para ler caractere sem bloquear a execução
char lerCharNaoBloqueante()
{
    if (entradaDisponivel())
    {
#ifdef _WIN32
        return _getch(); // Windows: lê caractere sem eco
#else
        char c;
        read(STDIN_FILENO, &c, 1); // Linux: lê caractere do stdin
        return c;
#endif
    }
    return '\0'; // Retorna nulo se não há entrada
}

// Função para limpar a tela do terminal
void limparTela()
{
#ifdef _WIN32
    system("cls"); // Windows: comando clear screen
#else
    system("clear"); // Linux: comando clear terminal
#endif
}

// Definições de constantes
#define num_palavras 10 // Número total de palavras no jogo
#define tamanho 30      // Tamanho máximo das palavras

// Estrutura para armazenar palavra e sua dica
typedef struct
{
    char palavra[tamanho]; // Palavra para adivinhar
    char dica[100];        // Dica associada à palavra
} Palavra;

// Estrutura para armazenar dados do jogador
typedef struct
{
    char nome[50];     // Nome do jogador
    int pontos;        // Pontuação alcançada
    char palavra[30];  // Palavra jogada
    int erros;         // Número de erros
    double tempo;      // Tempo levado
    time_t data;       // Timestamp da partida
    char data_str[20]; // Data formatada como string
} Player;

// Declarações de funções
void menuPrincipal();                                                                              // Menu principal do jogo
int jogo();                                                                                        // Função principal do jogo
void forca(int erros);                                                                             // Desenha a forca baseada nos erros
Palavra palavra_forca();                                                                           // Seleciona palavra aleatória
int calcularPoints(const char *palavra, int erros, int tentativas_totais, double tempo_decorrido); // Calcula pontuação
void mostrarTempo(double tempo);                                                                   // Exibe tempo formatado
void salvar_ranking_csv(char nome[20], int pontos, const char *palavra, int erros, double tempo);  // Salva ranking
int compararPoints(const void *a, const void *b);                                                  // Compara pontuações para ordenação
void carregar_e_mostrar_ranking();                                                                 // Carrega e exibe ranking

// Função principal
int main()
{
    menuPrincipal(); // Inicia o menu principal
    return 0;
}

// Menu principal do jogo
void menuPrincipal()
{
    setlocale(LC_ALL, "Portuguese_Brazil"); // Configura locale para português
    limparTela();
    char opcao[10];
    int points = 0;

    do
    {
        // ASCII Art do título do jogo
        printf("\n            █████                                                 █████     ");
        printf("\n           ▒▒███                                                 ▒▒███      ");
        printf("\n  █████████ ▒███        █████ ████ █████ ████ █████ ████  ██████  ▒███ █████");
        printf("\n ▒█▒▒▒▒███  ▒███       ▒▒███ ▒███ ▒▒███ ▒███ ▒▒███ ▒███  ███▒▒███ ▒███▒▒███ ");
        printf("\n ▒   ███▒   ▒███        ▒███ ▒███  ▒███ ▒███  ▒███ ▒███ ▒███ ▒▒▒  ▒██████▒  ");
        printf("\n   ███▒   █ ▒███      █ ▒███ ▒███  ▒███ ▒███  ▒███ ▒███ ▒███  ███ ▒███▒▒███ ");
        printf("\n  █████████ ███████████ ▒▒████████ ▒▒████████ ▒▒████████▒▒██████  ████ █████");
        printf("\n ▒▒▒▒▒▒▒▒▒ ▒▒▒▒▒▒▒▒▒▒▒   ▒▒▒▒▒▒▒▒   ▒▒▒▒▒▒▒▒   ▒▒▒▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒ ▒▒▒▒▒ ");
        printf("\n >>>>>>>>>>>>>>>>>>>>>>>>> zLuuuck Programs - 2025 <<<<<<<<<<<<<<<<<<<<<<<<<\n");
        printf("\n -=-=-=-=-=-=-=-=-=-=-=-=-=-=- JOGO DA FORCA -=-=-=-=-=-=-=-=-=-=-=-=-=");
        printf("\n -=-=-=-=-=-=- Estudo Dirigido de Programação Estruturada -=-=-=-=-=-=-");
        printf("\n -=-=-=-=-=-=-=-=- por Lucas Toterol Rodrigues - 2025 -=-=-=-=-=-=-=-=-\n\n");

        printf("Sua última pontuação: %d\n", points);
        printf("[1] Jogar\n");
        printf("[2] Ver Ranking\n");
        printf("[3] Sair\n");
        printf("Escolha uma opção: ");

        fgets(opcao, sizeof(opcao), stdin); // Lê opção do usuário

        switch (opcao[0])
        {
        case '1':
            limparTela();
            printf("Você escolheu jogar!\n");
            sleep(1);
            points = jogo(); // Inicia o jogo
            printf("Você fez %d pontos!\n", points);
            sleep(2);
            break;
        case '2':
            limparTela();
            carregar_e_mostrar_ranking(); // Exibe ranking
            sleep(2);
            break;
        case '3':
            limparTela();
            printf("Obrigado por jogar!\n");
            sleep(3);
            exit(0); // Sai do programa
        default:
            limparTela();
            printf("Opção inválida!\n");
            sleep(1);
        }
    } while (opcao[0] != '3');
}

// Exibe tempo formatado em minutos e segundos
void mostrarTempo(double tempo)
{
    int minutos = (int)tempo / 60;
    int segundos = (int)tempo % 60;
    printf("Tempo: %02d:%02d", minutos, segundos);
}

// Função principal do jogo da forca
int jogo()
{
    srand(time(NULL)); // Inicializa gerador de números aleatórios
    int finalPoints = 0, erros = 0;

    printf("Iniciando o jogo da forca...\n\n");
    sleep(2);

    limparTela();
    char nome[20];
    printf("Digite seu nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0; // Remove newline do final
    sleep(2);
    limparTela();
    printf("Boa sorte, %s! O jogo vai começar em 3 segundos...\n", nome);
    sleep(3);

    configurarTerminal(1); // Configura terminal para modo não-canônico

    inicio_jogo = time(NULL); // Marca início do tempo
    tempo_decorrido = 0.0;

    Palavra palavraFunc = palavra_forca();
    const char *palavra = palavraFunc.palavra; // Palavra sorteada
    const char *dica = palavraFunc.dica;       // Dica da palavra

    int tamanho_palavra = strlen(palavra);

    char letras_descobertas[tamanho_palavra + 1]; // Array para letras descobertas
    char letras_chutadas[26] = {0};               // Array para letras já tentadas
    int tentativas_totais = 0;
    int letras_encontradas = 0;
    int i;

    // Inicializa array de letras descobertas com underscores
    for (i = 0; i < tamanho_palavra; i++)
        letras_descobertas[i] = '_';
    letras_descobertas[tamanho_palavra] = '\0';

    time_t ultima_update = time(NULL);
    int precisa_atualizar = 1;

    // Loop principal do jogo
    while (erros < 7 && letras_encontradas < tamanho_palavra)
    {
        time_t agora = time(NULL);
        tempo_decorrido = difftime(agora, inicio_jogo); // Atualiza tempo decorrido

        // Atualiza tela periodicamente
        if (precisa_atualizar || difftime(agora, ultima_update) >= 0.3)
        {
            limparTela();
            forca(erros); // Desenha estado atual da forca

            printf("\nPalavra: ");
            int i;
            for (i = 0; i < tamanho_palavra; i++)
                printf("%c ", letras_descobertas[i]); // Mostra letras descobertas

            printf("\nLetras chutadas: ");
            for (i = 0; i < 26; i++)
                if (letras_chutadas[i])
                    printf("%c ", 'A' + i); // Mostra letras já tentadas

            printf("\nErros: %d/7\n", erros);
            printf("Dica: %s\n", dica);
            printf("Tempo decorrido: ");
            mostrarTempo(tempo_decorrido);
            printf("\nDigite uma letra: ");
            fflush(stdout);

            ultima_update = agora;
            precisa_atualizar = 0;
        }

        char input = lerCharNaoBloqueante(); // Lê entrada do usuário

        if (input != '\0')
        {
            if (input == '\r' || input == '\n') // Ignora Enter
            {
                continue;
            }

            char guess = toupper(input); // Converte para maiúscula

            if (guess >= 'A' && guess <= 'Z') // Verifica se é letra válida
            {
                if (letras_chutadas[guess - 'A']) // Letra já tentada
                {
                    printf("\nVocê já tentou essa letra! Pressione qualquer tecla...");
                    fflush(stdout);
                    while (lerCharNaoBloqueante() == '\0')
                    {
                        usleep(10000); // Pequena pausa
                    }
                }
                else
                {
                    letras_chutadas[guess - 'A'] = 1; // Marca letra como tentada
                    tentativas_totais++;

                    int acertou = 0;
                    int i;
                    // Verifica se letra existe na palavra
                    for (i = 0; i < tamanho_palavra; i++)
                    {
                        if (palavra[i] == guess && letras_descobertas[i] == '_')
                        {
                            letras_descobertas[i] = guess; // Revela letra
                            letras_encontradas++;
                            acertou = 1;
                        }
                    }

                    if (!acertou)
                    {
                        erros++; // Incrementa erros se não acertou
                    }

                    precisa_atualizar = 1; // Força atualização da tela
                }
            }
            else
            {
                printf("\nLetra inválida! Pressione qualquer tecla...");
                fflush(stdout);
                while (lerCharNaoBloqueante() == '\0')
                {
                    usleep(10000);
                }
            }
        }

        usleep(50000); // Pausa de 50ms entre iterações
    }

    configurarTerminal(0); // Restaura configurações do terminal

    time_t fim = time(NULL);
    tempo_decorrido = difftime(fim, inicio_jogo); // Calcula tempo final

    limparTela();
    forca(erros); // Mostra estado final da forca

    // Verifica se jogador venceu ou perdeu
    if (letras_encontradas == tamanho_palavra)
    {
        printf("\nParabéns! Você acertou a palavra: %s\n", palavra);
        printf("Tempo total: ");
        mostrarTempo(tempo_decorrido);
        printf("\n");
        finalPoints = calcularPoints(palavra, erros, tentativas_totais, tempo_decorrido);
        salvar_ranking_csv(nome, finalPoints, palavra, erros, tempo_decorrido);
    }
    else
    {
        printf("\nVocê perdeu! A palavra era: %s\n", palavra);
        printf("Tempo total: ");
        mostrarTempo(tempo_decorrido);
        printf("\n");
        finalPoints = 0;
    }
    sleep(3);
    return finalPoints;
}

// Desenha a forca baseada no número de erros
void forca(int erros)
{
    switch (erros)
    {
    case 0: // Forca vazia
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 1: // Cabeça
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 2: // Corpo
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |       | \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 3: // Braço esquerdo
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /| \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 4: // Braço direito
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 5: // Perna esquerda
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      /  \n");
        printf("_|___\n");
        break;
    case 6: // Perna direita - Game Over
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      / \\ \n");
        printf("_|___\n");
        break;
    default: // Estado extra
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     (x-x) bleh.\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      / \\ \n");
        printf("_|___\n");
        break;
    }
}

// Retorna uma palavra aleatória com sua dica
Palavra palavra_forca()
{
    static const Palavra palavras[num_palavras] = {
        {"COMPUTADOR", "Tu usa todo dia"},
        {"MEMORIA", "Se cair a luz, esquece kkk"},
        {"PROCESSADOR", "Esquenta que é uma beleza"},
        {"SOFTWARE", "Esse você xinga"},
        {"HARDWARE", "Esse você chuta"},
        {"PROGRAMADOR", "Coisa que Bill Gates não era kk"},
        {"ALGORITMO", "Receita de bolo"},
        {"DIOGENES", "Professor top esse kkkkk"},
        {"VARIAVEL", "É.. o negócio varia "},
        {"ESTRUTURADA", "Se não for assim, é orientada a objetos"},
    };

    int i = rand() % num_palavras; // Seleciona índice aleatório
    return palavras[i];            // Retorna palavra e dica
}

// Calcula pontuação baseada em vários fatores
int calcularPoints(const char *palavra, int erros, int tentativas_totais, double tempo_decorrido)
{
    int tamanho_palavra = strlen(palavra);
    int points_base = 0;

    // Pontuação base baseada no tamanho da palavra
    switch (tamanho_palavra)
    {
    case 7:
        points_base = 100;
        break;
    case 8:
        points_base = 120;
        break;
    case 9:
        points_base = 140;
        break;
    case 10:
        points_base = 160;
        break;
    case 11:
        points_base = 180;
        break;
    default:
        points_base = 100;
    }

    // Calcula eficiência (acertos/tentativas)
    float efficiency;
    if (tentativas_totais > 0)
    {
        efficiency = (float)(tamanho_palavra - erros) / tentativas_totais;
    }
    else
    {
        efficiency = 1.0;
    }

    // Bônus por eficiência
    int bonus_efficiency = 0;
    if (efficiency >= 0.9)
        bonus_efficiency = 80;
    else if (efficiency >= 0.7)
        bonus_efficiency = 50;
    else if (efficiency >= 0.5)
        bonus_efficiency = 30;
    else if (efficiency >= 0.3)
        bonus_efficiency = 10;

    int penalidade_erros = erros * 15;              // Penalidade por erros
    int bonus_perfeito = (erros == 0) ? 100 : 0;    // Bônus por jogo perfeito
    int bonus_tamanho = (tamanho_palavra - 7) * 10; // Bônus por palavra longa

    // Bônus/penalidade por tempo
    int bonus_tempo = 0;
    if (tempo_decorrido <= 20.0)
        bonus_tempo = 150;
    else if (tempo_decorrido <= 40.0)
        bonus_tempo = 100;
    else if (tempo_decorrido <= 60.0)
        bonus_tempo = 50;
    else if (tempo_decorrido <= 90.0)
        bonus_tempo = 0;
    else if (tempo_decorrido <= 120.0)
        bonus_tempo = -30;
    else
        bonus_tempo = -70;

    // Calcula pontuação final
    int points_final = points_base + bonus_efficiency + bonus_perfeito + bonus_tamanho + bonus_tempo - penalidade_erros;

    if (points_final < 10)
        points_final = 10; // Pontuação mínima

    return points_final;
}

// Salva dados da partida em arquivo CSV
void salvar_ranking_csv(char nome[20], int pontos, const char *palavra, int erros, double tempo)
{
    FILE *arquivo = fopen("ranking.csv", "a"); // Abre para append

    if (arquivo == NULL)
    {
        // Se arquivo não existe, cria com cabeçalho
        arquivo = fopen("ranking.csv", "w");
        if (arquivo)
        {
            fprintf(arquivo, "Nome,Pontos,Palavra,Erros,Tempo,Data\n");
            fclose(arquivo);
            arquivo = fopen("ranking.csv", "a"); // Reabre para append
        }
    }

    if (arquivo)
    {
        time_t agora = time(NULL);
        struct tm *tm_info = localtime(&agora);
        char data_str[20];
        strftime(data_str, sizeof(data_str), "%Y-%m-%d %H:%M:%S", tm_info); // Formata data

        // Escreve dados no arquivo CSV
        fprintf(arquivo, "%s,%d,%s,%d,%.1f,%s\n",
                nome, pontos, palavra, erros, tempo, data_str);

        fclose(arquivo);
    }
}

#define MAX_RANKING 100 // Número máximo de entradas no ranking

// Função de comparação para qsort - ordena por pontuação decrescente
int compararPoints(const void *a, const void *b)
{
    Player *ra = (Player *)a;
    Player *rb = (Player *)b;
    return rb->pontos - ra->pontos; // Ordem decrescente
}

// Carrega e exibe ranking de pontuações
void carregar_e_mostrar_ranking()
{
    FILE *arquivo = fopen("ranking.csv", "r");
    Player rankings[MAX_RANKING];
    int count = 0;

    if (arquivo == NULL)
    {
        limparTela();
        printf("-=-=-=-=-=-=-=- RANKING -=-=-=-=-=-=-=-\n\n");
        printf("Nenhuma pontuação registrada ainda!\n");
        printf("Seja o primeiro a jogar e aparecer aqui!\n");
        printf("\nPressione Enter para continuar...");
        getchar();
        return;
    }

    // Lê todas as linhas do arquivo
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo) && count < MAX_RANKING)
    {
        linha[strcspn(linha, "\n")] = 0; // Remove newline

        if (strlen(linha) == 0) // Pula linhas vazias
            continue;

        // Parse dos dados CSV
        char *token = strtok(linha, ",");
        if (token)
            strcpy(rankings[count].nome, token);

        token = strtok(NULL, ",");
        if (token)
            rankings[count].pontos = atoi(token);

        token = strtok(NULL, ",");
        if (token)
            strcpy(rankings[count].palavra, token);

        token = strtok(NULL, ",");
        if (token)
            rankings[count].erros = atoi(token);

        token = strtok(NULL, ",");
        if (token)
            rankings[count].tempo = atof(token);

        token = strtok(NULL, ",");
        if (token)
            strcpy(rankings[count].data_str, token);

        count++;
    }
    fclose(arquivo);

    // Ordena ranking por pontuação
    if (count > 0)
    {
        qsort(rankings, count, sizeof(Player), compararPoints);
    }

    // Exibe top 10 pontuações
    limparTela();
    printf("-=-=-=-=-=-=-=- TOP 10 PONTUAÇÕES REGISTRADAS -=-=-=-=-=-=-=-\n\n");
    printf("Pos. Nome       Pontos  Palavra     Erros Tempo   Data\n");
    printf("----------------------------------------------------------------------\n");

    int limite = (count > 10) ? 10 : count;
    int i;
    for (i = 0; i < limite; i++)
    {
        printf("%2d.  %-10s %-7d %-12s %-2d    %-6.1f %s\n", i + 1, rankings[i].nome, rankings[i].pontos, rankings[i].palavra, rankings[i].erros, rankings[i].tempo, rankings[i].data_str);
    }

    if (count == 0)
    {
        printf("Nenhuma pontuação registrada ainda!\n");
    }

    printf("\nPressione Enter para continuar...");
    getchar();
}