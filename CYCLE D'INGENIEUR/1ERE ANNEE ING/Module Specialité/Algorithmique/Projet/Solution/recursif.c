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

// Fonction récursive avec affichage du chemin
int tresor(int i, int j) {
    printf("(%d, %d) -> ", i + 1, j + 1); // Affiche la salle actuelle
    if (i == N - 1 && j == M - 1) {
        printf("Exit\n"); // Arrivée à la sortie
        return valeur[i][j];
    }
    if (i == N - 1) return valeur[i][j] + tresor(i, j + 1); // Aller à droite
    if (j == M - 1) return valeur[i][j] + tresor(i + 1, j); // Aller en bas
    int gain_bas = tresor(i + 1, j);
    int gain_droite = tresor(i, j + 1);
    return valeur[i][j] + (gain_bas > gain_droite ? gain_bas : gain_droite);
}

int main() {
    printf("Valeur maximale : %d\n", tresor(0, 0));
    return 0;
}
