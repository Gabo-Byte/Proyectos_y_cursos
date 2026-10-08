#include <stdio.h>
#include <stdlib.h>

// El máximo de ancho soportado, ideal para leer solo una fila a la vez
#define MAX_ANCHO 8192 

typedef struct {
    int r, g, b;
} Pixel;

typedef struct {
    char formato[3]; // Para "P3"
    int alto, ancho, profundidad_color;
} CabeceraPPM;

// Retorna el valor E
int leer_escalado() {
    FILE *archivo = fopen("escalado.conf", "r");
    int escalado = 0;
    if (archivo != NULL) {
        fscanf(archivo, "%d", &escalado);
        fclose(archivo);
    }
    return escalado;
}

// Lee la cabecera e inmediatamente procesa y escala los píxeles
void procesar_y_escalar_imagen(int E) {
    CabeceraPPM cabecera;
    
    // 1. Leer cabecera original por entrada estándar
    scanf("%2s", cabecera.formato);
    scanf("%d %d", &cabecera.ancho, &cabecera.alto);
    scanf("%d", &cabecera.profundidad_color);

    // 2. Imprimir nueva cabecera escalada directo a salida estándar (printf)
    printf("%s\n", cabecera.formato);
    printf("%d %d\n", cabecera.ancho * E, cabecera.alto * E);
    printf("%d\n", cabecera.profundidad_color);

    // Arreglo para guardar SOLO una fila original a la vez
    Pixel fila_actual[MAX_ANCHO];

    // 3. Procesar fila por fila 
    for (int i = 0; i < cabecera.alto; i++) {
        
        // Cargar la fila original en memoria
        for (int j = 0; j < cabecera.ancho; j++) {
            scanf("%d %d %d", &fila_actual[j].r, &fila_actual[j].g, &fila_actual[j].b);
        }

        // Repetir la impresión de esta fila 'E' veces hacia abajo
        for (int y = 0; y < E; y++) {
            // Imprimir cada pixel 'E' veces hacia los lados
            for (int j = 0; j < cabecera.ancho; j++) {
                for (int x = 0; x < E; x++) {
                    printf("%d %d %d ", fila_actual[j].r, fila_actual[j].g, fila_actual[j].b);
                }
            }
            printf("\n");
        }
    }
}

int main() {
    int escalado = leer_escalado();

    // Validamos que el escalado sea un número natural mayor a 0
    if (escalado >= 1) {
        // Llamamos a la función que hace todo el trabajo de I/O y escalado
        procesar_y_escalar_imagen(escalado);
    }

    return 0;
}