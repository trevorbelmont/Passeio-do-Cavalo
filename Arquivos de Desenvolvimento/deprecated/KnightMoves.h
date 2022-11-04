#include <math.h>
#include <stdio.h>

#define dimensions 8

// int tile[dimensions][dimensions];
int nn = dimensions;

int tile[dimensions][dimensions] = {[0 ... 7][0 ... 7] = -1};

void displayChessBoard() {
    printf("\n\n");
    for (int i = 0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            printf("%d ", tile[i][j]);
        }
        printf("\n");
    }
}
void consider(int x, int y, int i) {
    if (i > ceil(nn / 2) + 1) return;
    if (i == 0) tile[x][y] = 0;
    i++;

    for (int c = -2; c <= 2; c++) {
        if (c == 0) continue;
        int xp = (((abs(c) + 2) % 2) + 1);

        if (x + xp >= 0 && x + xp < nn && y + c >= 0 && y + c < nn) {
            tile[x + xp][y + c] = (tile[x + xp][y + c] > i || tile[x + xp][y + c] < 0) ? i : tile[x + xp][y + c];
            consider(x + xp, y + c, i);
        }
        if (x - xp >= 0 && x - xp < nn && y + c >= 0 && y + c < nn) {
            tile[x - xp][y + c] = (tile[x - xp][y + c] > i || tile[x - xp][y + c] < 0) ? i : tile[x - xp][y + c];
            consider(x - xp, y + c, i);
        }
    }
}

void update() {
    int x0, y0, xf, yf;
    do {
        scanf("%d %d", &x0, &y0);
        scanf("%d %d", &xf, &yf);
        x0--;
        y0--;
        xf--;
        yf--;
    } while (x0 >= nn || y0 >= nn || x0 < 0 || y0 < 0 || xf >= nn || yf >= nn || xf < 0 || yf < 0);

    consider(x0, y0, 0);
    displayChessBoard();

    printf("To get from %d,%d to %d,%d takes %d knight moves.\n\n", x0, y0, xf, yf, tile[xf][yf]);

    update();
}

void main() {
    update();
}