/**
 * @file DungeonCrawlerUltimoDocumentado.c
 * @author Gabriel Rosario 
 * @brief Juego de tipo Dungeon Crawler en consola.
 * @details Este archivo contiene la implementación completa de un juego de exploración de mazmorras (Dungeon Crawler) basado en texto. Incluye mecánicas de combate por turnos, sistema de inventario, subida de nivel, generación aleatoria de enemigos y un sistema de guardado de estado en memoria secundaria (archivos de texto).[cite: 4]
 * @version 0.2
 * @date 2026-11-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ==========================================
// ESTRUCTURAS DE DATOS
// ==========================================

/**
 * @struct Objeto
 * @brief Estructura que representa un objeto consumible dentro del juego.
 * @details Los objetos pueden alterar los atributos temporales o permanentes del jugador, como curar puntos de vida o aumentar el ataque/defensa.
 */
typedef struct {
    int id_Objeto;           /**< Identificador numérico único del objeto. */
    char nombreObjeto[50];   /**< Nombre descriptivo del objeto (ej. "Pocion_Menor"). */
    int aumentoStats;        /**< Magnitud del efecto (cantidad de HP curado o puntos de estadística aumentados). */
    char tipoEfecto[50];     /**< Categoría del efecto (Valores válidos: Cura_Max, Cura_Vida, Buff_Dano, Buff_Defensa). */
} Objeto;

/**
 * @struct EstadoJuego
 * @brief Estructura central que almacena el estado completo de la partida en curso.[cite: 4]
 * @details Se utiliza para rastrear el progreso del jugador, sus estadísticas, su inventario y el avance dentro de la mazmorra. Es la estructura que se serializa para guardar y cargar partidas.
 */
typedef struct {
    int semilla;                    /**< Semilla para el generador de números pseudoaleatorios, garantizando consistencia. */
    int puntosHP_actual;            /**< Puntos de vida actuales del jugador. */
    int puntosHP_maximo;            /**< Puntos de vida máximos del jugador (aumentan al subir de nivel). */
    int atqBase;                    /**< Puntos de ataque base actuales del jugador. */
    int defBase;                    /**< Puntos de defensa base actuales del jugador. */
    int puntosXP;                   /**< Puntos de experiencia acumulados hacia el próximo nivel. */
    int nivelActual;                /**< Nivel actual de progresión del jugador. */
    int numPiso;                    /**< Número del piso actual en el que se encuentra el jugador. */
    int numCuartoActual;            /**< Número del cuarto actual explorado dentro del piso. */
    Objeto inventarioJugador[100];  /**< Arreglo que actúa como la mochila/inventario del jugador. */
    int cantidadObjetosActual;      /**< Número de objetos que el jugador posee actualmente. */
    int tamanoInventarioMax;        /**< Capacidad máxima de objetos que el jugador puede llevar. */
    int totalPisosMax;              /**< Condición de victoria: cantidad total de pisos a superar. */
    int cuartosBasePorPiso;         /**< Cantidad base de cuartos generados por cada piso. */
    int totalEnemigosDerrotados;    /**< Estadística: total histórico de enemigos derrotados en la partida. */
    int cuartosExplorados;          /**< Estadística: total histórico de cuartos visitados en la partida. */
    int objetosConsumidos;          /**< Estadística: total histórico de objetos utilizados por el jugador. */
    int resultadoPartida;           /**< Bandera de estado final: 0 si está en curso/derrota, 1 si es victoria. */
} EstadoJuego;

/**
 * @struct Entidad
 * @brief Estructura que define a un enemigo o adversario dentro de la mazmorra.[cite: 4]
 */
typedef struct {
    int id_Enemigo;         /**< Identificador numérico único del enemigo. */
    char nombreEnemigo[50]; /**< Nombre en pantalla del enemigo. */
    int vidaBase;           /**< Puntos de salud máximos del enemigo. */
    int danoBase;           /**< Capacidad de ataque base del enemigo. */
    int defensaBase;        /**< Capacidad de mitigación de daño del enemigo. */
    char tipo[20];          /**< Clasificación de rareza del enemigo (Valores comunes: Comun, Raro). */
} Entidad;

// ==========================================
// PROTOTIPOS DE FUNCIONES 
// ==========================================

/**
 * @brief Inicializa los parámetros de la partida leyendo el archivo de configuración.[cite: 4]
 * @pre El archivo `config.txt` debe existir en el directorio raíz y tener el formato correcto.
 * @post El `estado` contendrá los valores base de salud, pisos, tamaño de inventario y estadísticas.
 * @param estado Puntero a la estructura EstadoJuego que se inicializará.
 */
void cargarConfiguracion(EstadoJuego *estado);

/**
 * @brief Carga el catálogo de enemigos desde un archivo de texto.[cite: 4]
 * @pre El archivo `bestiario.txt` debe existir y contener datos válidos.
 * @post El arreglo `bestiario` se poblará con las entidades disponibles para el juego.
 * @param bestiario Arreglo de tipo Entidad donde se almacenarán los enemigos.
 * @param total_enemigos Puntero a entero donde se guardará la cantidad de enemigos leídos exitosamente.
 */
void cargarBestiario(Entidad bestiario[], int *total_enemigos);

/**
 * @brief Carga la base de datos de objetos interactuables desde un archivo de texto.[cite: 4]
 * @pre El archivo `objetos.txt` debe existir y contener datos válidos con efectos reconocidos.
 * @post El arreglo `catalogo` se poblará con los objetos disponibles para ser looteados.
 * @param catalogo Arreglo de tipo Objeto donde se almacenarán los ítems.
 * @param total_objetos Puntero a entero donde se guardará la cantidad de ítems leídos exitosamente.
 */
void cargarObjetos(Objeto catalogo[], int *total_objetos);

/**
 * @brief Lógica principal de exploración: avanza al jugador al siguiente cuarto o piso.[cite: 4]
 * @details Esta función gestiona la transición espacial del jugador, calcula la aparición de jefes si se está en el último cuarto, y desencadena el evento de combate correspondiente.
 * @param estado Puntero al estado actual de la partida.
 * @param bestiario Arreglo constante con los enemigos disponibles.
 * @param total_enemigos Número total de enemigos en el bestiario.
 * @param catalogo Arreglo constante con los objetos disponibles en el juego.
 * @param total_objetos Número total de objetos en el catálogo.
 */
void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);

/**
 * @brief Algoritmo de selección aleatoria de enemigos basado en la rareza requerida.[cite: 4]
 * @param bestiario Arreglo de enemigos disponibles.
 * @param total_enemigos Total de enemigos cargados en el arreglo.
 * @param es_cuarto_final Bandera (1 o 0) que indica si el cuarto actual es el último de un piso, forzando la aparición de enemigos de tipo "Raro".
 * @return Entidad Una copia del enemigo seleccionado aleatoriamente para el encuentro.
 */
Entidad seleccionarEnemigo(Entidad bestiario[], int total_enemigos, int es_cuarto_final);

/**
 * @brief Bucle principal de combate por turnos entre el jugador y una entidad.[cite: 4]
 * @details Administra las opciones de atacar, usar objetos o huir. Finaliza cuando los HP del jugador o del enemigo llegan a 0, o si el jugador huye con éxito.
 * @param estado Puntero al estado actual del jugador.
 * @param enemigo Puntero al enemigo instanciado para el combate (sus HP se modificarán aquí).
 */
void iniciarCombate(EstadoJuego *estado, Entidad *enemigo); 

/**
 * @brief Calcula el daño neto infligido en un ataque.[cite: 4]
 * @details Resta la defensa del defensor al ataque del atacante. Garantiza que el daño mínimo siempre sea 1.
 * @param ataque_atacante Valor de ataque de la entidad que realiza la ofensiva.
 * @param defensa_defensor Valor de defensa de la entidad que recibe el golpe.
 * @return int El daño neto a restar de los puntos de vida.
 */
int calcularDano(int ataque_atacante, int defensa_defensor);

/**
 * @brief Sistema de botín (loot) que decide si un enemigo suelta un objeto al ser derrotado.[cite: 4]
 * @details La probabilidad varía según el tipo de enemigo (100% para raros, 30% para comunes). Si el inventario está lleno, el objeto se descarta automáticamente.
 * @param estado Puntero al estado del juego para actualizar el inventario.
 * @param catalogo Arreglo general de objetos posibles.
 * @param total_objetos Total de objetos disponibles en el catálogo.
 * @param tipo_enemigo Cadena de texto indicando el tipo del enemigo (ej. "Comun" o "Raro").
 */
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], int total_objetos, char tipo_enemigo[]);

/**
 * @brief Comprueba y aplica la subida de nivel del jugador si se supera el umbral de experiencia.[cite: 4]
 * @details El umbral dinámico se calcula como `nivelActual * 100`. Al subir de nivel, se incrementan las estadísticas base y se restaura la salud.
 * @param estado Puntero al estado de la partida a evaluar.
 */
void verificarSubidaNivel(EstadoJuego *estado);

/**
 * @brief Despliega la interfaz de inventario y permite al jugador consumir o soltar un objeto.[cite: 4]
 * @details Aplica los efectos lógicos a las estadísticas del jugador si el objeto es consumido exitosamente. Reorganiza el arreglo del inventario tras eliminar un ítem.
 * @param estado Puntero al estado de la partida actual.
 * @return int Retorna 1 si se consumió/soltó un ítem con éxito (consumiendo un turno en combate), 0 si la acción fue cancelada.
 */
int usarObjeto(EstadoJuego *estado);

/**
 * @brief Serializa y guarda el estado actual de la partida en el disco duro.[cite: 4]
 * @post Se crea o sobrescribe el archivo `partida_guardada.txt`.
 * @param estado Copia de la estructura del estado del juego a guardar.
 */
void guardarPartida(EstadoJuego estado);

/**
 * @brief Lee y deserializa el estado de una partida guardada previamente.[cite: 4]
 * @param estado Puntero a la estructura donde se volcarán los datos recuperados.
 * @return int Retorna 1 si la partida se cargó de manera exitosa, 0 en caso de error, archivo corrupto o no encontrado.
 */
int cargarPartida(EstadoJuego *estado);

/**
 * @brief Limpia los datos residuales de guardado una vez que la partida ha terminado (victoria o derrota).[cite: 4]
 * @post El archivo `partida_guardada.txt` es eliminado del sistema si existe.
 */
void gestionarPartidaGuardada();

/**
 * @brief Guarda las estadísticas finales de la partida en el registro histórico.[cite: 4]
 * @post Se añade una nueva línea con los datos del jugador al archivo `highscore.txt`.
 * @param estado Estado final de la partida.
 * @param victoria Bandera booleana (1 para victoria, 0 para derrota).
 */
void registrarHighscore(EstadoJuego estado, int victoria);

/**
 * @brief Bucle principal de la interfaz de inicio del juego.[cite: 4]
 * @param partida Puntero al estado de la partida a iniciar.
 * @param bestiario Arreglo de enemigos cargados en el sistema.
 * @param total_enemigos Cantidad total de enemigos.
 * @param catalogo Arreglo de objetos cargados en el sistema.
 * @param total_objetos Cantidad total de objetos.
 * @return int Código de salida del menú (0 para finalizar el programa de forma segura).
 */
int mostrarMenuInicio(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);

/**
 * @brief Despliega la interfaz de "Zona Segura" entre combates.[cite: 4]
 * @details Permite al jugador revisar su inventario, avanzar de cuarto o guardar la partida y salir al menú.
 * @param partida Puntero al estado de la partida actual.
 * @param bestiario Arreglo de enemigos.
 * @param total_enemigos Cantidad total de enemigos.
 * @param catalogo Arreglo de objetos.
 * @param total_objetos Cantidad total de objetos.
 */
void menuFueraDeCombate(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);

/**
 * @brief Reinicia todos los contadores y variables del estado para comenzar una partida desde cero.[cite: 4]
 * @param partida Puntero a la estructura de la partida a limpiar.
 */
void reiniciarPartida(EstadoJuego *partida);

/**
 * @brief Función auxiliar para limpiar el buffer de entrada estándar (`stdin`).[cite: 4]
 * @details Previene problemas de lectura encadenados tras un uso inválido de `scanf`.
 */
void limpiarBuffer();

/**
 * @brief Función auxiliar multiplataforma para limpiar la pantalla de la consola.[cite: 4]
 * @details Usa `system("cls")` en entornos Windows y `system("clear")` en Unix/Linux.
 */
void limpiarPantalla();

// ==========================================
// FUNCIÓN PRINCIPAL
// ==========================================

/**
 * @brief Punto de entrada de la aplicación.[cite: 4]
 * @details Instancia las variables centrales, ejecuta los procesos de lectura de los archivos `.txt` (config, bestiario, objetos) y dispara el bucle del menú principal.
 * @return int Código 0 si la ejecución finaliza sin errores del sistema.
 */
int main() {
    EstadoJuego partida;
        
    Entidad bestiario[100]; 
    Objeto catalogo[100];   
        
    int total_enemigos = 0;
    int total_objetos = 0;

    partida.puntosXP = 0;
    partida.nivelActual = 1; 
    partida.numPiso = 1;    
    partida.numCuartoActual = 0; 
    partida.cantidadObjetosActual = 0; 
    partida.totalPisosMax = 0;
    partida.cuartosBasePorPiso = 1;
    partida.totalEnemigosDerrotados = 0;
    partida.cuartosExplorados = 0;
    partida.objetosConsumidos = 0;
    partida.resultadoPartida = 0;
    
    //Cargar archivos
    cargarConfiguracion(&partida);
    cargarBestiario(bestiario, &total_enemigos);
    cargarObjetos(catalogo, &total_objetos);

    srand(partida.semilla);

    mostrarMenuInicio(&partida, bestiario, total_enemigos, catalogo, total_objetos);

    return 0;
}

// ==========================================
// DEFINICIÓN DE FUNCIONES
// ==========================================

void limpiarBuffer() {
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void reiniciarPartida(EstadoJuego *partida) {
    cargarConfiguracion(partida);
    partida->puntosXP = 0;
    partida->nivelActual = 1;
    partida->numPiso = 1;
    partida->numCuartoActual = 0;
    partida->cantidadObjetosActual = 0;
    partida->totalEnemigosDerrotados = 0;
    partida->cuartosExplorados = 0;
    partida->objetosConsumidos = 0;
    partida->resultadoPartida = 0;
    srand(partida->semilla);
}

int mostrarMenuInicio(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos) {
    int opc;
    do {
        limpiarPantalla();
        printf("\n");
        printf("=========================================\n");
        printf("||                                     ||\n");
        printf("||       EL DESCENSO AL NUCLEO         ||\n");
        printf("||                                     ||\n");
        printf("=========================================\n");
        printf("  [1] Empezar nueva partida\n");
        printf("  [2] Cargar partida guardada\n");
        printf("  [3] Ver Highscores\n");
        printf("  [0] Salir del juego\n");
        printf("=========================================\n");

        printf("Elige una opcion: ");
        if (scanf("%d", &opc) != 1) opc = -1;
        limpiarBuffer();

        if (opc < 0) {
            printf("\nEntrada no permitida. No se aceptan numeros negativos.\n");
            printf("Presiona ENTER para continuar...");
            getchar();
            continue; 
        }

        switch (opc) {
        case 1:
            reiniciarPartida(partida);
            printf("Iniciando partida...\n");
            menuFueraDeCombate(partida, bestiario, total_enemigos, catalogo, total_objetos);
            break;
        case 2:
            if (cargarPartida(partida)) {
                printf("Partida cargada. Reanudando...\n");
                printf("Presiona ENTER para continuar...");
                getchar();
                if (partida->puntosHP_actual <= 0 || partida->resultadoPartida == 1) {
                    printf("La partida guardada ya habia terminado.\n");
                    printf("Presiona ENTER...");
                    getchar();
                } else {
                    menuFueraDeCombate(partida, bestiario, total_enemigos, catalogo, total_objetos);
                }
            } else {
                printf("Presiona ENTER para volver...\n");
                getchar();
            }
            break;
        case 3:
            printf("\n--- ULTIMAS 5 VICTORIAS ---\n");
            FILE *archivo = fopen("highscore.txt", "r");
            if (archivo == NULL) {
                printf("Aun no hay puntuaciones registradas.\n");
            } else {
                char ultimasVictorias[5][300];
                int totalVictorias = 0;
                char buffer[300];
                int i;

                while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
                    if (strstr(buffer, "Victoria") != NULL) {
                        if (totalVictorias < 5) {
                            strcpy(ultimasVictorias[totalVictorias], buffer);
                            totalVictorias++;
                        } else {
                            for (i = 0; i < 4; i++) strcpy(ultimasVictorias[i], ultimasVictorias[i + 1]);
                            strcpy(ultimasVictorias[4], buffer);
                        }
                    }
                }
                fclose(archivo);

                if (totalVictorias == 0) {
                    printf("Aun no se han registrado victorias.\n");
                } else {
                    for (i = 0; i < totalVictorias; i++) printf("%s", ultimasVictorias[i]);
                }
            }
            break;
        case 0:
            printf("Saliendo...\n");
            break;
        default:
            printf("Opcion invalida. Por favor, ingresa un numero valido.\n");
            printf("Presiona ENTER para continuar...");
            getchar();
            break;
        }
    } while (opc != 0); 
    return opc;
}

void menuFueraDeCombate(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos) {
    int opc;
    do { 
        limpiarPantalla();
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

        printf("Seleccione una accion, explorador: ");
        if (scanf("%d", &opc) != 1) opc = -1;
        limpiarBuffer();

        if (opc < 0) {
            printf("\nEntrada no permitida. No se aceptan numeros negativos.\n");
            printf("Presiona ENTER para continuar...");
            getchar();
            continue; 
        }

        switch (opc) {
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
        case 0:
            printf("Saliendo...\n");
            break;
        default:
            printf("Opcion invalida. Intenta nuevamente.\n");
            break;
        }

        if (partida->puntosHP_actual <= 0 || partida->resultadoPartida == 1) opc = 0; 

    } while (opc != 0);
}

void iniciarCombate(EstadoJuego *estado, Entidad *enemigo) { 
    int opc;
    int huidaExitosa = 0;
    int turnoJugadorConsumido = 0;
      
    do { 
        turnoJugadorConsumido = 0;
        limpiarPantalla();
        printf("\n");
        printf("=========================================\n");
        printf("!          SISTEMA DE COMBATE           !\n");
        printf("=========================================\n");
        printf(" OBJETIVO: %-15s [HP: %03d] \n", enemigo->nombreEnemigo, enemigo->vidaBase);
        printf("-----------------------------------------\n");
        printf(" TU:       %-15s [HP: %03d/%03d]\n", "Explorador", estado->puntosHP_actual, estado->puntosHP_maximo);
        printf("=========================================\n");
        printf("  [1] Atacar objetivo\n");
        printf("  [2] Abrir mochila\n");
        printf("  [3] Intentar huir\n");
        printf("  [0] Saliendo\n");
        printf("=========================================\n");
        
        printf("Ejecutar accion: ");
        if (scanf("%d", &opc) != 1) opc = -1;
        limpiarBuffer();
        
        if (opc < 0) {
            printf("\nEntrada no permitida. No se aceptan numeros negativos.\n");
            printf("Presiona ENTER para continuar...");
            getchar();
            continue; 
        }

        switch (opc) {
        case 1:
            printf("Atacar objetivo\n");
            int dano = calcularDano(estado->atqBase, enemigo->defensaBase);
            enemigo->vidaBase -= dano;
            turnoJugadorConsumido = 1;
            break;
        case 2:
            turnoJugadorConsumido = usarObjeto(estado); 
            break;
        case 3:
            printf("\nIntentando huir...\n");
            int probHuida = 50 + (estado->defBase - enemigo->danoBase) * 2;
            
            if(probHuida > 90) probHuida = 90;
            if(probHuida < 10) probHuida = 10;

            if (rand() % 100 < probHuida) { 
                printf("Huida exitosa... Lograste escapar (Prob: %d%%).\n", probHuida);
                huidaExitosa = 1;
                estado->numCuartoActual--; // Retroceder el cuarto para no saltarse la progresión
            } else {
                printf("Fallaste al intentar huir... (Prob: %d%%)\n", probHuida);
                turnoJugadorConsumido = 1;
            }
            break;
        case 0:
            printf("Saliendo...\n"); 
            printf("Presiona ENTER...");
            getchar();
            break;
        default:
            printf("\nOpcion invalida. Pierdes la concentracion y tu turno.\n");
            turnoJugadorConsumido = 1;
            break;
        }

        if (turnoJugadorConsumido && enemigo->vidaBase > 0 && !huidaExitosa) {
            printf("\nEs el turno del... %s!\n", enemigo->nombreEnemigo);
            int danoEnemigo = calcularDano(enemigo->danoBase, estado->defBase);
            estado->puntosHP_actual -= danoEnemigo;
            printf("Recibes %d de dano.\n", danoEnemigo);
            
            printf("Presiona ENTER para continuar...");
            getchar();
        }

    } while (estado->puntosHP_actual > 0 && enemigo->vidaBase > 0 && !huidaExitosa);
}

Entidad seleccionarEnemigo(Entidad bestiario[], int total_enemigos, int es_cuarto_final) { 
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

    return posibles[rand() % cantidad_posibles];
}

void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos) {
    int cuartos_del_piso;
    int es_cuarto_final;
    int reintentar_jefe = 0; 
    Entidad enemigoActual;

    if (estado->puntosHP_actual <= 0) {
        printf("\nHas caido en batalla...\n");
        registrarHighscore(*estado, 0);
        gestionarPartidaGuardada();
        return;
    }

    if (estado->resultadoPartida == 1) {
        printf("La partida ya fue completada.\n");
        return;
    }

    if (estado->totalPisosMax <= 0 || estado->cuartosBasePorPiso <= 0) {
        printf("Configuracion de pisos o cuartos invalida.\n");
        return;
    }

    if (estado->numPiso == estado->totalPisosMax) {
        cuartos_del_piso = estado->cuartosBasePorPiso;
        if (estado->totalPisosMax > 1) cuartos_del_piso++;
    } else if (estado->numPiso == 1) {
        cuartos_del_piso = estado->cuartosBasePorPiso - 1;
        if (cuartos_del_piso < 1) cuartos_del_piso = 1;
    } else {
        cuartos_del_piso = estado->cuartosBasePorPiso;
    }

    if (estado->numPiso == estado->totalPisosMax && estado->numCuartoActual >= cuartos_del_piso) {
        reintentar_jefe = 1;
        printf("\nEl jefe final sigue vivo. Debes volver a enfrentarlo.\n");
    } else if (estado->numCuartoActual < cuartos_del_piso) {
        estado->numCuartoActual++;
    } else {
        if (estado->numPiso >= estado->totalPisosMax) {
            printf("Ya estas en el ultimo cuarto del dungeon.\n");
            return;
        }
        estado->numPiso++;
        estado->numCuartoActual = 1;
        printf("\n--- Has limpiado el piso! Desciendes al piso %d ---\n", estado->numPiso);
    }

    if (estado->numPiso == estado->totalPisosMax) {
        cuartos_del_piso = estado->cuartosBasePorPiso;
        if (estado->totalPisosMax > 1) cuartos_del_piso++;
    } else if (estado->numPiso == 1) {
        cuartos_del_piso = estado->cuartosBasePorPiso - 1;
        if (cuartos_del_piso < 1) cuartos_del_piso = 1;
    } else {
        cuartos_del_piso = estado->cuartosBasePorPiso;
    }

    if (!reintentar_jefe) estado->cuartosExplorados++; 
    es_cuarto_final = (estado->numCuartoActual == cuartos_del_piso);
    if (es_cuarto_final) printf("\nCuidado! Una presencia imponente te aguarda...\n");

    enemigoActual = seleccionarEnemigo(bestiario, total_enemigos, es_cuarto_final);
    printf("\nTe adentras en el cuarto %d y aparece un %s.\n", estado->numCuartoActual, enemigoActual.nombreEnemigo);
    
    printf("Presiona ENTER para continuar...");
    getchar();

    iniciarCombate(estado, &enemigoActual);

    if (estado->puntosHP_actual <= 0) {
        printf("\nHas caido en batalla...\n");
        registrarHighscore(*estado, 0);
        gestionarPartidaGuardada();
        return;
    }

    if (enemigoActual.vidaBase <= 0) {
        printf("\nHas derrotado al %s!\n", enemigoActual.nombreEnemigo);
        estado->totalEnemigosDerrotados++;
        estado->puntosXP += 50;
        otorgarRecompensa(estado, catalogo, total_objetos, enemigoActual.tipo);
        verificarSubidaNivel(estado);

        if (es_cuarto_final && estado->numPiso == estado->totalPisosMax) {
            printf("\n=== HAS CONQUISTADO EL NUCLEO! ===\n");
            estado->resultadoPartida = 1;
            registrarHighscore(*estado, 1);
            gestionarPartidaGuardada();
            return;
        }
        
        printf("Presiona ENTER para continuar...");
        getchar();
    }
}

void gestionarPartidaGuardada() {
    if (remove("partida_guardada.txt") == 0) {
        printf("Partida anterior eliminada del sistema.\n");
    }
}

void guardarPartida(EstadoJuego estado) {
    FILE *archivo = fopen("partida_guardada.txt", "w"); 
    
    if (archivo == NULL) {
        printf("[Error] No se pudo crear el archivo de guardado.\n");
        return;
    }

    fprintf(archivo, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        estado.semilla,
        estado.puntosHP_actual, estado.puntosHP_maximo,
        estado.atqBase, estado.defBase,
        estado.puntosXP, estado.nivelActual,
        estado.numPiso, estado.numCuartoActual,
        estado.tamanoInventarioMax, estado.totalPisosMax,
        estado.cuartosBasePorPiso, estado.totalEnemigosDerrotados,
        estado.cuartosExplorados, estado.objetosConsumidos, estado.resultadoPartida);

    fprintf(archivo, "%d\n", estado.cantidadObjetosActual);

    for (int i = 0; i < estado.cantidadObjetosActual; i++) {
        fprintf(archivo, "%d \"%s\" %d %s\n", 
            estado.inventarioJugador[i].id_Objeto,
            estado.inventarioJugador[i].nombreObjeto,
            estado.inventarioJugador[i].aumentoStats,
            estado.inventarioJugador[i].tipoEfecto);
    }

    fclose(archivo);
    printf("[Sistema] Partida guardada con exito en modo texto.\n");
}

int cargarPartida(EstadoJuego *estado) {
    FILE *archivo = fopen("partida_guardada.txt", "r");
    int resultadoTemporal, leidos, i;

    if (archivo == NULL) {
        printf("[Sistema] No se encontro una partida previa.\n");
        return 0;
    }

    leidos = fscanf(archivo, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        &estado->semilla,
        &estado->puntosHP_actual, &estado->puntosHP_maximo,
        &estado->atqBase, &estado->defBase,
        &estado->puntosXP, &estado->nivelActual,
        &estado->numPiso, &estado->numCuartoActual,
        &estado->tamanoInventarioMax, &estado->totalPisosMax,
        &estado->cuartosBasePorPiso, &estado->totalEnemigosDerrotados,
        &estado->cuartosExplorados, &estado->objetosConsumidos, &resultadoTemporal);

    if (leidos != 16 || estado->tamanoInventarioMax < 1 || estado->totalPisosMax < 1 ||
        estado->cuartosBasePorPiso < 1 || estado->puntosHP_maximo < 1 || estado->nivelActual < 1 ||
        estado->numPiso < 1 || estado->numPiso > estado->totalPisosMax || estado->numCuartoActual < 0 || estado->semilla < 0) {
        printf("[Error] El archivo de guardado esta incompleto o contiene datos invalidos.\n");
        fclose(archivo);
        return 0;
    }

    estado->resultadoPartida = resultadoTemporal;
    if (fscanf(archivo, "%d", &estado->cantidadObjetosActual) != 1 ||
        estado->cantidadObjetosActual < 0 || estado->cantidadObjetosActual > estado->tamanoInventarioMax) {
        printf("[Error] Cantidad de objetos invalida en el guardado.\n");
        fclose(archivo);
        return 0;
    }

    for (i = 0; i < estado->cantidadObjetosActual; i++) {
        if (fscanf(archivo, "%d \"%49[^\"]\" %d %49s", 
            &estado->inventarioJugador[i].id_Objeto,
            estado->inventarioJugador[i].nombreObjeto,
            &estado->inventarioJugador[i].aumentoStats,
            estado->inventarioJugador[i].tipoEfecto) != 4 || estado->inventarioJugador[i].id_Objeto < 0) {
            printf("[Error] Registro de objeto invalido en el guardado.\n");
            fclose(archivo);
            estado->cantidadObjetosActual = 0;
            return 0;
        }
    }

    fclose(archivo);
    printf("[Sistema] Partida cargada exitosamente.\n");
    srand(estado->semilla);
    return 1;
}

int usarObjeto(EstadoJuego *estado) {
    if (estado->cantidadObjetosActual == 0) {
        printf("\nTu mochila esta vacia.\n");
        return 0;
    }

    printf("\n--- INVENTARIO ---\n");
    for (int i = 0; i < estado->cantidadObjetosActual; i++) {
        printf("[%d] %s (Efecto: %s | Poder: %d)\n", i + 1,
               estado->inventarioJugador[i].nombreObjeto,
               estado->inventarioJugador[i].tipoEfecto,
               estado->inventarioJugador[i].aumentoStats);
    }
    printf("[0] Cancelar\n");
    printf("Elige un objeto para usar: ");

    int opc = -1;
    if (scanf("%d", &opc) != 1) opc = -1;
    limpiarBuffer();

    if (opc <= 0 || opc > estado->cantidadObjetosActual) {
        printf("\nCancelado.\n");
        return 0;
    }

    int indice = opc - 1;
    Objeto obj = estado->inventarioJugador[indice];

    printf("\nHas seleccionado: %s\n", obj.nombreObjeto);
    printf("[1] Usar objeto\n");
    printf("[2] Soltar objeto\n");
    printf("[0] Cancelar\n");
    printf("Elige una opcion: ");

    int accion = -1;
    if (scanf("%d", &accion) != 1) accion = -1;
    limpiarBuffer();

    if (accion == 0 || accion < 0) {
        printf("\nCancelado.\n");
        return 0;
    }

    if (accion == 1) {
        int efectoAplicado = 1;

        if (strcmp(obj.tipoEfecto, "Cura_Max") == 0) {
            estado->puntosHP_actual = estado->puntosHP_maximo;
            printf("\nRestauraste toda tu salud al maximo.\n");
        }
        else if (strcmp(obj.tipoEfecto, "Cura_Vida") == 0) {
            estado->puntosHP_actual += obj.aumentoStats;
            if (estado->puntosHP_actual > estado->puntosHP_maximo) {
                estado->puntosHP_actual = estado->puntosHP_maximo;
            }
            printf("\nRecuperaste %d HP.\n", obj.aumentoStats);
        }
        else if (strcmp(obj.tipoEfecto, "Buff_Dano") == 0) {
            estado->atqBase += obj.aumentoStats;
            printf("\nTu ataque aumento en %d.\n", obj.aumentoStats);
        }
        else if (strcmp(obj.tipoEfecto, "Buff_Defensa") == 0) {
            estado->defBase += obj.aumentoStats;
            printf("\nTu defensa aumento en %d.\n", obj.aumentoStats);
        }
        else {
            printf("\nEste objeto no tiene un efecto conocido. No se consumio.\n");
            efectoAplicado = 0;
        }

        if (efectoAplicado) {
            estado->objetosConsumidos++;
            for (int i = indice; i < estado->cantidadObjetosActual - 1; i++) {
                estado->inventarioJugador[i] = estado->inventarioJugador[i + 1];
            }
            estado->cantidadObjetosActual--;
        }
        return 1;
    }

    if (accion == 2) {
        printf("\nHas soltado %s.\n", obj.nombreObjeto);
        for (int i = indice; i < estado->cantidadObjetosActual - 1; i++) {
            estado->inventarioJugador[i] = estado->inventarioJugador[i + 1];
        }
        estado->cantidadObjetosActual--;
        return 1; 
    }

    printf("\nOpcion invalida.\n");
    return 0;
}

int calcularDano(int ataque_atacante, int defensa_defensor) {
    int dano = ataque_atacante - defensa_defensor;
    if (dano < 1) dano = 1;
    return dano;
}

void cargarConfiguracion(EstadoJuego *estado) {
    FILE *archivo = fopen("config.txt", "r");
    int leidos;

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo config.txt\n");
        exit(1);
    }

    leidos = fscanf(archivo, "%d %d %d %d %d %d %d",
        &estado->semilla, &estado->totalPisosMax,
        &estado->cuartosBasePorPiso, &estado->puntosHP_maximo,
        &estado->atqBase, &estado->defBase, &estado->tamanoInventarioMax);

    if (leidos != 7 || estado->semilla < 0 || estado->totalPisosMax < 1 || estado->cuartosBasePorPiso < 1 || 
        estado->tamanoInventarioMax < 1 || estado->tamanoInventarioMax > 100 ||
        estado->puntosHP_maximo < 1 || estado->atqBase < 0 || estado->defBase < 0) {
        printf("Error: config.txt esta incompleto o contiene valores invalidos.\n");
        fclose(archivo);
        exit(1);
    }

    estado->puntosHP_actual = estado->puntosHP_maximo;
    fclose(archivo);
}

void cargarBestiario(Entidad bestiario[], int *total_enemigos) {
    FILE *archivo = fopen("bestiario.txt", "r");
    int id, vida, dano, defensa;
    char nombre[50], tipo[20];

    if (archivo == NULL) {
        printf("Error: No se pudo abrir bestiario.txt\n");
        exit(1);
    }

    *total_enemigos = 0;
    while (fscanf(archivo, "%d %49[a-zA-Z_ ] %d %d %d %19s", &id, nombre, &vida, &dano, &defensa, tipo) == 6) {
        if (*total_enemigos >= 100 || id < 0 || vida < 1 || dano < 0 || defensa < 0) {
            printf("Error: Registro invalido o demasiados enemigos en bestiario.txt\n");
            fclose(archivo);
            exit(1);
        }
        bestiario[*total_enemigos].id_Enemigo = id;
        strcpy(bestiario[*total_enemigos].nombreEnemigo, nombre);
        bestiario[*total_enemigos].vidaBase = vida;
        bestiario[*total_enemigos].danoBase = dano;
        bestiario[*total_enemigos].defensaBase = defensa;
        strcpy(bestiario[*total_enemigos].tipo, tipo);
        (*total_enemigos)++;
    }

    if (*total_enemigos == 0) {
        printf("Error: bestiario.txt esta incompleto o contiene valores invalidos.\n");
        fclose(archivo);
        exit(1);
    }

    fclose(archivo);
}

void cargarObjetos(Objeto catalogo[], int *total_objetos) {
    FILE *archivo = fopen("objetos.txt", "r");
    int id, aumento; 
    char nombre[50], efecto[50];

    if (archivo == NULL) {
        printf("Error: No se pudo abrir objetos.txt\n");
        exit(1);
    }

    *total_objetos = 0;
    while (fscanf(archivo, "%d %49[a-zA-Z_ ] %d %49s", &id, nombre, &aumento, efecto) == 4) {
        if (*total_objetos >= 100 || id < 0 || aumento < 0) {
            printf("Error: objetos.txt supera el maximo o contiene valores invalidos/negativos.\n");
            fclose(archivo);
            exit(1);
        }
        
        if (strcmp(efecto, "Cura_Vida") != 0 && strcmp(efecto, "Cura_Max") != 0 &&
            strcmp(efecto, "Buff_Dano") != 0 && strcmp(efecto, "Buff_Defensa") != 0) {
            printf("Error: El objeto '%s' tiene un efecto desconocido ('%s').\n", nombre, efecto);
            fclose(archivo);
            exit(1);
        }

        catalogo[*total_objetos].id_Objeto = id;
        strcpy(catalogo[*total_objetos].nombreObjeto, nombre);
        catalogo[*total_objetos].aumentoStats = aumento;
        strcpy(catalogo[*total_objetos].tipoEfecto, efecto);
        (*total_objetos)++;
    }

    if (*total_objetos == 0) {
        printf("Error: objetos.txt esta incompleto o contiene valores invalidos.\n");
        fclose(archivo);
        exit(1);
    }

    fclose(archivo);
}

void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], int total_objetos, char tipo_enemigo[]) {
    int probabilidad = (strcmp(tipo_enemigo, "Raro") == 0) ? 100 : 30;

    if (total_objetos <= 0) return;

    if ((rand() % 100) < probabilidad) {
        int objAleatorio = rand() % total_objetos;
        Objeto objObtenido = catalogo[objAleatorio];

        if (estado->cantidadObjetosActual < estado->tamanoInventarioMax) {
            estado->inventarioJugador[estado->cantidadObjetosActual] = objObtenido;
            estado->cantidadObjetosActual++;
            printf("\nEl enemigo dejo caer un objeto! Obtuviste: %s\n", objObtenido.nombreObjeto);
        } else {
            printf("\nEl enemigo solto %s, pero tu inventario esta lleno.\n", objObtenido.nombreObjeto);
        }
    }
}

void verificarSubidaNivel(EstadoJuego *estado) {
    int umbral = estado->nivelActual * 100; 
    
    while (estado->puntosXP >= umbral) { 
        estado->nivelActual++;
        estado->puntosXP -= umbral; 
        estado->puntosHP_maximo += 20;
        estado->puntosHP_actual = estado->puntosHP_maximo; 
        estado->atqBase += 5;
        estado->defBase += 5;
        printf("\n*** SUBISTE DE NIVEL! Ahora eres nivel %d ***\n", estado->nivelActual);
        umbral = estado->nivelActual * 100;
    }
}

void registrarHighscore(EstadoJuego estado, int victoria) {
    char nombre[50];
    FILE *archivo;

    printf("\nIntroduce tu nombre o tag para el registro de Highscores: ");
    fflush(stdout);
    
    if (fgets(nombre, sizeof(nombre), stdin) != NULL) {
        if (nombre[0] == '\n') { 
            fgets(nombre, sizeof(nombre), stdin); 
        }
        nombre[strcspn(nombre, "\r\n")] = '\0';
    }

    if (strlen(nombre) == 0) {
        strcpy(nombre, "Explorador_Anonimo");
    }

    archivo = fopen("highscore.txt", "a");
    if (archivo == NULL) {
        printf("\nError al abrir highscore.txt.\n");
        return;
    }

    fprintf(archivo,
        "Jugador: %-15s | Resultado: %-8s | Piso: %02d | Cuartos explorados: %d | "
        "Enemigos derrotados: %d | Objetos consumidos: %d | Nivel: %02d | "
        "HP: %d/%d | Ataque: %d | Defensa: %d | XP: %d\n",
        nombre, victoria ? "Victoria" : "Derrota", estado.numPiso,
        estado.cuartosExplorados, estado.totalEnemigosDerrotados,
        estado.objetosConsumidos, estado.nivelActual,
        estado.puntosHP_actual, estado.puntosHP_maximo,
        estado.atqBase, estado.defBase, estado.puntosXP);

    fclose(archivo);
    printf("\nPuntuacion registrada correctamente.\n");
}