#include <math.h>
#include <stdio.h>

// dim 20 xy 90 não faz nenhum backtrack
#ifndef DIMENSIONS
#define DIMENSIONS 8
#endif

#ifndef XY
#define XY 0
#endif

struct pos {
    int x;
    int y;
    int a;
    int mvmnt[9];
    int persistent[9];
    int m;
    int p;
    int prevX;
    int prevY;
} typedef pos;

void setup();
void limpa();
int marginalidade(int x, int y);
void setMarginalidade(int display);
void displayChessBoard(int simple);
void displayKnightMovements();

int updateKnightMoves(int x, int y, int assign);
pos qualCasa(int x, int y, int mv);
int bestMove(int x, int y);
int ride(int x, int y, int track, int ridingBack);

pos tile[DIMENSIONS][DIMENSIONS];
int dim, passo;

int allSteps, backSteps;

void update() {
    /* int x = XY % DIMENSIONS;
    int y = XY / DIMENSIONS;  // temporario com define. fazer com scanf
 */

    int x = 0, y = 0;
    printf("digite as coordenadas: ");
    scanf("%d %d", &x, &y);
    x--;
    y--;
    setup();

    tile[x][y].p = passo = allSteps = 1;

    // displayChessBoard(0);
    // displayKnightMovements();

    ride(x, y, 1, 0);

    // displayChessBoard(1);
    // displayKnightMovements();
    printf("\n end of update:::::: PASSO: %d\n", passo);
}

int ride(int x, int y, int track, int ridingBack) {  // track = diminui acessibilidade da casa // ridingback = fazendo backTrack ou não
    if (passo <= 0 && backSteps > 2) {
        printf("fudeu! passo = %d  backtracking = %d  allSteps = %d\n", passo, backSteps, allSteps);
        exit(0);
    }

    if (ridingBack && tile[x][y].a > 0) {
        for (int j = 0; j < dim; j++) {
            for (int i = 0; i < dim; i++) {
                updateKnightMoves(i, j, 1);
            }
        }
        for (int i = 1; i < 9; i++) {
            if (tile[x][y].persistent[i] == passo) {
                tile[x][y].mvmnt[i] = 0;
                tile[x][y].a--;
            }
        }
        tile[x][y].persistent[0]++;
    }

    int move = bestMove(x, y);  // calcula o melhor passo ou interrompe a execução em endpoints
    printf(" x = %d , y = %d    ; mvmnt: %d\t ", x, y, move);

    if (move <= 0) {  // não achou saída
        printf("\n\tdeadend! BACKTRACKIN' NO passo: %d: prev : %d,%d\n\n", tile[x][y].p, tile[y][y].prevX, tile[x][y].prevY);
        // vai voltar até um passo que tenha opção, escolherá o melhor movimento antes de
        // atualizar os movimentos (pra não levar em consideração os passos já tomados)

        passo--;
        backSteps++;
        tile[x][y].p = 0;

        ride(tile[x][y].prevX, tile[x][y].prevY, 1, 1);  // último argumento diz se está fazendo backTrack (true)
        return 0;
    }

    if (track != 0) {    // diminui a liberdade e invalida o movimento considerado dessa casa (CASO JÁ NÃO TENHA SIDO FEITO)
        tile[x][y].a--;  // a casa ocupada perde um de liberdade, pois o knight se moverá dali
        tile[x][y].mvmnt[move] = 0;
        tile[x][y].persistent[move] = passo;
    }

    if (move != 0) {
        passo++;
    } else {  // não necessário e inalcançável
        return 0;
    }

    int nx = qualCasa(x, y, move).x;
    int ny = qualCasa(x, y, move).y;

    printf(" para: %d , %d\t passo: %d\n", nx, ny, passo);

    tile[nx][ny].p = passo;
    tile[nx][ny].prevX = x;
    tile[nx][ny].prevY = y;

    allSteps++;

    if (passo == dim * dim) {
        printf("\n\n\t---------SE ACABAOU---------\n");
        displayChessBoard(1);
        printf("Allsteps = %d\tbackSteps = %d\tpasso = %d\tsubtração: %d", allSteps, backSteps, passo, allSteps - backSteps);

        // displayKnightMovements();
        printf("\n\n\t---------SE ACABAOU---------\n");
        return 1;
    }
    ride(nx, ny, 1, 0);  //  disconta acessibilidade e movimentos da próxima casa (tracked)
    return 1;
}

// dada uma certa casa, retorna o índice do melhor movimento
int bestMove(int x, int y) {
    int menor = (tile[x][y].mvmnt[1] > 0) ? tile[x][y].mvmnt[1] : 9;  // inicialmente atribui a menor liberdade ao primeiro mov ou uma liberdade invalida e alta
    int best = (tile[x][y].mvmnt[1] > 0) ? 1 : 0;

    printf("bestMove da %d,%d : a = [%d]    ", x, y, tile[x][y].a);
    for (int i = 1; i < 9; i++) {
        printf("%d", tile[x][y].mvmnt[i]);
    }
    if (tile[x][y].a <= 0) {
        // printf("fuck!     liberdade da casa: %d" , updateKnightMoves(x, y, 0));
        // displayChessBoard(1);
        // displayKnightMovements();
        printf("\n\tCasa %d,%d ilhada no passo %d. Prosseguir com backtracking!\n", x, y, passo);
        printf("\tAllsteps = %d \t backSteps = %d \t , passo = %d\t", allSteps, backSteps, passo);
        // exit(1);
        return 0;
    }

    for (int i = 1; i < 9; i++) {
        int mvi = tile[x][y].mvmnt[i];

        if (tile[x][y].mvmnt[i] <= 0) {  // movimento inválido ou leva para casa já vistiada
            continue;
        }
        int xx = qualCasa(x, y, i).x;
        int yy = qualCasa(x, y, i).y;

        if (tile[xx][yy].p > 0) {  // não deveria ser necessário (checa se a casa já foi visitada)
            continue;
        }
        if (tile[x][y].mvmnt[i] > 0) {
            // a próxima casa já tem sua liberdade e movimento de retorno restringido
            tile[xx][yy].a--;               // toda casa válida testada perde um de freedom pois não se poderá voltar a casa ocupada
            tile[xx][yy].mvmnt[9 - i] = 0;  // o movimento reverso ao que leva a estas casas passa ser inválido
            // as casas que se está checando não poderão mais vir para a casa atual

            // printf("\n\n\n-------\n\n");
            // printf("%d,  %d , %d", qualCasa(x, y, i), mov[qualCasa(x, y, i)][0], mov[qualCasa(x, y, i)][9 - i]);
        }
        if (tile[x][y].mvmnt[i] > 0 && tile[x][y].mvmnt[i] < menor) {  // testa qual movimento leva a uma casa com menor liberdade
            menor = tile[x][y].mvmnt[i];
            best = i;
        } else if (tile[x][y].mvmnt[i] > 0 && tile[x][y].mvmnt[i] == menor) {  // caso as liberdades forem iguais, seleciona a de maior marginalidade
            int margBest = tile[qualCasa(x, y, best).x][qualCasa(x, y, best).y].m;
            int margMv = tile[xx][yy].m;

            if (margMv >= margBest) {
                menor = tile[x][y].mvmnt[i];
                best = i;
            }
        }
    }
    if (best == 0) {  // tirar essa linha
        best = best;
    }
    return best;
}

pos qualCasa(int x, int y, int mv) {
    int ml, mc;

    if (mv <= 4) {  // movimentos da esquerda
        ml = -2 + (mv - 1) / 2;
        mc = (3 - abs(ml)) * pow(-1, mv - 1);
    } else if (mv >= 5) {  // movimentos da direita
        ml = -1 + (mv - 1) / 2;
        mc = (3 - abs(ml)) * pow(-1, mv - 1);
    }
    pos p;
    p.y = y + mc;
    p.x = x + ml;

    if (p.x >= 0 && p.x < dim && p.y >= 0 && p.y < dim && tile[p.x][p.y].p <= 0) {  // se casa válida e não visitada
        return p;
    } else {
        printf("\n\n------ ERRO NO QUAL CASA -------------- %d,%d.  movimento: %d \n", p.x, p.y, mv);
        displayChessBoard(0);
        displayKnightMovements();

        printf("\nPasso : %d \n", passo);
        pos same;
        same.x = x;
        same.y = y;

        exit(1);

        return same;
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
        if (x + c >= 0 && x + c < dim && y + l >= 0 && y + l < dim) {  // se o movimento é válido
            if (tile[x + c][y + l].p <= 0) {                           // tile não visitado                          // se a casa não foi previamente visitada
                freedom++;
                if (assign != 0) tile[x][y].mvmnt[mvmnt] = updateKnightMoves(x + c, y + l, 0);
                if (tile[x][y].mvmnt[mvmnt] == 0) tile[x][y].mvmnt[mvmnt] = 1;  // só para garantir que movimento válido não seja 0;
            } else {
                tile[x][y].mvmnt[mvmnt] = 0;
            }
        }

        mvmnt++;
        // todas avariações a esquerda, já que c é sempre positivo
        if (x + c >= 0 && x + c < dim && y - l >= 0 && y - l < dim) {
            if (tile[x + c][y - l].p <= 0) {  // tile não visitado
                freedom++;
                if (assign != 0) tile[x][y].mvmnt[mvmnt] = updateKnightMoves(x + c, y - l, 0);
                if (tile[x][y].mvmnt[mvmnt] == 0) tile[x][y].mvmnt[mvmnt] = 1;  // só para garantir que movimento válido não seja 0;
            } else {
                tile[x][y].mvmnt[mvmnt] = 0;
            }
        }
    }
    tile[x][y].a = freedom;
    return freedom;
}

void limpa() {
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            tile[x][y].p = 0;  // atribui -1 inicialmente a todos os tiles (output concerns)
            int i = 0;
            while (i < 9) {  // loop que percorre os 8 mvmnt e persistent de cada title[dim][dim]
                tile[x][y].persistent[i] = tile[x][y].mvmnt[i] = 0;
                i++;
            }
        }
    }
}

void setup() {
    allSteps = backSteps = 0;
    dim = DIMENSIONS;
    int counter = 0;
    // duplo for que percorre a matriz
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            tile[x][y].p = 0;            // atribui -1 inicialmente a todos os tiles (output concerns)
            updateKnightMoves(x, y, 1);  // atualiza todos os movimentos de todos os tiles da matriz
            int i = 1;
            while (i < 9) {                    // loop que percorre os 8 mov[dim * dim][8] de cada title[dim][dim]
                tile[x][y].persistent[i] = 0;  // atribui -1 para a freedom e pra os 8 mov
                i++;
            }
            tile[x][y].persistent[0] = 0;  // tile[x][y].a;
        }
    }

    setMarginalidade(0);
}
int marginalidade(int x, int y) {  // retorna a marginalidade da casa. Quanto maior, mais periférica é a casa

    float r = (dim + 1) / 2.0;
    float c = round(round((float)(r - (x + 1)) * (float)(r - (x + 1))) + round((float)(r - (y + 1)) * (float)(r - (y + 1))));
    return c;
}

void setMarginalidade(int display) {
    if (display)    printf("\n //////     MARGINALIDADE     //////// \n");
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            tile[x][y].m = marginalidade(x, y);
            if (display != 0)   printf(" %d ", tile[x][y].m);
        }
        if (display)    printf("\n");
    }
}

void displayChessBoard(int simple) {  // desenha a matriz com /step#freedom/
    printf("\n //////     TABULEIRO     //////// \n");
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            if (tile[x][y].p < 10) {
                if (simple) printf(" 0%d ", tile[x][y].p);
                if (!simple) printf(" |0%d#%d| ", tile[x][y].p, tile[x][y].a);
            } else {
                if (simple) printf(" %d ", tile[x][y].p);
                if (!simple) printf(" |%d#%d| ", tile[x][y].p, tile[x][y].a);
            }
        }
        printf("\n");
    }
}

void displayKnightMovements() {  // desenha a matriz com /step#freedom/
    printf("\n ****     KNIGHT MOVES    *****\n");
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            printf(" |");

            // printf("%d-", tile[x][y].p);
            printf("%d-", tile[x][y].a);

            for (int k = 1; k < 9; k++) {  // for que percorre os mvmnts (movimentos)
                printf("%d", tile[x][y].mvmnt[k]);
            }
            printf("| ");
        }
        printf("\n");
    }
}
