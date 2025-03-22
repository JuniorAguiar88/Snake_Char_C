#include <stdio.h>   // Biblioteca para entrada e saída de dados (ex: imprimir na tela)
#include <stdlib.h>  // Biblioteca para funções gerais (ex: gerar números aleatórios)
#include <time.h>    // Biblioteca para trabalhar com tempo (ex: gerar números aleatórios)
#include <windows.h> // Biblioteca para funcionalidades do Windows (ex: mudar cores no terminal)
#include <locale.h>  // Biblioteca para configurar a localidade (ex: suportar caracteres especiais)
#include <string.h>  // Biblioteca para manipulação de textos (ex: copiar nomes)

#define WIDTH 40     // Largura do campo do jogo (40 colunas)
#define HEIGHT 20    // Altura do campo do jogo (20 linhas)
#define MAX_RECORDS 10 // Número máximo de recordes que podem ser salvos

// Estrutura para armazenar o nome e a pontuação de um jogador
typedef struct {
    char name[50];  // Nome do jogador (até 50 caracteres)
    int score;      // Pontuação do jogador
} Record;

Record records[MAX_RECORDS]; // Lista de recordes (armazena até 10 recordes)
int recordCount = 0;         // Contador de quantos recordes estão salvos

// Variáveis do jogo
int gameOver;       // Controla se o jogo acabou (1 = sim, 0 = não)
int score;          // Pontuação atual do jogador
int tailX[100], tailY[100]; // Posições da cauda da cobra (até 100 partes)
int nTail;          // Número de partes da cauda da cobra
int fruitX, fruitY; // Posição da fruta no campo
int x, y;           // Posição da cabeça da cobra
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // Direções possíveis da cobra
enum eDirection dir; // Direção atual da cobra
int snakeColor;     // Cor da cobra no terminal

// Função para configurar o jogo no início
void Setup() {
    gameOver = 0;   // O jogo começa (não está acabado)
    dir = STOP;     // A cobra começa parada
    x = WIDTH / 2;  // Posiciona a cabeça da cobra no meio do campo (eixo X)
    y = HEIGHT / 2; // Posiciona a cabeça da cobra no meio do campo (eixo Y)
    fruitX = rand() % WIDTH;  // Gera uma posição aleatória para a fruta (eixo X)
    fruitY = rand() % HEIGHT; // Gera uma posição aleatória para a fruta (eixo Y)
    score = 0;      // A pontuação começa em zero
    nTail = 0;      // Cobra começa sem cauda
    snakeColor = 2; // Define a cor inicial da cobra como verde
}

// Função para desenhar o campo do jogo
void Draw() {
    system("cls");  // Limpa a tela antes de desenhar o campo

    // Desenha a borda superior do campo
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    // Desenha o campo com a cobra e a fruta
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) printf("#"); // Borda esquerda

            // Desenha a cabeça da cobra (O) ou a fruta (F)
            if (i == y && j == x)
                printf("O");
            else if (i == fruitY && j == fruitX)
                printf("F");
            else {
                int printTail = 0;
                // Verifica se há cauda na posição atual
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("<"); // Desenha a cauda
                        printTail = 1;
                    }
                }
                if (!printTail) printf(" "); // Espaço vazio
            }

            if (j == WIDTH - 1) printf("#"); // Borda direita
        }
        printf("\n");
    }

    // Desenha a borda inferior do campo
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    printf("Score: %d\n", score); // Mostra a pontuação atual
}

// Função para capturar as teclas pressionadas
void Input() {
    if (_kbhit()) { // Verifica se uma tecla foi pressionada
        int key = _getch(); // Captura a tecla pressionada
        if (key == 224) {   // Se for uma tecla especial (setas)
            key = _getch();
            switch (key) {
                case 75: dir = LEFT; break;  // Seta para a esquerda
                case 77: dir = RIGHT; break; // Seta para a direita
                case 72: dir = UP; break;    // Seta para cima
                case 80: dir = DOWN; break; // Seta para baixo
            }
        }
        else if (key == 'x') gameOver = 1; // Tecla 'x' para sair do jogo
    }
}

// Função para atualizar a lógica do jogo
void Logic() {
    // Movimenta a cauda da cobra
    int prevX = tailX[0], prevY = tailY[0];
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        int tempX = tailX[i], tempY = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = tempX;
        prevY = tempY;
    }

    // Movimenta a cabeça da cobra
    switch (dir) {
        case LEFT: x--; break;
        case RIGHT: x++; break;
        case UP: y--; break;
        case DOWN: y++; break;
    }

    // Faz a cobra aparecer do outro lado se sair do campo
    if (x >= WIDTH) x = 0; else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0; else if (y < 0) y = HEIGHT - 1;

    // Verifica se a cobra colidiu com a própria cauda
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == x && tailY[i] == y)
            gameOver = 1;

    // Verifica se a cobra comeu a fruta
    if (x == fruitX && y == fruitY) {
        score += 10; // Aumenta a pontuação
        fruitX = rand() % WIDTH; // Nova posição da fruta
        fruitY = rand() % HEIGHT;
        nTail++; // Aumenta o tamanho da cauda
    }
}

// Função para salvar o recorde
void SaveRecord(char* name, int score) {
    if (recordCount < MAX_RECORDS) {
        strcpy(records[recordCount].name, name); // Salva o nome
        records[recordCount].score = score;      // Salva a pontuação
        recordCount++; // Incrementa o contador de recordes
    } else {
        // Substitui o recorde mais baixo se a nova pontuação for maior
        int minIndex = 0;
        for (int i = 1; i < MAX_RECORDS; i++)
            if (records[i].score < records[minIndex].score)
                minIndex = i;
        if (score > records[minIndex].score) {
            strcpy(records[minIndex].name, name);
            records[minIndex].score = score;
        }
    }
}

// Função para mostrar os recordes
void ShowRecords() {
    system("cls"); // Limpa a tela
    printf("Recordes:\n");
    for (int i = 0; i < recordCount; i++)
        printf("%d. %s - %d\n", i + 1, records[i].name, records[i].score);
    printf("\nPressione qualquer tecla para voltar ao menu.\n");
    _getch(); // Espera o jogador pressionar uma tecla
}

// Função para exibir o menu principal
void ShowMenu() {
    int choice;
    do {
        system("cls"); // Limpa a tela
        printf("1. Novo Jogo\n");
        printf("2. Recorde\n");
        printf("3. Créditos\n");
        printf("4. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char name[50];
                printf("Digite seu nome: ");
                scanf("%s", name);
                Setup();
                while (!gameOver) {
                    Draw();
                    Input();
                    Logic();
                    Sleep(50); // Controla a velocidade do jogo
                }
                SaveRecord(name, score); // Salva o recorde
                printf("Game Over! Pressione 'R' para retornar ao menu.\n");
                while (_getch() != 'r'); // Espera o jogador pressionar 'R'
                break;
            }
            case 2:
                ShowRecords(); // Mostra os recordes
                break;
            case 3:
                printf("Créditos: Desenvolvido por [Júnior Aguiar]\n");
                Sleep(2000); // Espera 2 segundos
                break;
            case 4:
                exit(0); // Sai do jogo
                break;
            default:
                printf("Opção inválida! Tente novamente.\n");
                Sleep(1000); // Espera 1 segundo
                break;
        }
    } while (1); // Repete o menu até o jogador sair
}

// Função principal
int main() {
    setlocale(LC_ALL, "Portuguese"); // Configura o programa para suportar caracteres especiais
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    ShowMenu(); // Exibe o menu principal
    return 0; // Encerra o programa
}
