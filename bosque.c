#include <stdio.h>
#define MAX_DIM 100

typedef struct{
    int energia;
    int vidas;
    int bayas;
} Aventurero;

int validarPosicion(char casilla, Aventurero *persona){
    int inicio;
    // Evaluamos directamente el char de la casilla que recibimos
    if(casilla == 'S'){
        inicio = 1;
    }else if(casilla == 'B'){
        persona->energia++;
        persona->bayas++;
    }else if(casilla == 'V'){
        persona->vidas--;
    }else if(casilla == 'X'){
        persona->energia -= 2;
    }else if(casilla == '-'){
        // no hace nada
    }else{
        // Cualquier otra letra indica el final del camino o la salida
        printf("GOOD GAME\n");
        printf("El jugador recolecto %d bayas\n", persona->bayas);
        printf("Logro llegar al bosque con %d vidas y %d energias\n", persona->vidas, persona->energia);
        inicio = 0; // 0 significa "Detener el juego"
    }

    // Comprobamos si el jugador murió por el movimiento
    if(persona->vidas < 0 || persona->energia < 0){
        printf("END GAME\n");
        inicio = 0; // 0 significa "Detener el juego"
    }
    
    return inicio;
}

void recorrer_espiral_matriz(int dimension, char matriz[MAX_DIM][MAX_DIM], Aventurero *player) {
    // Definimos los 4 límites de la matriz
    int inicio_fila = 0;
    int fin_fila = dimension - 1;
    int inicio_col = 0;
    int fin_col = dimension - 1;

    // El bucle continúa mientras los límites no se crucen
    while (inicio_fila <= fin_fila && inicio_col <= fin_col) {
        
        // 1. Recorrer de Izquierda a Derecha (Fila superior)
        for (int i = inicio_col; i <= fin_col; i++) {
            if(validarPosicion(matriz[inicio_fila][i], player) == 0) return;
        }
        inicio_fila++; // Bajamos el límite superior
        
        // 2. Recorrer de Arriba a Abajo (Columna derecha)
        for (int i = inicio_fila; i <= fin_fila; i++) {
            // CORREGIDO: matriz[i][fin_col]
            if (validarPosicion(matriz[i][fin_col], player) == 0) return;
        }
        fin_col--; // Movemos el límite derecho hacia la izquierda
        
        // Verificamos si aún hay filas por recorrer
        if (inicio_fila <= fin_fila) {
            // 3. Recorrer de Derecha a Izquierda (Fila inferior)
            for (int i = fin_col; i >= inicio_col; i--) {
                if (validarPosicion(matriz[fin_fila][i], player) == 0) return;
            }
            fin_fila--; // Subimos el límite inferior
        }
        
        // Verificamos si aún hay columnas por recorrer
        if (inicio_col <= fin_col) {
            // 4. Recorrer de Abajo a Arriba (Columna izquierda)
            for (int i = fin_fila; i >= inicio_fila; i--) {
                if (validarPosicion(matriz[i][inicio_col], player) == 0) return;
            }
            inicio_col++; // Movemos el límite izquierdo hacia la derecha
        }
    }
}


int main(){

    FILE *entrada = fopen("bosque.in", "r");
    
    if(entrada == NULL){
        puts("No se pudo abrir el archivo\n");
        // CORREGIDO: Se eliminó el fclose(entrada);
        return 1;
    }else{
        int dimension; //N
        Aventurero jugador = {10, 3, 0};

        fscanf(entrada, "%d", &dimension);
        
        if(dimension > MAX_DIM){
            puts("Dimension demasiado grande\n");
            fclose(entrada);
            return 1;
        }
     
        // CORREGIDO: Se declara usando MAX_DIM para que coincida con la función
        char terreno[MAX_DIM][MAX_DIM];
        
        for(int i = 0; i < dimension; i++){
            for(int j = 0; j < dimension; j++){
                fscanf(entrada, " %c", &terreno[i][j]);
            }
        }
        
        fclose(entrada); // Se cierra tras terminar de leer
        
        recorrer_espiral_matriz(dimension, terreno, &jugador);
    }   

    return 0;
}