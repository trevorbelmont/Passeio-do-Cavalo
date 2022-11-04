#include <stdio.h>

#include "passeio.h"

int main() {
    while (1 > 0) {
        int x = 0, y = 0;
        printf("digite as coordenadas: ");
        scanf("%d %d", &x, &y);
        update(x,y);
    }
    return 0;
}