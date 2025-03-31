#include <stdio.h>
#include <limits.h>

#define N 8
#define M 8
#define MAX_K 20

// entre parenthere, le matrice en bas, on peut la modifier comme on veut !
int valeur[N][M] = {
    {0, 3, 5, 4, 2, 3, 2, 4},
    {2, 3, 6, 1, 1, 1, 1, 6},
    {4, 1, 1, 2, 4, 3, 3, 2},
    {4, 2, 5, 3, 3, 4, 3, 5},
    {2, 1, 4, 4, 2, 3, 2, 3},
    {2, 3, 4, 3, 1, 2, 1, 2},
    {5, 4, 1, 2, 3, 1, 1, 1},
    {5, 3, 2, 3, 2, 1, 2, 0}
};
int gain[N][M][MAX_K + 1];

int tresorAvecK(int i, int j, int k) {
    if (k < 0) return INT_MIN; // -infini refer au Dépassement du nombre maximal de salles
    if (i == N - 1 && j == M - 1) return valeur[i][j];
    if (gain[i][j][k] != -1) return gain[i][j][k];
    int gain_bas = (i + 1 < N) ? tresorAvecK(i + 1, j, k - 1) : INT_MIN;
    int gain_droite = (j + 1 < M) ? tresorAvecK(i, j + 1, k - 1) : INT_MIN;
    gain[i][j][k] = valeur[i][j] + (gain_bas > gain_droite ? gain_bas : gain_droite);
    return gain[i][j][k];
}

int main() {
    int k = 55; // ici j'ai pris un cas de 15, mais on peut en prendre autre chose
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            for (int l = 0; l <= MAX_K; l++)
                gain[i][j][l] = -1;

    printf("Valeur maximale avec k=%d : %d\n", k, tresorAvecK(0, 0, k));
    return 0;
}
