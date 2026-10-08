#include<stdio.h>
#define MAX_DIM 100

int leerMatriz(FILE *entrada, int matriz[MAX_DIM][MAX_DIM], int filas, int col){
    for(int i = 0; i < filas; i++){
        for(int j = 0; j < col; j++){
            fscanf(entrada, "%d", &matriz[i]);
        }
    }

    return matriz;
}

void matrizEspiral(FILE *salida, int matriz[MAX_DIM][MAX_DIM], int filas, int col){
    int arriba = 0;
    int abajo = filas - 1;
    int der = col - 1;
    int izq = 0;

    while(arriba <= abajo && der <= izq){

         for(int i = izq; i < der; ){
            fprintf(salida, "%d\n", matriz[arriba][i])
         }
         arriba++
    }
}
int main(){

FILE *entrada, *salida;
entrada = fopen("matriz.txt", "r");

if(entrada == NULL){
    puts("No se pudo leer el archivo\n");
}else{
    salida = fopen("espiral.txt", "w");


}





    return 0;
}