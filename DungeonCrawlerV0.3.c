#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<stdbool.h>
#include<time.h>

// ==========================================
// ESTRUCTURAS DE DATOS
// ==========================================

// 1. Estructura de Objetos
typedef struct{
    char id_Objeto[50];
    char nombreObjeto[50];
    int aumentoStats; 
    char tipoEfecto[50];
}Objeto;

// 2. Estructura del Estado del Juego
typedef struct{
    int semilla;
    int puntosHP_actual, puntosHP_maximo;
    int atqBase;
    int defBase;
    int puntosXP; 
    int nivelActual; 
    int numPiso;
    int numCuartoActual;
    Objeto inventarioJugador[100];
    int cantidadObjetosActual; 
    int tamanoInventarioMax;   
    int totalPisosMax;
    int cuartosBasePorPiso;
    int totalEnemigosDerrotados;
    int objetosConsumidos;
    bool resultadoPartida; 
}EstadoJuego;

// 3. Estructura de Entidad 
typedef struct{
    int id_Enemigo;
    char nombreEnemigo[50];
    int vidaBase;
    int danoBase;
    int defensaBase;
    char tipo[20]; 
}Entidad;

// ==========================================
// PROTOTIPOS DE FUNCIONES 
// ==========================================

// Módulo 1: Inicialización y Archivos Input
void cargarConfiguracion(EstadoJuego *estado);
void cargarBestiario(Entidad bestiario[], int *total_enemigos);
void cargarObjetos(Objeto catalogo[], int *total_objetos);

// Módulo 2: Lógica de Exploración y Navegación
void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
Entidad seleccionarEnemigo(Entidad bestiario[], int total_enemigos, int es_cuarto_final);

// Módulo 3: Sistema de Combate y Progresión
void iniciarCombate(EstadoJuego *estado, Entidad *enemigo); // Corregido de "mostrarMenuCombate" a "iniciarCombate"
int calcularDano(int ataque_atacante, int defensa_defensor);
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], int total_objetos, char tipo_enemigo[]);
void verificarSubidaNivel(EstadoJuego *estado);

// Módulo 4: Gestión de Recursos
void usarObjeto(EstadoJuego *estado);

// Módulo 5: Persistencia y Output (Highscore)
void guardarPartida(EstadoJuego estado);
int cargarPartida(EstadoJuego *estado);
void registrarHighscore(EstadoJuego estado, bool victoria);

// Menús adicionales y auxiliares (Necesarios para consola)
int mostrarMenuInicio(EstadoJuego *partida);
void menuFueraDeCombate(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
void limpiarBuffer();
void gestionarPartidaGuardada();

// ==========================================
// FUNCIÓN PRINCIPAL
// ==========================================

int main(){

    EstadoJuego partida;
        
    // Arreglos instanciados con el nombre correcto de las estructuras
    Entidad bestiario[100]; 
    Objeto catalogo[100];   
        
    int total_enemigos = 0;
    int total_objetos = 0;

    // Inicialización de variables
    partida.puntosXP = 0;
    partida.nivelActual = 1; 
    partida.numPiso = 1;    
    partida.numCuartoActual = 0; 
    partida.cantidadObjetosActual = 0; 
    partida.totalPisosMax = 0;
    partida.cuartosBasePorPiso = 1;
    partida.totalEnemigosDerrotados = 0;
    partida.objetosConsumidos = 0;
    partida.resultadoPartida = false; 
    
    cargarConfiguracion(&partida);
    cargarBestiario(bestiario, &total_enemigos);
    cargarObjetos(catalogo, &total_objetos);

    srand(partida.semilla);

    // Iniciar el juego
    mostrarMenuInicio(&partida); 

    return 0;
}

// ==========================================
// DEFINICIÓN DE FUNCIONES
// ==========================================

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int mostrarMenuInicio(EstadoJuego *partida){
    int opc;
    do{
        printf("\n");
        printf("=========================================\n");
        printf("||                                     ||\n");
        printf("||       EL DESCENSO AL NÚCLEO         ||\n");
        printf("||                                     ||\n");
        printf("=========================================\n");
        printf("  [1] Empezar nueva partida\n");
        printf("  [2] Cargar partida guardada\n");
        printf("  [3] Ver Highscores\n");
        printf("  [0] Salir del juego\n");
        printf("=========================================\n");
        printf("Elige una opcion: ");
        scanf("%d", &opc);

        limpiarBuffer();

        switch (opc){
        case 1:
            printf("Iniciando partida\n");
            break;
        case 2:
            printf("Cargando partida (si existe)\n");
            cargarPartida(partida); 
            break;
        case 3:
            printf("Highscores: ultimos 5 victorias\n");
            break;
        case 0:
            printf("Saliendo...\n");
            break;
        default:
            printf("Saliendo...\n");
            break;
        }

    }while(opc != 0); 

    return opc;
}

void menuFueraDeCombate(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos){
    int opc;
    do{ 
        printf("\n");
        printf("+---------------------------------------+\n");
        printf("| ZONA SEGURA  |  Piso: %02d | Cuarto: %02d |\n", partida->numPiso, partida->numCuartoActual);
        printf("+---------------------------------------+\n");
        printf("| HP: %03d/%03d  | Nivel: %02d | XP: %04d |\n", partida->puntosHP_actual, partida->puntosHP_maximo, partida->nivelActual, partida->puntosXP);
        printf("+---------------------------------------+\n");
        printf("  [1] Avanzar al siguiente cuarto\n");
        printf("  [2] Revisar mochila (%d/%d)\n", partida->cantidadObjetosActual, partida->tamanoInventarioMax);
        printf("  [3] Guardar y salir\n");
        printf("  [0] Salir\n");
        printf("+---------------------------------------+\n");
        printf("¿Que deseas hacer, explorador?: ");
        scanf("%d", &opc);

        limpiarBuffer();

        switch (opc){
        case 1:
            avanzarCuarto(partida, bestiario, total_enemigos, catalogo, total_objetos);
            break;
        case 2:
            usarObjeto(partida);
            break;
        case 3:
            guardarPartida(*partida);
            opc = 0; 
            break;
        default:
            printf("Saliendo...\n"); 
            break;
        }
    }while(opc != 0);
}

void iniciarCombate(EstadoJuego *estado, Entidad *enemigo) { // Modificado a los nombres requeridos
    int opc;
    bool huidaExitosa = false;
    bool turnoJugadorConsumido = false;
      
    do{ 
        turnoJugadorConsumido = false;
        printf("\n");
        printf("=========================================\n");
        printf("!          SISTEMA DE COMBATE           !\n");
        printf("=========================================\n");
        printf(" OBJETIVO: %-15s [HP: %03d] \n", enemigo->nombreEnemigo, enemigo->vidaBase);
        printf("-----------------------------------------\n");
        printf(" TÚ:       %-15s [HP: %03d/%03d]\n", "Explorador", estado->puntosHP_actual, estado->puntosHP_maximo);
        printf("=========================================\n");
        printf("  [1] Atacar objetivo\n");
        printf("  [2] Abrir mochila\n");
        printf("  [3] Intentar huir\n");
        printf("  [0] Saliendo\n");
        printf("=========================================\n");
        printf("Ejecutar accion: ");
        scanf("%d", &opc);

        limpiarBuffer();
        
        switch (opc){
        case 1:
            printf("Atacar objetivo\n");
            // int dano = calcularDano(estado->atqBase, enemigo->defensaBase);
            // enemigo->vidaBase -= dano;
            turnoJugadorConsumido = true;
            break;
        case 2:
            usarObjeto(estado);
            turnoJugadorConsumido = true;
            break;
        case 3:
            printf("Intentando huir\n");
            break;
        default:
            printf("Saliendo...\n"); 
            break;
        }

        if (turnoJugadorConsumido && enemigo->vidaBase > 0 && !huidaExitosa) {
            printf("\n¡Es el turno del %s!\n", enemigo->nombreEnemigo);
            // int danoEnemigo = calcularDano(enemigo->danoBase, estado->defBase);
            // estado->puntosHP_actual -= danoEnemigo;
        }

    }while(estado->puntosHP_actual > 0 && enemigo->vidaBase > 0 && !huidaExitosa);
}

Entidad seleccionarEnemigo(Entidad bestiario[], int total_enemigos, int es_cuarto_final) { // Cambiado a Entidad
    Entidad posibles[100]; 
    int cantidad_posibles = 0;

    for (int i = 0; i < total_enemigos; i++) {
        if (es_cuarto_final) {
            if (strcmp(bestiario[i].tipo, "Raro") == 0) {
                posibles[cantidad_posibles] = bestiario[i];
                cantidad_posibles++;
            }
        } else {
            if (strcmp(bestiario[i].tipo, "Comun") == 0) {
                posibles[cantidad_posibles] = bestiario[i];
                cantidad_posibles++;
            }
        }
    }

    if (cantidad_posibles == 0) {
        printf("\n[ERROR CRITICO]: No se encontraron enemigos validos en el bestiario para este cuarto.\n");
        exit(1);
    }

    int indice_aleatorio = rand() % cantidad_posibles;
    return posibles[indice_aleatorio];
}

void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos) { // Cambiado a Entidad
    int cuartos_del_piso = estado->cuartosBasePorPiso;
    
    if (estado->numPiso == 1) {
        cuartos_del_piso -= 1; 
    } else if (estado->numPiso == estado->totalPisosMax) {
        cuartos_del_piso += 1; 
    }

    if (estado->numCuartoActual < cuartos_del_piso) {
        estado->numCuartoActual++;
    } else {
        if (estado->numPiso < estado->totalPisosMax) {
            estado->numPiso++;
            estado->numCuartoActual = 1; 
            printf("\n--- ¡Has limpiado el piso! Desciendes al piso %d ---\n", estado->numPiso);
        } else {
            printf("\n¡Felicidades! Has superado el último cuarto y conquistado el núcleo.\n");
            estado->resultadoPartida = true; 
            return; 
        }
    }

    int es_cuarto_final = 0;
    if (estado->numCuartoActual == cuartos_del_piso) {
        es_cuarto_final = 1;
        printf("\n¡Cuidado! Has entrado al último cuarto de este piso. Una presencia imponente te aguarda...\n");
    }

    Entidad enemigoActual = seleccionarEnemigo(bestiario, total_enemigos, es_cuarto_final);
    
    printf("\nTe adentras en el cuarto %d y te encuentras con un %s.\n", estado->numCuartoActual, enemigoActual.nombreEnemigo);
    
    // Aquí invocamos el combate usando el nombre correcto
    // iniciarCombate(estado, &enemigoActual);
}

void gestionarPartidaGuardada() {
    if (remove("partida_guardada.txt") == 0) {
        printf("Partida anterior eliminada del sistema.\n");
    }
}

int cargarPartida(EstadoJuego *estado) {
    printf("[Sistema] Cargando partida... (Funcionalidad pendiente)\n");
    return 1;
}

void guardarPartida(EstadoJuego estado) {
    printf("[Sistema] Guardando partida... (Funcionalidad pendiente)\n");
}

void usarObjeto(EstadoJuego *estado) {
    printf("[Sistema] Abriendo inventario... (Funcionalidad pendiente)\n");
}

int calcularDano(int ataque_atacante, int defensa_defensor) {
    // La regla estricta indica que el daño nunca puede ser menor a 1
    int dano = ataque_atacante - defensa_defensor;
    if (dano < 1) dano = 1;
    return dano;
}

void cargarConfiguracion(EstadoJuego *estado) {
    FILE *archivo = fopen("config.txt", "r");
    
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo config.txt\n");
        exit(1); 
    }
    
    int leidos = fscanf(archivo, "%d %d %d %d %d %d %d", 
                        &estado->semilla,
                        &estado->totalPisosMax,
                        &estado->cuartosBasePorPiso,
                        &estado->puntosHP_maximo, 
                        &estado->atqBase, 
                        &estado->defBase, 
                        &estado->tamanoInventarioMax);

    if (leidos != 7) {
        printf("Error: El archivo config.txt esta corrupto o incompleto.\n");
        fclose(archivo);
        exit(1);
    }

    estado->puntosHP_actual = estado->puntosHP_maximo;
    fclose(archivo);
}

void cargarBestiario(Entidad bestiario[], int *total_enemigos) { // Cambiado a Entidad
    FILE *archivo = fopen("bestiario.txt", "r");
    
    if (archivo == NULL) {
        printf("Error: No se pudo abrir bestiario.txt\n");
        exit(1);
    }

    *total_enemigos = 0; 
    int i = 0;

    while (fscanf(archivo, "%d %s %d %d %d %s", 
                  &bestiario[i].id_Enemigo, 
                  bestiario[i].nombreEnemigo, 
                  &bestiario[i].vidaBase, 
                  &bestiario[i].danoBase, 
                  &bestiario[i].defensaBase, 
                  bestiario[i].tipo) == 6) {
        i++;
    }
    
    *total_enemigos = i; 
    fclose(archivo);
}

void cargarObjetos(Objeto catalogo[], int *total_objetos) {
    FILE *archivo = fopen("objetos.txt", "r");
    
    if (archivo == NULL) {
        printf("Error: No se pudo abrir objetos.txt\n");
        exit(1);
    }

    *total_objetos = 0;
    int i = 0;

    while (fscanf(archivo, "%s %s %d %s", 
                catalogo[i].id_Objeto, 
                catalogo[i].nombreObjeto, 
                &catalogo[i].aumentoStats, 
                catalogo[i].tipoEfecto) == 4) {
        i++;
    }

    *total_objetos = i;
    fclose(archivo);
}

// Estructuras vacías para completar luego y evitar errores de compilación
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], int total_objetos, char tipo_enemigo[]) {
    // Funcionalidad pendiente
}

void verificarSubidaNivel(EstadoJuego *estado) {
    // Funcionalidad pendiente
}

void registrarHighscore(EstadoJuego estado, bool victoria) {
    // Funcionalidad pendiente
}