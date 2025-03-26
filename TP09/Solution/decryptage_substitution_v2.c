#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define ALPHABET_SIZE 26
#define INITIAL_TEMPERATURE 10.0
#define COOLING_RATE 0.1 // je dois ici faire diminuer la temp avec le rate de cooling
#define MAX_ITERATIONS 1000

double ****quadgram_tab;

// Déclaration des fonctions
void allocate_quadgram_table();
void free_quadgram_table();
void load_quadgrams(const char* filename);
double compute_score(const char* input);
void permute(char* password);
void decrypt(const char* input, char* output, const char* password);
double acceptance_probability(double old_score, double new_score, double temperature);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <encrypted_file> <quadgram_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    allocate_quadgram_table();
    load_quadgrams(argv[2]);


    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening encrypted file");
        free_quadgram_table(); 
        return EXIT_FAILURE;
    }

    char *encrypted_text = (char *)malloc(10000 * sizeof(char));
    if (!encrypted_text) {
        perror("Memory allocation failed for encrypted text");
        fclose(input_file);
        free_quadgram_table(); 
        return EXIT_FAILURE;
    }
    fgets(encrypted_text, 10000, input_file);
    fclose(input_file);

    char password[ALPHABET_SIZE + 1] = "abcdefghijklmnopqrstuvwxyz";
    char best_password[ALPHABET_SIZE + 1];
    strcpy(best_password, password);

    double best_score = compute_score(encrypted_text);
    printf("Initial score: %.2f\n", best_score);

    double temperature = INITIAL_TEMPERATURE; // ici je donne la temperature 10

    srand(time(NULL));
    char temp_password[ALPHABET_SIZE + 1];

  
    while (temperature > 0) {
        for (int i = 0; i< 1000;i++) {
        strcpy(temp_password, best_password);

        permute(temp_password);

    
        char *decrypted_text = (char *)malloc((strlen(encrypted_text) + 1) * sizeof(char));
        if (!decrypted_text) {
            perror("Memory allocation failed for decrypted text");
            free(encrypted_text);
            free_quadgram_table();
            return EXIT_FAILURE;
        }

        decrypt(encrypted_text, decrypted_text, temp_password);

        double current_score = compute_score(decrypted_text);
        double r = (double)((int) rand() % 100)/100;
        printf("r = %.2f \n",r);
        // Quand j'accepte le nouveau mdp ?
        if (current_score > best_score || acceptance_probability(best_score, current_score, temperature) > r) {
            strcpy(best_password, temp_password);
            best_score = current_score;
            printf("Accepted new password with score: %.2f\n", best_score);
        }

        free(decrypted_text);
        
        }
        temperature -= COOLING_RATE; // je soustraaire 0.1
    }

    char *final_output = (char *)malloc((strlen(encrypted_text) + 1) * sizeof(char));
    if (!final_output) {
        perror("Memory allocation failed for final output");
        free(encrypted_text);
        free_quadgram_table(); 
        return EXIT_FAILURE;
    }

    decrypt(encrypted_text, final_output, best_password);


    FILE *output_file = fopen(argv[3], "w");
    if (!output_file) {
        perror("Error opening output file");
        free(final_output); 
        free(encrypted_text);
        free_quadgram_table(); 
        return EXIT_FAILURE;
    }

    fprintf(output_file, "%s", final_output);
    fclose(output_file);

    printf("Decryption completed with best score: %.2f\n", best_score);

    free(final_output);
    free(encrypted_text);
    free_quadgram_table();

    return EXIT_SUCCESS;
}

// Fonction pour calculer la probabilité d'acceptation
double acceptance_probability(double old_score, double new_score, double temperature) {
    if (new_score > old_score) {
        return 1.0;
    }
    return exp((new_score - old_score) / temperature);
}

void allocate_quadgram_table() {
    quadgram_tab = (double ****)malloc(ALPHABET_SIZE * sizeof(double ***));
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        quadgram_tab[i] = (double ***)malloc(ALPHABET_SIZE * sizeof(double **));
        for (int j = 0; j < ALPHABET_SIZE; j++) {
            quadgram_tab[i][j] = (double **)malloc(ALPHABET_SIZE * sizeof(double *));
            for (int k = 0; k < ALPHABET_SIZE; k++) {
                quadgram_tab[i][j][k] = (double *)malloc(ALPHABET_SIZE * sizeof(double));
                // J'initialise le tableau avec des zero pour eviter les probleme que ça peut générer
                for (int l = 0; l < ALPHABET_SIZE; l++) {
                    quadgram_tab[i][j][k][l] = 0.0;
                }
            }
        }
    }
}

void free_quadgram_table() {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        for (int j = 0; j < ALPHABET_SIZE; j++) {
            for (int k = 0; k < ALPHABET_SIZE; k++) {
                free(quadgram_tab[i][j][k]);
            }
            free(quadgram_tab[i][j]);
        }
        free(quadgram_tab[i]);
    }
    free(quadgram_tab);
}

void load_quadgrams(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening quadgram file");
        exit(EXIT_FAILURE);
    }

    char quadgram[5];
    double frequency;
    while (fscanf(file, "%4s %lf", quadgram, &frequency) == 2) {
        int a = quadgram[0] - 'a';
        int b = quadgram[1] - 'a';
        int c = quadgram[2] - 'a';
        int d = quadgram[3] - 'a';

        quadgram_tab[a][b][c][d] += frequency;

       

        //printf("Quadgram [%c][%c][%c][%c]: %.5f\n", a+'a', b+'a', c+'a', d+'a', quadgram_tab[a][b][c][d]);

    }

    fclose(file);
}



double compute_score(const char* input) {
    double score = 0.0;
    for (int i = 0; input[i + 3] != '\0'; i++) {
        int a = input[i] - 'a';
        int b = input[i + 1] - 'a';
        int c = input[i + 2] - 'a';
        int d = input[i + 3] - 'a';

       

        double quadgram_value = quadgram_tab[a][b][c][d];

        //printf("le quadgram : %.2f pour %c %c %c %c \n",quadgram_value,input[i],input[i+1],input[i+2],input[i+3]);

        
        // printf("Quadgram [%c][%c][%c][%c]: %.5f\n", a + 'a', b + 'a', c + 'a', d + 'a', quadgram_value);

        if (quadgram_value > 0) {
            score += log(quadgram_value);
        } else {
           // ici je soute les quadgram nul
            //printf("Skipped quadgram [%c][%c][%c][%c] with value %.5f\n", a + 'a', b + 'a', c + 'a', d + 'a', quadgram_value);
        }
    }
    return score;
}



void permute(char* password) {
    int i = rand() % ALPHABET_SIZE; 
    int j = rand() % ALPHABET_SIZE;
    printf("i = %d, j = %d \n",i,j);
    char temp = password[i];
    password[i] = password[j];
    password[j] = temp;
}


void decrypt(const char* input, char* output, const char* password) {
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] >= 'a' && input[i] <= 'z') {
            output[i] = password[input[i] - 'a'];
        } else {
            output[i] = input[i]; // en cas de text où y a trop de ponctuation et tt
        }
    }
    output[strlen(input)] = '\0'; 
}
