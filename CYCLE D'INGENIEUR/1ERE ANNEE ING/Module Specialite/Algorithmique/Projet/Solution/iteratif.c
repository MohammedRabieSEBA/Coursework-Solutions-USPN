#include <stdio.h>

// Grille d'exemple
#define N 8
#define M 8
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
int gain[N][M];

// Calculer le tableau gain
void calculerGain() {
    for (int i = N - 1; i >= 0; i--) {
        for (int j = M - 1; j >= 0; j--) {
            if (i == N - 1 && j == M - 1) {
                gain[i][j] = valeur[i][j];
            } else if (i == N - 1) {
                gain[i][j] = valeur[i][j] + gain[i][j + 1];
            } else if (j == M - 1) {
                gain[i][j] = valeur[i][j] + gain[i + 1][j];
            } else {
                gain[i][j] = valeur[i][j] + (gain[i + 1][j] > gain[i][j + 1] ? gain[i + 1][j] : gain[i][j + 1]);
            }
        }
    }
}

// Reconstruire et afficher le chemin optimal
void afficherChemin() {
    int i = 0, j = 0;
    printf("Chemin : ");
    while (i < N - 1 || j < M - 1) {
        printf("(%d, %d) -> ", i + 1, j + 1);
        if (i == N - 1) {
            j++; // Aller à droite
        } else if (j == M - 1) {
            i++; // Aller en bas
        } else if (gain[i + 1][j] > gain[i][j + 1]) {
            i++; // Aller en bas
        } else {
            j++; // Aller à droite
        }
    }
    printf("(%d, %d) (Exit)\n", i + 1, j + 1);
}

int main() {
    calculerGain();
    printf("Valeur maximale : %d\n", gain[0][0]);
    afficherChemin();
    return 0;
}
