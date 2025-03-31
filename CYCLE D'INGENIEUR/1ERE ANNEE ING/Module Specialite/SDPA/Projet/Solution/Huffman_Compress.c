#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h> 

#define MAX_CHAR 256

// Structure d'un nœud de l'arbre de Huffman
typedef struct node {
    char data;          // Caractère stocké dans le nœud
    uint64_t frq;       // Fréquence du caractère (utilisation de uint64_t en cas de Donnée Voluminaux)
    bool useData;       // Indicateur si le champ data est utilisé (true = vrai, false = faux)
    struct node *left;  // Pointeur vers le fils gauche
    struct node *right; // Pointeur vers le fils droit
} Node;

// Structure pour stocker un tableau de nœuds
typedef struct tabNode {
    Node **array;       // Tableau de pointeurs vers des nœuds
    uint64_t size;      // Taille actuelle du tableau (utilisation de uint64_t, en cas de donnée volumineux et vraiment des cas exceptionnelle)
} tab;

// Créer un nouveau nœud
Node* createNode(char data, uint64_t frq, bool useData) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->frq = frq;
    newNode->useData = useData;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Initialiser le tableau de nœuds
tab* createTab() {
    tab* newTab = (tab*)malloc(sizeof(tab));
    newTab->array = (Node**)malloc(MAX_CHAR * sizeof(Node*));
    newTab->size = 0;
    return newTab;
}

// Inséretion d' un nœud dans le tableau en ordre croissant de fréquence
void insertInOrder(tab* t, Node* node) {
    uint64_t i = t->size; 
    while (i > 0 && t->array[i - 1]->frq > node->frq) {
        t->array[i] = t->array[i - 1];
        i--;
    }
    t->array[i] = node;
    t->size++;
}

// Extraire le noeud avec la plus petite fréquence
Node* extractMin(tab* t) {
    if (t->size == 0) return NULL;
    Node* minNode = t->array[0];
    for (uint64_t i = 1; i < t->size; i++) {
        t->array[i - 1] = t->array[i];
    }
    t->size--;
    return minNode;
}

// Construction l'arbre de Huffman
Node* buildHuffmanTree(tab* t) {
    while (t->size > 1) {
        Node* left = extractMin(t);
        Node* right = extractMin(t);
        Node* parent = createNode('\0', left->frq + right->frq, false);

        if (left->frq < right->frq) {
            parent->left = left;
            parent->right = right;
        } else {
            parent->left = right;
            parent->right = left;
        }

        insertInOrder(t, parent);
    }
    return t->array[0];
}

// Fonction pour compter les fréquences des caractères dans un fichier
void countFrequencies(const char* filename, uint64_t freq[MAX_CHAR]) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    uint32_t ch;
    while ((ch = fgetc(file)) != EOF) {
        freq[(unsigned char) ch]++;
    }

    fclose(file);
}

// Fonction pour générer les codes binaires en parcourant l'arbre de Huffman
void generateCodes(Node* root, char* code, uint64_t depth, char codes[MAX_CHAR][MAX_CHAR]) {
    if (root == NULL) return;

    if (root->useData) {
        code[depth] = '\0';
        strcpy(codes[(uint8_t)root->data], code);
        return;
    }

    code[depth] = '0';
    generateCodes(root->left, code, depth + 1, codes);

    code[depth] = '1';
    generateCodes(root->right, code, depth + 1, codes);
}

// Ecrire l'en-tête dans le fichier compressé
void writeHeader(FILE* outputFile, char codes[MAX_CHAR][MAX_CHAR]) {
    for (uint64_t i = 0; i < MAX_CHAR; i++) {
        if (strlen(codes[i]) > 0) { // ici, je prends les caractere qui existe dans le fichier à compresser
            fprintf(outputFile, "%c:%s\n", (char)i, codes[i]);
        }
    }
    fprintf(outputFile, "----\n");
}

// Texte compressé dans le fichier
void writeCompressedText(FILE* inputFile, FILE* outputFile, char codes[MAX_CHAR][MAX_CHAR]) {
    size_t buffer_size = 1024;
    char buffer[buffer_size];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), buffer_size, inputFile)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            fprintf(outputFile, "%s", codes[(unsigned char)buffer[i]]);
        }
    }
}

// compresstion le fichier
void compressFile(const char* inputFilename, const char* outputFilename, Node* root) {
    FILE* inputFile = fopen(inputFilename, "rb");
    FILE* outputFile = fopen(outputFilename, "wb");

    if (!inputFile || !outputFile) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char codes[MAX_CHAR][MAX_CHAR] = {0};
    char code[MAX_CHAR];
    generateCodes(root, code, 0, codes);

    writeHeader(outputFile, codes);
    writeCompressedText(inputFile, outputFile, codes);

    fclose(inputFile);
    fclose(outputFile);
}

int main(int argc, char *argv[]) {
    if (argc < 3) { 
        printf("Usage: %s <nom_du_fichier_input> <nom_du_fichier_output>\n", argv[0]);
        return 1;
    }

    const char* inputFilename = argv[1];
    const char* outputFilename = argv[2]; 
    uint64_t freq[MAX_CHAR] = {0}; // 64 pour etre à l'aise


    countFrequencies(inputFilename, freq);

 
    tab* t = createTab();

    // Ajouter les nœuds dans le tableau
    for (uint64_t i = 0; i < MAX_CHAR; i++) {
        if (freq[i] > 0) {
            Node* newNode = createNode((char)i, freq[i], true);
            insertInOrder(t, newNode);
        }
    }

    // Construirtion de l'arbre de Huffman
    Node* root = buildHuffmanTree(t);

    // Compresser le fichier
    compressFile(inputFilename, outputFilename, root);
    printf("Fichier compressé avec succès dans : %s\n", outputFilename);

    // Libérer la mémoire
    free(t->array);
    free(t);

    return 0;
}