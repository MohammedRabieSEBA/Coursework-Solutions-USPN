#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include <time.h>

void incrustation_noir_et_blanc(unsigned char* image, unsigned char* calque, unsigned width, unsigned height) {
    for (unsigned y = 0; y < height; y++) {
        for (unsigned x = 0; x < width; x++) {
            unsigned idx = 4 * (y*width+x);
            srand(time(NULL));
            if (calque[idx] == 255 && calque[idx+1] == 255 && calque[idx+2] == 255) {
                srand(time(NULL));
                float noise = (rand()*100 % 51) - 25;
                printf("%f",noise);
                printf("\n");
                unsigned char gray = (unsigned char) ((image[idx]*1.0+image[idx+1]+image[idx+2])/3);
                unsigned char result = (unsigned char) (gray*1.0+noise);

                while (gray*1.0+noise < 0 || gray*1.0+noise > 255) {
                    noise = (rand()*100 % 51) - 25;
                    if (noise*1.0 < -25.0) {
                        noise = (((int)noise)*(-1))%25-25;
                    }
                    if (noise*1.0 > 25) {
                        noise = ((int)noise)%25;
                    }
                    result = (unsigned char) (gray*1.0+noise);
                }
                
                /*
                if ((result)< 0) {
                    noise += 255;
                    result = (unsigned char) (gray*1.0+noise);
                }

                */
                /*
                while ((unsigned char) (gray*1.0+noise) > 255) {
                    printf("je suis là dans la boucle, j'essaie de trouver un noise compris entre -25 et 25 ! \n");
                    srand(time(NULL));
                    double noise = (rand()*100 % 51) - 25;
                    result = (unsigned char) (gray*1.0+noise);
                }

                */


                if ((unsigned char) (gray*1.0+noise) > 255) {
                    result = 255;
                } 
                
                image[idx] = image[idx+1] = image[idx+2] = result;
                printf("%d",image[idx]);
                printf(" terminer avec pixel ! \n");
                
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s entree.png calque.png sortie.png\n", argv[0]);
        return EXIT_FAILURE;
    }

    unsigned char *image, *calque;
    unsigned width, height;
    
    
    if (lodepng_decode32_file(&image, &width, &height, argv[1])) {
        printf("Erreur lors du chargement de l'image\n");
        return EXIT_FAILURE;
    }
    if (lodepng_decode32_file(&calque, &width, &height, argv[2])) {
        printf("Erreur lors du chargement du calque\n");
        /*free(image);*/
        return EXIT_FAILURE;
    }

    
    incrustation_noir_et_blanc(image, calque, width, height);

    
    if (lodepng_encode32_file(argv[3], image, width, height)) {
        printf("Erreur lors de l'enregistrement de l'image\n");
        /*free(image);*/
        /*free(calque);*/
        return EXIT_FAILURE;
    }

    /*free(image);*/
        /*free(calque);*/
    return EXIT_SUCCESS;
}