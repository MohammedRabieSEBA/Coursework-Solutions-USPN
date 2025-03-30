#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>  

#define MAX_CHAR 256

// Structure pour un nœud de l'arbre de Huffman
typedef struct node {
    char data;          // Caractère stocké dans le nœud
    bool useData;       // Indicateur si le champ data est utilisé => True Sinon false
    struct node *left;  
    struct node *right; 
} Node;

// Fonction pour create un new noeud
Node* createNode(char data, bool useData) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->useData = useData;
    node->left = node->right = NULL;
    return node;
}

// Construire l'arbre de Huffman à partir de la table de codes
void construireHuffmanTree(Node** root, char character, const char* code) {
    if (*root == NULL) {
        *root = createNode('\0', false); // Crée un nœud interne sans caractère
    }
    Node* current = *root;
    while (*code) { // On essaye de suivre le cheminement de l'arbre 
        if (*code == '0') {
            if (!current->left) current->left = createNode('\0', false);
            current = current->left;
        } else if (*code == '1') {
            if (!current->right) current->right = createNode('\0', false);
            current = current->right;
        }
        code++;
    }
    current->data = character; // Stocke le caractère dans le nœud
    current->useData = true;   // Marque le champ data comme utilisé
}

// Fonction pour charger les codes de Huffman et construire l'arbre
Node* chargerHuffmanTree(FILE* inputFile) {
    Node* root = NULL;
    char character;
    char code[64];
    uint32_t codeIndex = 0;
    uint32_t readingCode = 0;
    char lastChar = '\0'; // garder la trace du dernier caractère
    uint32_t firstColonFound = 1;

    while (1) {
        uint32_t c = fgetc(inputFile);
        if (c == EOF) break;    

        // Vérifier la séquence '---' pour arrêter la lecture
        if (c == '-' && lastChar == '-' && codeIndex == 0) { // Arrêter la lecture dès qu'on rencontre '--' SUFFIT
            break; 
        }

        // Si on trouve un saut de ligne, on doit traiter les codes
        if (c == '\n' && readingCode > 0) {
            if (codeIndex > 0) { 
                // Si un code est en cours de lecture, on le termine
                code[codeIndex] = '\0'; // Terminer la chaîne de code
                construireHuffmanTree(&root, character, code); // Ajouter le caractère et son code à l'arbre
            }
            // Réinitialiser les indices pour la prochaine entrée
            codeIndex = 0;
            readingCode = 0;
            continue;
        }
        // faire la difference entre character ":" et separateur ":"
        if (c == ':') {
            long position = ftell(inputFile);
            uint32_t tmp = fgetc(inputFile);
            if (tmp == c) {
                character = ':';
            }
            else {
                fseek(inputFile, position, SEEK_SET);
            }
            // Si on trouve le caractère ':', on commence à lire le code
            readingCode = 1; // Réinitialiser l'index du code
            codeIndex = 0; // Passer à la lecture du code
            continue;
        }

        // Si on est en train de lire le code, on ajoute les bits au tableau 'code' readingCode on Progess = 1
        if (readingCode) {
            code[codeIndex++] = c; 
        } else {
            // Si on n'est pas encore en train de lire le code, c'est un caractère avant ':', readingCode on Progess = 0
            character = c; // Stocker le caractère avant ':'
        }

        lastChar = c; // Garder une trace du dernier caractère lu
    }
    return root; 
}

// Libérer l'arbre apres tout, 
void freeHuffmanTree(Node* root) {
    if (root) {
        freeHuffmanTree(root->left);
        freeHuffmanTree(root->right);
        free(root);
    }
}

// Décoder le fichier compressé
void decompressFile(const char* inputFilename, const char* outputFilename) {
    FILE* inputFile = fopen(inputFilename, "rb");
    FILE* outputFile = fopen(outputFilename, "wb");
    if (!inputFile || !outputFile) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }
    
    // Construction de l'arbre de Huffman
    Node* root = chargerHuffmanTree(inputFile);
    Node* current = root;

    uint8_t c;
    while ((c = fgetc(inputFile)) != '\n' && c != EOF) {
        // Ne rien faire, juste avancer caractère par caractère
    }
    
    // Lire les bits et parcourir l'arbre
    uint32_t bit;
    while ((bit = fgetc(inputFile)) != EOF) {
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        }
        
        if (current->useData) { // Feuille atteinte
            fputc(current->data, outputFile);
            current = root;
        }
    }
    
    fclose(inputFile);
    fclose(outputFile);
    freeHuffmanTree(root);
    printf("Fichier décompressé avec succès !\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <nom_du_fichier_compressed> <nom_du_fichier_output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* inputFilename = argv[1];
    const char* outputFilename = argv[2];
    decompressFile(inputFilename, outputFilename);
    return 0;
}