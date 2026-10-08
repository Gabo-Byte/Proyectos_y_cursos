#include<stdio.h>

#define MAX_DIM 256

typedef struct{
    int r,g,b;
}Pixel;

typedef struct{
    char magic_number[3];
    int ancho, alto, max_color;
    Pixel matriz[MAX_DIM][MAX_DIM];
}ImagenPPM;

int leer_grados(){
    int grados = 0;
    FILE *archivo = fopen("grados.in", "r");
    
    if(archivo != NULL){
      fscanf(archivo, "%d", &grados);
      fclose(archivo);
    }
    return grados;
}

// Ahora pasamos la imagen por referencia (puntero *)
void leer_imagen_estandar(ImagenPPM *img){
    scanf(" %2s", img->magic_number);
    scanf(" %d %d", &img->ancho, &img->alto);
    scanf(" %d", &img->max_color);

    for(int i = 0; i < img->alto; i++){
        for(int j = 0; j < img->ancho; j++){
            scanf(" %d %d %d", &img->matriz[i][j].r, 
                               &img->matriz[i][j].g,
                               &img->matriz[i][j].b);
        }
    }
}

void guardar_imagen(ImagenPPM *img_final){
    FILE *archivo = fopen("icono_rotado.ppm", "w");

    if(archivo != NULL){
        fprintf(archivo, "%s\n", img_final->magic_number);
        fprintf(archivo, "%d %d\n", img_final->ancho, img_final->alto);
        fprintf(archivo, "%d\n", img_final->max_color);

        for(int i = 0; i < img_final->alto; i++){
             for(int j = 0; j < img_final->ancho; j++){
                fprintf(archivo, "%d %d %d ", img_final->matriz[i][j].r,
                            img_final->matriz[i][j].g, img_final->matriz[i][j].b);
            }
            fprintf(archivo, "\n");
        }
        fclose(archivo);
    }
}

void rotarIMG(ImagenPPM *original){
    // Creamos solo una copia temporal
    ImagenPPM nuevaIMG;
    nuevaIMG.ancho = original->alto;
    nuevaIMG.alto = original->ancho;

    for(int i = 0; i < original->alto; i++){
        for(int j = 0; j < original->ancho; j++){
            nuevaIMG.matriz[j][original->alto - 1 - i] = original->matriz[i][j];
        }
    }
    
    // Sobrescribimos la original con la rotada
    original->alto = nuevaIMG.alto;
    original->ancho = nuevaIMG.ancho;
    for(int i = 0; i < nuevaIMG.alto; i++){
        for(int j = 0; j < nuevaIMG.ancho; j++){
            original->matriz[i][j] = nuevaIMG.matriz[i][j];
        }
    }
}

int main(){
    int grados = leer_grados();
    
    // Creamos la imagen una sola vez en el main
    ImagenPPM img_actual; 
    
    // Le pasamos la dirección de memoria usando &
    leer_imagen_estandar(&img_actual);

    int cantidad_rotaciones = grados / 90;

    for(int i = 0; i < cantidad_rotaciones; i++){
        rotarIMG(&img_actual);    
    }

    guardar_imagen(&img_actual);

    return 0;
}