#include <math.h>
#include <stdio.h>

// default 8x8 dimensions - apenas quando não especificado no terminal
#ifndef dimensions
#define dimensions 8
#endif
// configura a consideração global para falsa por default - pode ser alterado no terminal
#ifndef global
#define global 1
#endif

int mov[dimensions * dimensions][1 + 8] = {0};  // 1[freedom] + 8[movimentos]
int tile[dimensions][dimensions] = {0};         // output concerns
int dim;

void setup();
void updateKnightMoves();                         // atualizacao global da matriz de tiles e freedom
int updateKnightMoves(int x, int y, int assign);  // atualicao local de tiles nao visitadas. Retorna freedom do tile[x][y]
void displayChessBoard();
void displayKnightMovements();
void ride(int x, int y, int track);
int bestMove(int x, int y, int mv[9]);
int qualCasa(int x, int y, int mv);
int marginalidade(int i);
void marginalidade();

int passo = 1;

void update() {
    int x = 7;
    int y = 7;
    passo = 1;

    setup();

    updateKnightMoves(x, y, 0);

    tile[x][y] = 1;

    displayChessBoard();
    displayKnightMovements();

    ride(x, y, 1);

    displayChessBoard();

    displayKnightMovements();

    // int move = bestMove(x, y, mov[y * 8 + x]);
    // printf("\n\t %d %d,%d\n", move, qualCasa(x, y, move) / 8, qualCasa(x, y, move) % 8);
}

void ride(int x, int y, int track) {
    int i = y * dim + x;

    int mvmnt = bestMove(x, y, mov[i]);  // calcula o melhor passo ou interrompe a execução em endpoints
    printf(" x = %d , y = %d    ; mvmnt: %d\t", x, y, mvmnt);

    if (track != 0) {       // diminui a liberdade e invalida o movimento considerado dessa casa (CASO JÁ NÃO TENHA SIDO FEITO)
        mov[i][0]--;        // a casa ocupada perde um de liberdade, pois o knight se moverá dali
        mov[i][mvmnt] = 0;
        mov[qualCasa(x,y,mvmnt)][9-mvmnt] = 0;
        printf("--------- %d --------------", qualCasa(x,y,mvmnt));  // o movimento que o knight fará já não estará disponível (para, caso houver backtracking, ele não ser repetido)
    }

    passo++;
    int nx = qualCasa(x, y, mvmnt) % dim;
    int ny = qualCasa(x, y, mvmnt) / dim;

    printf(" para: %d , %d\n", nx, ny);

    tile[nx][ny] = passo;
    displayChessBoard();
    displayKnightMovements();
   // ride(nx, ny, 0);  // não disconta próxima casa pq já foi discontado (tracked)
}

// dada uma certa casa, retorna o índice do melhor movimento
int bestMove(int x, int y, int mv[9]) {
    int menor = (mv[1] > 0) ? mv[1] : 9;  // inicialmente atribui a menor liberdade ao primeiro mov ou uma liberdade invalida e alta
    int best = 0;

    printf("bestMove da %d,%d : [%d]", x,y, y*dim+x);
    for (int i = 1; i < 9; i++) {
        printf("%d",mv[i]);
    }
    if (mv[0] <= 0) {
        displayChessBoard();
        displayKnightMovements();
        printf("Casa %d,%d ilhada no passo %d. Prosseguir com backtracking!", x, y, passo);
        printf("fuck     %d", updateKnightMoves(x, y, 0));
        exit(1);
        return 0;
    }

    for (int i = 1; i < 9; i++) {
        int mvi = mv[i];

        if (mv[i] <= 0) {
            continue;
        }

        if (tile[qualCasa(x, y, i) % dim][qualCasa(x, y, i) / dim] > 0) {
            continue;
        }
        if (mv[i] > 0) {
            // a próxima casa já tem sua liberdade e movimento de retorno restringido
            mov[qualCasa(x, y, i)][0]--;        // toda casa válida testada perde um de freedom pois não se poderá voltar a casa ocupada
            mov[qualCasa(x, y, i)][9 - i] = 0;  // o movimento reverso ao que leva a estas casas passa ser inválido
            printf("\n\n\n-------\n\n");
            printf("%d,  %d , %d", qualCasa(x,y,i), mov[qualCasa(x, y, i)][0],mov[qualCasa(x, y, i)][9 - i]);
        }
        if (mv[i] > 0 && mv[i] < menor) {  // testa qual movimento leva a uma casa com menor liberdade
            menor = mv[i];
            best = i;
        } else if (mv[i] > 0 && mv[i] == menor) {  // caso as liberdades forem iguais, seleciona a de maior marginalidade
            int margBest = marginalidade(qualCasa(x, y, best));
            int margMv = marginalidade(qualCasa(x, y, i));

            if (margMv >= margBest) {
                menor = mv[i];
                best = i;
            }
        }
    }
    return best;
}

void marginalidade() {
    printf("\n //////     MARGINALIDADE     //////// \n");
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            printf(" %d ", marginalidade((j * 8) + i));
            // printf(" |%d#%d| ", tile[i][j], mov[i * dim + j][0]);
        }
        printf("\n");
    }
}

int marginalidade(int i) {  // retorna a marginalidade da casa. Quanto maior, mais periférica é a casa
    float y = i / dim;
    float x = i % dim;
    float r = (dim + 1) / 2.0;
    float c = round(round((float)(r - (x + 1)) * (float)(r - (x + 1))) + round((float)(r - (y + 1)) * (float)(r - (y + 1))));
    return c;
}

// dado uma casa em i[8][8] de origem e movimento, retorna a casa para qual o movimento leva em i[64]
int qualCasa(int x, int y, int mv) {
    int ml, mc;

    if (mv <= 4) {  // movimentos da esquerda
        ml = -2 + (mv - 1) / 2;
        mc = (3 - abs(ml)) * pow(-1, mv - 1);
    } else if (mv >= 5) {  // movimentos da direita
        ml = -1 + (mv - 1) / 2;
        mc = (3 - abs(ml)) * pow(-1, mv - 1);
    }
    int yy = y + mc;
    int xx = x + ml;
    if (xx >= 0 && xx < dim && yy >= 0 && yy < dim && tile[xx][yy] <= 0) {  // se casa válida e não visitada
        return yy * dim + xx;
    } else {
        // printf("-------------------- %d %d, %d", xx, yy, mv);
        // exit(1);
        return -1;
    }
}

// atualiza os movimentos de uma casa [tile] específico e retorna sua acessibilidade ou freedom.
int updateKnightMoves(int x, int y, int assign) {
    int freedom = 0;
    int mvmnt = 0;

    // for e condições que determinam os 8 movimentos do cavalo
    for (int c = -2; c <= 2; c++) {
        if (c == 0) continue;
        int l = (((abs(c) + 2) % 2) + 1);

        mvmnt++;
        // todas as variações a direita -já que c é sempre positivo
        if (x + c >= 0 && x + c < dim && y + l >= 0 && y + l < dim && tile[x + c][y + l] <= 0) {
            freedom++;
            if (assign != 0) mov[y * dim + x][mvmnt] = updateKnightMoves(x + c, y + l, 0);
        }
        mvmnt++;
        // todas avariações a esquerda, já que c é sempre positivo
        if (x + c >= 0 && x + c < dim && y - l >= 0 && y - l < dim && tile[x + c][y - l] <= 0) {
            freedom++;
            if (assign != 0) mov[y * dim + x][mvmnt] = updateKnightMoves(x + c, y - l, 0);
        }
    }
    mov[y * dim + x][0] = freedom;
    return freedom;
}

void updateKnightMoves() {  // reseta movimentos de toda a matriz do tabuleiro - útil na iicialização
    // for que determina a matriz
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            int freedom = 0;
            int mvmnt = 0;  // mapaeia e denomina (numera) cada movimento do cavalo neste algoritmo
            // movimentos que são opostos tem sempre somatória 9. Ou seja, mvmnt de ida + volta = 9;

            // for e condições que determinam os 8 movimentos do cavalo
            for (int l = -2; l <= 2; l++) {
                if (l == 0) continue;
                int c = (((abs(l) + 2) % 2) + 1);
                mvmnt++;
                // todas as variações a direita -já que c é sempre positivo
                if (x + c >= 0 && x + c < dim && y + l >= 0 && y + l < dim) {
                    freedom++;
                    mov[y * dim + x][mvmnt] = updateKnightMoves(x + c, y + l, 0);
                }
                mvmnt++;
                // todas avariações a esquerda, já que c é sempre positivo
                if (x - c >= 0 && x - c < dim && y + l >= 0 && y + l < dim) {
                    freedom++;
                    mov[y * dim + x][mvmnt] = updateKnightMoves(x - c, y + l, 0);
                }
            }
            mov[y * dim + x][0] = freedom;
        }
    }
}

void setup() {
    dim = dimensions;
    // duplo for que percorre a matriz
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            tile[x][y] = 0;  // atribui -1 inicialmente a todos os tiles (output concerns)

            int i = 0;
            while (i < dim) {               // for que percorre os 8 mov[dim * dim][8] de cada title[dim][dim]
                mov[(y * dim) + x][i] = 0;  // atribui -1 para a freedom e pra os 8 mov
                i++;
            }
        }
    }
    if (global != 0) {
        for (int y = 0; y < dim; y++)
            for (int x = 0; x < dim; x++)
                updateKnightMoves(x, y, 1);  // atualiza todos os movimentos de todos os tiles da matriz
    } else {
        updateKnightMoves();
    }
}

void displayChessBoard() {  // desenha a matriz com /step#freedom/
    printf("\n //////     STEPS AND FREEDOM     //////// \n");
    for (int j = 0; j < dim; j++) {
        for (int i = 0; i < dim; i++) {
            printf(" %d ", tile[i][j]);
            // printf(" |%d#%d| ", tile[i][j], mov[i * dim + j][0]);
        }
        printf("\n");
    }
}

void displayKnightMovements() {  // desenha a matriz com /step#freedom/
    printf("\n ****     KNIGHT MOVES    *****\n");
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            printf(" |");
            for (int k = 0; k < 9; k++) {  // for que percorre os mvmnts (movimentos)
                printf("%d", mov[j * dim + i][k]);
            }
            printf("| ");
        }
        printf("\n");
    }
}
