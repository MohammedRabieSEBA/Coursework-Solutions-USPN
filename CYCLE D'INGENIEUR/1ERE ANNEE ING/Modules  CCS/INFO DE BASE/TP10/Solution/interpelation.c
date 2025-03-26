#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DEGREE 100
#define EPSILON 0.000000001

typedef struct {
    int degre;
    double coeff[MAX_DEGREE];
} Polynome;

Polynome creer_polynome() {
    Polynome P;
    P.degre = 0;
    for (int i = 0; i < MAX_DEGREE; i++) {
        P.coeff[i] = 0.0;
    }
    return P;
}

void ajuster_degre(Polynome *P) {
    while (P->degre > 0 && fabs(P->coeff[P->degre]) < EPSILON) {
        P->coeff[P->degre] = 0;
        P->degre--;
    }
}

Polynome mult_scalaire(Polynome P, double lambda) {
    Polynome result = P;
    for (int i = 0; i <= P.degre; i++) {
        result.coeff[i] *= lambda;
    }
    ajuster_degre(&result);
    return result;
}



Polynome add(Polynome P1, Polynome P2) {
    Polynome result = creer_polynome();
    result.degre = (P1.degre > P2.degre) ? P1.degre : P2.degre;
    
    for (int i = 0; i <= result.degre; i++) {
        result.coeff[i] = P1.coeff[i] + P2.coeff[i];
    }
    ajuster_degre(&result);
    return result;
}

Polynome mult(Polynome P1, Polynome P2) {
    Polynome result = creer_polynome();
    result.degre = P1.degre + P2.degre;

    for (int i = 0; i <= P1.degre; i++) {
        for (int j = 0; j <= P2.degre; j++) {
            result.coeff[i + j] += P1.coeff[i] * P2.coeff[j];
        }
    }
    ajuster_degre(&result);
    return result;
}

Polynome interpole(int k, double X[], double Y[]) {
    Polynome result = creer_polynome();
    
    for (int i = 0; i < k; i++) {
        Polynome Li = creer_polynome();
        Li.coeff[0] = 1.0;

        for (int j = 0; j < k; j++) {
            if (i != j) {
                Polynome temp = creer_polynome();
                temp.coeff[0] = -X[j];
                temp.coeff[1] = 1.0;
                Li = mult(Li, temp);
                Li = mult_scalaire(Li, 1.0 / (X[i] - X[j]));
            }
        }
        Li = mult_scalaire(Li, Y[i]);
        result = add(result, Li);
    }
    
    ajuster_degre(&result);
    return result;
}

void valeurs(Polynome P, double debut, double fin, double pas) {
    printf("X\tY\n");
    for (double x = debut; x <= fin; x += pas) {
        double y = 0;
        for (int i = 0; i <= P.degre; i++) {
            y += P.coeff[i] * pow(x, i);
        }
        printf("%.2f\t%.2f\n", x, y);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <entree.txt> <sortie.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        printf("Erreur : impossible d'ouvrir le fichier d'entrée\n");
        return EXIT_FAILURE;
    }

    int k;
    fscanf(input, "%d", &k);
    
    double X[k], Y[k];
    for (int i = 0; i < k; i++) {
        fscanf(input, "%lf %lf", &X[i], &Y[i]);
    }
    fclose(input);

    Polynome P = interpole(k, X, Y);

    FILE *output = fopen(argv[2], "w");
    if (!output) {
        printf("Erreur : impossible d'ouvrir le fichier de sortie\n");
        return EXIT_FAILURE;
    }

    fprintf(output, "%d\n", P.degre);
    for (int i = 0; i <= P.degre; i++) {
        fprintf(output, "%.8lf ", P.coeff[i]);
    }
    fprintf(output, "\n");
    fclose(output);

    return EXIT_SUCCESS;
}
