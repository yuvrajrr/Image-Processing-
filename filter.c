#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel {
    unsigned char r, g, b;
} Pixel;

typedef struct image {
    unsigned int width, height, max;
    Pixel **data;
} Image;

Image *readPPM(char *file_name){

    FILE *file = fopen(file_name, "r");
    if (!file){
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        return NULL;
    }
    int value;
    char format[3];
    value = fscanf(file, "%s\n", format);
    if(strcmp(format, "P3")){
        return NULL;
    }
 
    Image *image = malloc(sizeof(Image));
         
    if(fscanf(file, "%u %u %u", &image->width, &image->height, &image->max) != 3){
        return NULL;
    }
    int t, j, i;
    image->data = malloc(sizeof(Pixel *) * image->height);
    for(t = 0; t < image->height; t++){
        image->data[t] = malloc(sizeof(Pixel) * image->width);
    }

    for(i = 0; i < image->height; i++){
        for(j = 0; j < image->width; j++){
            int pixels_read = fscanf(file, "%hhu %hhu %hhu", &image->data[i][j].r, &image->data[i][j].g, &image->data[i][j].b);
            if(pixels_read != 3){
                return NULL;
            }
        }
    }

    t = value;


    fclose(file);
    return image;

}


int writePPM(char *file_name, Image *image){

    FILE *file = fopen(file_name, "w");
    if (!file){
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        return -1;
    }

    fprintf(file, "P3\n");
    fprintf(file, "%u %u\n", image->width, image->height);
    fprintf(file, "%u\n", image->max);
    int i,j;
    for (i = 0; i < image->height; i++){
        for (j = 0; j < image->width; j++){
            fprintf(file, "%u %u %u ", image->data[i][j].r, image->data[i][j].g, image->data[i][j].b);
        }
        fprintf(file, "\n");
    }    

    fclose(file);
    return 0;
}

int subtractor(int n){
    int starter;
    starter = (n/2)+1;
    return starter;
}

int filter(char* image, char* kernelinput, char* output){

    Image *image1 = malloc(sizeof(Image));
    image1 = readPPM(image);
    /* Open File */
    FILE *kernel = fopen(kernelinput, "r");
    int n, multiplier;
    
    /* Scans the size of array and multiplier */
    int value;
    value = fscanf(kernel, "%d %d" , &n, &multiplier);
    int matrix[n*n], i=0,u;

    /* Scans the numbers and puts them in a matrix */
    for(u=(n*n)-1;u>-1;u--){       
        value = fscanf(kernel, "%d", &matrix[u]);
    }

    /* Finds the starter value */
    int starter;
    starter = n - subtractor(n);
    Image *image2 = malloc(sizeof(Image));
    int j = 0, k = 0;
    image2->data = malloc(sizeof(Pixel *) * image1->height);
    for(i = 0; i < image1->height; i++){
        image2->data[i] = malloc(sizeof(Pixel) * image1->width);
        }
    image2 -> height = image1 -> height;
    image2 -> width = image1 -> width;
    image2 -> max = image1 -> max;
    int firstInd=0,secondInd=0,RowCount=0;
    for(i=0; i<image1->height;i++){
        for(j = 0; j<image1->width;j++){
            int adderR=0,adderG=0,adderB=0;
            for(k = 0; k<n*n; k++){
                if(k ==0){
                    firstInd = i - starter;
                    secondInd = j - starter;
                }
                if(firstInd>=0 && secondInd >=0 && firstInd < image1->height && secondInd < image1->width){
                    adderR += matrix[k]*image1->data[firstInd][secondInd].r;
                    adderG += matrix[k]*image1->data[firstInd][secondInd].g;
                    adderB += matrix[k]*image1->data[firstInd][secondInd].b;
                }
                secondInd += 1;
                RowCount += 1;
                if(RowCount == n){
                    firstInd += 1;
                    secondInd = j - starter;
                    RowCount = 0;
                }

                if(k == (n*n)-1){
                    adderR = adderR/multiplier;
                    adderG = adderG/multiplier;
                    adderB = adderB/multiplier;

                    if(adderR < 0)
                        adderR =0;
                    if(adderG < 0)
                        adderG = 0;
                    if(adderB<0)
                        adderB = 0;

                    if(adderR > image2->max)
                        adderR = image2->max;

                    if(adderG > image2->max)
                        adderG = image2->max;
                    
                    if(adderB > image2->max)
                        adderB = image2->max;

                    image2->data[i][j].r = adderR;
                    image2->data[i][j].g = adderG;
                    image2->data[i][j].b = adderB;
                    RowCount = 0;
                }
            }
        }
    }
    n = value;
    writePPM(output,image2);  
    return 0;
}

int main(int argc, char** argv){

     if (argc != 4) {
        printf("Usage: ./filter input.ppm kernel output.ppm\n");
        return -1;
    }

    char *imageinput = argv[1];
    char *kernelinput = argv[2];
    char *outputfile = argv[3];

    filter(imageinput, kernelinput, outputfile);

    return 0;

}

