#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Configurações específicas para Windows
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define sleep(x) Sleep((x) * 1000)
#define usleep(x) Sleep((x) / 1000)
#else
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#endif

// Variáveis globais para o cronômetro
double tempo_decorrido = 0.0;
time_t inicio_jogo;

// Função para configurar o terminal em modo não-canônico
void configurarTerminal(int config)
{
#ifdef _WIN32
    // No Windows, usamos _kbhit() e _getch() que não precisam de configuração especial
#else
    static struct termios old, new;

    if (config)
    {
        tcgetattr(STDIN_FILENO, &old);
        new = old;
        new.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }
#endif
}

// Função para verificar se há entrada disponível
int entradaDisponivel()
{
#ifdef _WIN32
    return _kbhit() > 0;
#else
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
#endif
}

// Função para ler caractere sem bloquear
char lerCharNaoBloqueante()
{
    if (entradaDisponivel())
    {
#ifdef _WIN32
        return _getch();
#else
        char c;
        read(STDIN_FILENO, &c, 1);
        return c;
#endif
    }
    return '\0';
}

// Função para limpar a tela do terminal
void limparTela()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Definições de constantes
#define num_palavras 10
#define tamanho 30

// Definindo estrutura para palavras e dicas
typedef struct
{
    char palavra[tamanho];
    char dica[100];
} Palavra;

typedef struct
{
    char nome[50];
    int pontos;
    char palavra[30];
    int erros;
    double tempo;
    time_t data;
    char data_str[20];
} Player;

// Declarações de funções
void menuPrincipal();
int jogo();
void forca(int erros);
Palavra palavra_forca();
int calcularPoints(const char *palavra, int erros, int tentativas_totais, double tempo_decorrido);
void mostrarTempo(double tempo);
void salvar_ranking_csv(char nome[20], int pontos, const char *palavra, int erros, double tempo);
int compararPoints(const void *a, const void *b);
void carregar_e_mostrar_ranking();

int main()
{
    menuPrincipal();
    return 0;
}

void menuPrincipal()
{
    limparTela();
    char opcao[10];
    int points = 0;

    do
    {
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

        fgets(opcao, sizeof(opcao), stdin);

        switch (opcao[0])
        {
        case '1':
            limparTela();
            printf("Você escolheu jogar!\n");
            sleep(1);
            points = jogo();
            printf("Você fez %d pontos!\n", points);
            sleep(2);
            break;
        case '2':
            limparTela();
            carregar_e_mostrar_ranking();
            sleep(2);
            break;
        case '3':
            limparTela();
            printf("Obrigado por jogar!\n");
            sleep(3);
            exit(0);
        default:
            limparTela();
            printf("Opção inválida!\n");
            sleep(1);
        }
    } while (opcao[0] != '3');
}

// Função para mostrar o tempo formatado
void mostrarTempo(double tempo)
{
    int minutos = (int)tempo / 60;
    int segundos = (int)tempo % 60;
    printf("Tempo: %02d:%02d", minutos, segundos);
}

int jogo()
{
    srand(time(NULL));
    int finalPoints = 0, erros = 0;

    printf("Iniciando o jogo da forca...\n\n");
    sleep(2);

    limparTela();
    char nome[20];
    printf("Digite seu nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = 0; // Remove newline
    sleep(2);
    limparTela();
    printf("Boa sorte, %s! O jogo vai começar em 3 segundos...\n", nome);
    sleep(3);

    // Configura terminal para modo não-canônico
    configurarTerminal(1);

    // Inicia o cronômetro
    inicio_jogo = time(NULL);
    tempo_decorrido = 0.0;

    Palavra palavraFunc = palavra_forca();
    const char *palavra = palavraFunc.palavra; // A palavra sorteada
    const char *dica = palavraFunc.dica;       // A dica da palavra

    int tamanho_palavra = strlen(palavra);

    char letras_descobertas[tamanho_palavra + 1];
    char letras_chutadas[26] = {0};
    int tentativas_totais = 0;
    int letras_encontradas = 0;

    for (int i = 0; i < tamanho_palavra; i++)
        letras_descobertas[i] = '_';
    letras_descobertas[tamanho_palavra] = '\0';

    time_t ultima_update = time(NULL);
    int precisa_atualizar = 1;

    while (erros < 7 && letras_encontradas < tamanho_palavra)
    {
        // Atualiza o tempo continuamente
        time_t agora = time(NULL);
        tempo_decorrido = difftime(agora, inicio_jogo);

        // Atualiza a tela periodicamente ou quando necessário
        if (precisa_atualizar || difftime(agora, ultima_update) >= 0.3)
        {
            limparTela();
            forca(erros);

            printf("\nPalavra: ");
            for (int i = 0; i < tamanho_palavra; i++)
                printf("%c ", letras_descobertas[i]);

            printf("\nLetras chutadas: ");
            for (int i = 0; i < 26; i++)
                if (letras_chutadas[i])
                    printf("%c ", 'A' + i);

            printf("\nErros: %d/7\n", erros);
            printf("Dica: %s\n", dica);
            printf("Tempo decorrido: ");
            mostrarTempo(tempo_decorrido);
            printf("\nDigite uma letra: ");
            fflush(stdout);

            ultima_update = agora;
            precisa_atualizar = 0;
        }

        // Verifica entrada do usuário
        char input = lerCharNaoBloqueante();

        if (input != '\0')
        {
            if (input == '\r' || input == '\n') // Windows usa \r\n
            {
                // Ignora enter
                continue;
            }

            char guess = toupper(input);

            if (guess >= 'A' && guess <= 'Z')
            {
                if (letras_chutadas[guess - 'A'])
                {
                    printf("\nVocê já tentou essa letra! Pressione qualquer tecla...");
                    fflush(stdout);
                    while (lerCharNaoBloqueante() == '\0')
                    {
                        usleep(10000);
                    }
                }
                else
                {
                    letras_chutadas[guess - 'A'] = 1;
                    tentativas_totais++;

                    int acertou = 0;
                    for (int i = 0; i < tamanho_palavra; i++)
                    {
                        if (palavra[i] == guess && letras_descobertas[i] == '_')
                        {
                            letras_descobertas[i] = guess;
                            letras_encontradas++;
                            acertou = 1;
                        }
                    }

                    if (!acertou)
                    {
                        erros++;
                    }

                    precisa_atualizar = 1; // Força atualização na próxima iteração
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

        // Pequena pausa
        usleep(50000); // 50ms
    }

    // Restaura configurações do terminal
    configurarTerminal(0);

    // Calcula tempo final
    time_t fim = time(NULL);
    tempo_decorrido = difftime(fim, inicio_jogo);

    limparTela();
    forca(erros);

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

// Função forca() (mantida igual)
void forca(int erros)
{
    switch (erros)
    {
    case 0:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 1:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      \n");
        printf(" |      \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 2:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |       | \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 3:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /| \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 4:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      \n");
        printf("_|___\n");
        break;
    case 5:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      /  \n");
        printf("_|___\n");
        break;
    case 6:
        printf("  _______\n");
        printf(" |/      |\n");
        printf(" |     ('-')\n");
        printf(" |      /|\\ \n");
        printf(" |       | \n");
        printf(" |      / \\ \n");
        printf("_|___\n");
        break;
    default:
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

// Função palavra_forca() (mantida igual)
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

    int i = rand() % num_palavras;
    return palavras[i];
}

// Função calcularPoints() (mantida igual)
int calcularPoints(const char *palavra, int erros, int tentativas_totais, double tempo_decorrido)
{
    int tamanho_palavra = strlen(palavra);
    int points_base = 0;

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

    float efficiency;
    if (tentativas_totais > 0)
    {
        efficiency = (float)(tamanho_palavra - erros) / tentativas_totais;
    }
    else
    {
        efficiency = 1.0;
    }

    int bonus_efficiency = 0;
    if (efficiency >= 0.9)
        bonus_efficiency = 80;
    else if (efficiency >= 0.7)
        bonus_efficiency = 50;
    else if (efficiency >= 0.5)
        bonus_efficiency = 30;
    else if (efficiency >= 0.3)
        bonus_efficiency = 10;

    int penalidade_erros = erros * 15;
    int bonus_perfeito = (erros == 0) ? 100 : 0;
    int bonus_tamanho = (tamanho_palavra - 7) * 10;

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

    int points_final = points_base + bonus_efficiency + bonus_perfeito + bonus_tamanho + bonus_tempo - penalidade_erros;

    if (points_final < 10)
        points_final = 10;

    return points_final;
}

// Função para salvar o ranking em arquivo CSV
void salvar_ranking_csv(char nome[20], int pontos, const char *palavra, int erros, double tempo)
{
    FILE *arquivo = fopen("ranking.csv", "a"); // 'a' para append (adicionar no final)

    if (arquivo == NULL)
    {
        // Se o arquivo não existe, cria com cabeçalho primeiro
        arquivo = fopen("ranking.csv", "w");
        if (arquivo)
        {
            fprintf(arquivo, "Nome,Pontos,Palavra,Erros,Tempo,Data\n");
            fclose(arquivo);

            // Agora reabre para append
            arquivo = fopen("ranking.csv", "a");
        }
    }

    if (arquivo)
    {
        time_t agora = time(NULL);
        struct tm *tm_info = localtime(&agora);
        char data_str[20];
        strftime(data_str, sizeof(data_str), "%Y-%m-%d %H:%M:%S", tm_info);

        // Formato CSV: Nome,Pontos,Palavra,Erros,Tempo,Data
        fprintf(arquivo, "%s,%d,%s,%d,%.1f,%s\n",
                nome, pontos, palavra, erros, tempo, data_str);

        fclose(arquivo);
    }
}

#define MAX_RANKING 100

int compararPoints(const void *a, const void *b)
{
    Player *ra = (Player *)a;
    Player *rb = (Player *)b;
    return rb->pontos - ra->pontos; // Ordem decrescente
}

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

    // Ler todas as linhas do arquivo
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo) && count < MAX_RANKING)
    {
        // Remover quebra de linha do final
        linha[strcspn(linha, "\n")] = 0;

        // Pular linhas vazias
        if (strlen(linha) == 0)
            continue;

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

    // Ordenar por pontuação (maior primeiro)
    if (count > 0)
    {
        qsort(rankings, count, sizeof(Player), compararPoints);
    }

    // Mostrar as top 10
    limparTela();
    printf("-=-=-=-=-=-=-=- TOP 10 PONTUAÇÕES REGISTRADAS -=-=-=-=-=-=-=-\n\n");
    printf("Pos. Nome       Pontos  Palavra     Erros Tempo   Data\n");
    printf("----------------------------------------------------------------------\n");

    int limite = (count > 10) ? 10 : count;
    for (int i = 0; i < limite; i++)
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