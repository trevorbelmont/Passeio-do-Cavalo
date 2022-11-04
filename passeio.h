#include <math.h>

// dim 20 e xy 90 (11,5) não faz nenhum backtrack
// dim 31 e xy (5,5)  e dim 40 xy(2,2) tb não
#ifndef DIMENSIONS
#define DIMENSIONS 8
#endif

// estrutura de dados usada para cada casa do tabulerio
struct pos {
    int x;  // coordenada X.
    int y;  // coordenada Y.
    // as coordenadas x e y são usadas sobretudo pela função "qualCasa(x,y,mvmnt)" para retornar a posição que o movimento leva.

    int a;  // acessibilidade ou liberdade de movimentos possíveis a partir da casa

    int mvmnt[9];  // vetor de movimentos possíveis. cada índice guarda o nível de acessibilidade, a, da casa que aquele movimento leva
    // Exemplo:  mvmnt[8] = 2  (Fazendo o movimento 8 chega-se a uma casa com 2 de acessibilidade/liberdade.)

    int persistent[9];  // guarda em que passo o movimento do índice foi feito.
    // Ex: persistent[3] = 32 (O movimento 3 já foi tentado anteriormente nessa casa no passo 32)

    int m;      // valor estático da marginalidade da casa (em função da posição e do tamanho do tabuleiro)
    int p;      // registra o passo que passa por esta casa naquele passeio.
    int prevX;  // a coordenada x da casa do passo anterior
    int prevY;  // coordenda y da casa do passo anterior
} typedef pos;

void setup();                         // prepara tabuleiro
void limpa();                         // reseta variáveis persistentes após execução
int marginalidade(int x, int y);      // retorna marginalidade daquela casa
void setMarginalidade(int display);   // inicializa marginalidade de todas as casas. Imprime matriz de marginalidade se display != 0
void displayChessBoard(int simples);  // Imprime o tabuleiro alinhado e passeio atual de maneira visualmente organizada.
void displayKnightMovements();        // Imprime a matriz dos movimentos restantes e acessibilidades de cada casa.
void displayRide();                   // Imprime o tabuleiro e passeio conforme especificado no TP.

int updateKnightMoves(int x, int y, int assign);  // atualiza os movimentos possíveis e retorna a acessibilidade da casa.
pos qualCasa(int x, int y, int mv);               // retorna a posição que o movimento mv leva a partir da casa x,y.
int bestMove(int x, int y);                       // retorna o melhor movimento daquela casa ou 0 se não houver movimentos disponíveis.

// Função recursiva que calcula o melhor movimento e cavalga para próxima casa ou retorna a casa anterior (se não houver movimentos livres);
void ride(int x, int y, int ridingBack);  // ridingBack != 0; está retornando (fazendo backtrack)

pos tile[DIMENSIONS][DIMENSIONS];  // declara matriz do tabuleiro n x n
int dim;                           // declaa variável de dimensão do tabuleiro
int passo;                         // declara variável do passo de cada casa no passeio
int visitadas, regredidas;           // respectivamente contador de passos totais e de retornos das tentativas

//Durante o desenvolvimento considerei mais intuitivo nomear as variáveis x e y como coordenadas cartesianas (não como entradas de matriz).
//Dentro função passeio, portanto, o par ordenado (x,y) designa uma certa posição (horizontal, vertical). Ou seja, (coluna, linha).
// A sintaxe da chamada da função, no entanto, segue a ordem definida na especificação do TP: passeio (linha, coluna).
void passeio(int y, int x) {  
    // adequa o valor da entrada x e y,[1,8], aos valores do código, [0,7]
    x--;
    y--;

    // prepara tabuleiro e inicializa as variáveis para seus valores iniciais devidos
    setup();

    // a casa inicial recebe o valor de passo 1
    tile[x][y].p = passo = visitadas = 1;

    ride(x, y, 0);
    displayRide(); // imprime passeio de acordo com as normas do TP
    //displayChessBoard(1); // imprime passeio alinhado e visualmente organizado (mais fácil visualizar)
    limpa();
}

// void de cavalga percorrendo as casas
void ride(int x, int y, int ridingBack) {  // track = diminui acessibilidade da casa // ridingback = fazendo backTrack ou não
    if (passo <= 0 && visitadas > 8) {
        return;
    }

    // Se está fazendo backtracking e há um movimento ainda não explorado naquela casa e NAQUELE PASSO
    if (ridingBack && tile[x][y].a > 0) {
        // for duplo que percorre a matriz do tabuleiro
        for (int j = 0; j < dim; j++) {
            for (int i = 0; i < dim; i++) {
                updateKnightMoves(i, j, 1);  // atualiza as movimentações possíveis, mvmnt[], de todas as casas
            }
        }

        // for que percorre o vetor persistent APENAS da casa atual
        for (int i = 1; i < 9; i++) {
            if (tile[x][y].persistent[i] == passo) {  // checa, no vetor persistent[i], se o movimento i já foi tentado neste mesmo passo
                tile[x][y].mvmnt[i] = 0;              // se o movimento i já foi tentado antes neste msm passo, o moviento i se torna indisponível
                tile[x][y].a--;                       // acessibilidade portanto diminui
            }
        }
        tile[x][y].persistent[0]++;  // incrementa o número de tentativas mal-sucedidas na casa (essa variável não é usada)
    }

    int move = bestMove(x, y);  // calcula o melhor moviemnto. Caso não haja movimentos disponíveis retorna zero.

    if (move <= 0) {  // não achou saída
        // retorna pra casa e passo anterior para tentar outra movimentação
        passo--;
        regredidas++;
        tile[x][y].p = 0;  // a casa sem saída atual tem o contador de passo zerado

        ride(tile[x][y].prevX, tile[x][y].prevY, 1);  // retrocede para casa anterior. (cavalga de volta)
        // o último argumento do void ride = 1, sinaliza que se está retornando (fazendo backtrack)
        return;
    }

    tile[x][y].a--;  // a casa ocupada perde um de liberdade, pois o knight se moverá dali
    tile[x][y].mvmnt[move] = 0;
    tile[x][y].persistent[move] = passo;

    passo++;

    int nx = qualCasa(x, y, move).x;
    int ny = qualCasa(x, y, move).y;

    tile[nx][ny].p = passo;
    tile[nx][ny].prevX = x;
    tile[nx][ny].prevY = y;

    visitadas++;

    if (passo == dim * dim) {
        return;
    }
    ride(nx, ny, 0);  // cavalga para próxima casa
    return;
}

// dada uma certa casa, retorna o índice do melhor movimento. Retorna zero se não houver movimento  disponível
int bestMove(int x, int y) {
    int menor = (tile[x][y].mvmnt[1] > 0) ? tile[x][y].mvmnt[1] : 9;  // inicialmente atribui a menor liberdade ao primeiro mvmnt ou uma liberdade invalida e alta
    int best = (tile[x][y].mvmnt[1] > 0) ? 1 : 0;                     // inicialmente atribui o índice de melhor movimento ao primeiro mvmnt se disponnível. Caso não disponível, atribui ao índice zero

    if (tile[x][y].a <= 0) {  // caso não haja movimentos possíveis (ou não testados neste passo) retorna
        return 0;
    }

    for (int i = 1; i < 9; i++) {  // laço que testa os 8 movimentos do cavalo
        int mvi = tile[x][y].mvmnt[i];

        if (tile[x][y].mvmnt[i] <= 0) {  // movimento inválido ou leva para casa já vistiada
            continue;
        }
        pos candidata = qualCasa(x, y, i);  // a posição da casa que o movimento i leva (a possível casa analisada nesta iteração)
        int xx = candidata.x;
        int yy = candidata.y;

        if (tile[xx][yy].p > 0) {  // não deveria ser necessário (checa se a casa já foi visitada)
            continue;
        }
        if (tile[x][y].mvmnt[i] > 0) {      // a casa candidata já tem sua liberdade e movimento de retorno restringido
            tile[xx][yy].a--;               // toda casa válida testada perde 1 de acessibilidade, pois não se poderá voltar a casa ocupada
            tile[xx][yy].mvmnt[9 - i] = 0;  // invalida o movimento da possível casa de destino que leva a casa atual
        }
        if (tile[x][y].mvmnt[i] > 0 && tile[x][y].mvmnt[i] < menor) {  // testa qual movimento leva a uma casa com menor liberdade
            menor = tile[x][y].mvmnt[i];
            best = i;
        } else if (tile[x][y].mvmnt[i] > 0 && tile[x][y].mvmnt[i] == menor) {  // caso as liberdades forem iguais, seleciona a de maior marginalidade
            int margBest = tile[qualCasa(x, y, best).x][qualCasa(x, y, best).y].m;
            int margMv = tile[xx][yy].m;

            if (margMv >= margBest) {  // caso as marginalidades também forem iguais, seleciona a útlima checada
                menor = tile[x][y].mvmnt[i];
                best = i;
            }
        }
    }
    return best;
}

// Recebe as coordenadas de uma casa de origem e um movimento (mv) e retorna a casa de destino.
pos qualCasa(int x, int y, int mv) {
    int ml, mc;

    // determina a variação de linha e coluna que o movimento indica
    // Movimentos pares são para cima, movimentos ímpares para baixo (olhar documentação)
    if (mv <= 4) {                             // movimentos da esquerda
        mc = -2 + (mv - 1) / 2;                // calcula variação de coluna em função do movimento
        ml = (3 - abs(mc)) * pow(-1, mv - 1);  // calcula a variação de linha de acordo com a coluna.
    } else if (mv >= 5) {                      // movimentos da direita
        mc = -1 + (mv - 1) / 2;                // calcula variação de coluna em função do movimento
        ml = (3 - abs(mc)) * pow(-1, mv - 1);  // calcula variação de linha em função da coluna
    }

    pos p;  // posição determinada pelo movimento mv a partir daquela casa
    p.y = y + ml;
    p.x = x + mc;

    if (p.x >= 0 && p.x < dim && p.y >= 0 && p.y < dim && tile[p.x][p.y].p <= 0) {  // se casa válida e não visitada
        return p;                                                                   // retorna a nova posição
    } else {                                                                        // caso tenha havido algum erro, continua na mesma casa
        pos same;
        same.x = x;
        same.y = y;
        return same;  // retorna a casa atual (a mesma que já se está)
    }
}

// atualiza os movimentos possíveis de uma casa [tile] específica. Retorna sua acessibilidade (a atual liberdae de movimentos daquela casa)
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
// reinicializa as variaveis com valor 0, para a próxima execução
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
// reseta as variáveis para seus valores iniciais de acessibilidade, movimentos e persistencia)
void setup() {
    visitadas = regredidas = passo = 0;
    dim = DIMENSIONS;

    for (int y = 0; y < dim; y++) {  // for duplo for que percorre a matriz (tabuleiro)
        for (int x = 0; x < dim; x++) {
            tile[x][y].p = 0;            // atribui 0 inicialmente a todas as casas (tiles)
            updateKnightMoves(x, y, 1);  // seta os valores iniciais de acessibilidade e dos movimentos inicialmente possíveis
            int i = 0;
            while (i < 9) {  // laço que inicializa com zero a persistencia de todas as casas
                tile[x][y].persistent[i] = 0;
                i++;
            }
        }
    }
    setMarginalidade(0);  // atribui a marginalidade de cada casa de acordo com as dimensões do tabuleiro
}
// retorna a marginalidade em função da posição da casa. Quanto maior, mais periférica é a casa.
int marginalidade(int x, int y) {
    float r = (dim + 1) / 2.0;
    float c = round(round((float)(r - (x + 1)) * (float)(r - (x + 1))) + round((float)(r - (y + 1)) * (float)(r - (y + 1))));
    return c;
}

// atribui a marginalidade de cada casa de acordo com as dimensões do tabuleiro.
void setMarginalidade(int display) {
    if (display) printf("\n //////     MARGINALIDADE     //////// \n");
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            tile[x][y].m = marginalidade(x, y);
            if (display != 0) printf(" %d ", tile[x][y].m);
        }
        if (display) printf("\n");
    }
}

// desenha a matriz (tabuleiro) em função dos passos de maneira visualmente organizada
void displayChessBoard(int simple) {
    // simple != 0 : imprimie apenas os passos do passeio
    // simple == 0 : imprime passos do passeio e a atual acessibilidade daquela casa
    //printf("\n //////     TABULEIRO     //////// \n");
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            if (tile[x][y].p < 10) {
                if (simple) printf("0%-1d ", tile[x][y].p);
                if (!simple) printf(" |0%1d#%d| ", tile[x][y].p, tile[x][y].a);
            } else {
                if (simple) printf("%-2d ", tile[x][y].p);
                if (!simple) printf(" |%2d#%d| ", tile[x][y].p, tile[x][y].a);
            }
        }
        printf("\n");
    }
    printf("%d %d\n", visitadas, regredidas);
}

void displayKnightMovements() {  // desenha a matriz com /step#freedom/
    printf("\n ****     KNIGHT MOVEMENTS    *****\n");
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

// Imprime tabuleiro e passeio conforme especificações do TP.
void displayRide() {
    for (int y = 0; y < dim; y++) {
        for (int x = 0; x < dim; x++) {
            printf("%d ", tile[x][y].p);
        }
        printf("\n");
    }
    printf("%d %d\n", visitadas, regredidas);
}
