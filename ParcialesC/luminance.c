/* 
 * Fecha: [Tu Fecha]
 * Nombre: [Tu Nombre]
 * Apellido: [Tu Apellido]
 * Cédula: [Tu Cédula]
 * Número Sección: [Tu Sección]
 * Número del Modelo del Parcial: 2
 */

#include <stdio.h>
#include <math.h>

// Estructura para almacenar un píxel RGB
typedef struct {
    int r, g, b;
} Pixel;

// Función para calcular la luminancia relativa (L)
double calcular_luminancia(Pixel p, int profundidad_color) {
    double r_norm = (double)p.r / profundidad_color;
    double g_norm = (double)p.g / profundidad_color;
    double b_norm = (double)p.b / profundidad_color;

    double r_lin = pow(r_norm, 2.2);
    double g_lin = pow(g_norm, 2.2);
    double b_lin = pow(b_norm, 2.2);

    return 0.2126 * r_lin + 0.7152 * g_lin + 0.0722 * b_lin;
}

int main() {
    char nombre_entrada[100];
    char nombre_salida[100];
    int indico_valor;
    double L;

    // 1. Pedir nombre de la imagen y valor de luminancia
    printf("Ingresa el nombre de la imagen original y luminancia (ej. personaje.ppm); (0.0-1.0 / -1 para usar promedio relativo): ");
    if (scanf("%99s", nombre_entrada) != 1) return 1;
    if (scanf("%lf", &L) != 1) return 1;

    // Validar la luminancia ingresada
    if (L >= 0.0 && L <= 1.0) {
        indico_valor = 1; // usar valor ingresado
    } else if (L < 0.0) {
        indico_valor = 0; // usar promedio
    } else {
        printf("Valor incorrecto de luminancia\n");
        return 1;
    }
    
    // Pedir nombre de archivo de salida
    printf("Ingresa el nombre para guardar el resultado (ej. salida.ppm): ");
    if (scanf("%99s", nombre_salida) != 1) return 1;

    // 2. Abrir el archivo original para lectura ("r")
    FILE *archivo_in = fopen(nombre_entrada, "r");
    if (archivo_in == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", nombre_entrada);
        return 1;
    }

    // 3. Crear/Abrir el archivo de destino para escritura ("w")
    FILE *archivo_out = fopen(nombre_salida, "w");
    if (archivo_out == NULL) {
        printf("Error: No se pudo crear el archivo %s\n", nombre_salida);
        fclose(archivo_in);
        return 1;
    }

    char formato[3];
    int ancho, alto, profundidad_color;

    // Leer la cabecera P3 desde el archivo de entrada
    fscanf(archivo_in, "%2s", formato);
    fscanf(archivo_in, "%d %d", &ancho, &alto);
    fscanf(archivo_in, "%d", &profundidad_color);

    int total_pixeles = ancho * alto;
    Pixel p;
    double umbral_limite;

    // --- PRIMERA PASADA: Calcular el promedio si el usuario ingresó -1 ---
    if (indico_valor == 0) {
        double suma_luminancia = 0.0;
        for (int i = 0; i < total_pixeles; i++) {
            fscanf(archivo_in, "%d %d %d", &p.r, &p.g, &p.b);
            suma_luminancia += calcular_luminancia(p, profundidad_color);
        }
        
        umbral_limite = suma_luminancia / total_pixeles;

        // Regresar el cursor al inicio del archivo de entrada para la segunda lectura
        rewind(archivo_in);
        
        // Volver a leer la cabecera para ignorarla en la segunda pasada
        fscanf(archivo_in, "%2s", formato);
        fscanf(archivo_in, "%d %d", &ancho, &alto);
        fscanf(archivo_in, "%d", &profundidad_color);
        
        printf("\nPromedio calculado automaticamente: %f\n", umbral_limite);
    } else {
        // Si el usuario indicó un valor, lo pasamos al umbral límite
        umbral_limite = L;
    }
    
    // --- ESCRIBIR CABECERA EN EL ARCHIVO DE SALIDA ---
    fprintf(archivo_out, "%s\n", formato);
    fprintf(archivo_out, "%d %d\n", ancho, alto);
    fprintf(archivo_out, "%d\n", profundidad_color);

    // --- SEGUNDA PASADA: Aplicar Umbral y guardar en el archivo de salida ---
    for (int i = 0; i < total_pixeles; i++) {
        
        // Leer píxel
        fscanf(archivo_in, "%d %d %d", &p.r, &p.g, &p.b);
        
        // Calcular la luminancia del píxel actual usando una variable local
        double L_pixel = calcular_luminancia(p, profundidad_color);
        
        // Escribir los píxeles modificados directamente en el archivo nuevo
        if (L_pixel < umbral_limite) {
            fprintf(archivo_out, "0 0 0\n"); // Negro
        } else {
            fprintf(archivo_out, "%d %d %d\n", profundidad_color, profundidad_color, profundidad_color); // Blanco
        }
    }
    
    // Cerrar ambos archivos para asegurar que los datos se guarden correctamente
    fclose(archivo_in);
    fclose(archivo_out);
    
    printf("\nExito: La imagen filtrada se ha guardado en el archivo '%s'.\n", nombre_salida);

    return 0;
}
