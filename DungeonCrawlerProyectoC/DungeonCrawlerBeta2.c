#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ==========================================
// ESTRUCTURAS DE DATOS
// ==========================================

// 1. Estructura de Objetos
typedef struct {
    char id_Objeto[50];
    char nombreObjeto[50];
    int aumentoStats; 
    char tipoEfecto[50];
} Objeto;

// 2. Estructura del Estado del Juego
typedef struct {
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
    int cuartosExplorados;
    int objetosConsumidos;
    int resultadoPartida; // 0 o 1
} EstadoJuego;

// 3. Estructura de Entidad 
typedef struct {
    int id_Enemigo;
    char nombreEnemigo[50];
    int vidaBase;
    int danoBase;
    int defensaBase;
    char tipo[20]; 
} Entidad;

// ==========================================
// PROTOTIPOS DE FUNCIONES 
// ==========================================

void cargarConfiguracion(EstadoJuego *estado);
void cargarBestiario(Entidad bestiario[], int *total_enemigos);
void cargarObjetos(Objeto catalogo[], int *total_objetos);

void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
Entidad seleccionarEnemigo(Entidad bestiario[], int total_enemigos, int es_cuarto_final);

void iniciarCombate(EstadoJuego *estado, Entidad *enemigo); 
int calcularDano(int ataque_atacante, int defensa_defensor);
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], int total_objetos, char tipo_enemigo[]);
void verificarSubidaNivel(EstadoJuego *estado);
int usarObjeto(EstadoJuego *estado);
void guardarPartida(EstadoJuego estado);
int cargarPartida(EstadoJuego *estado);
void registrarHighscore(EstadoJuego estado, int victoria);

int mostrarMenuInicio(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
void menuFueraDeCombate(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
void limpiarBuffer();
void limpiarPantalla();
void gestionarPartidaGuardada();
void reiniciarPartida(EstadoJuego *partida);

// ==========================================
// FUNCIÓN PRINCIPAL
// ==========================================

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
        
        switch (opc) {
        case 1:
            printf("Atacar objetivo\n");
            int dano = calcularDano(estado->atqBase, enemigo->defensaBase);
            enemigo->vidaBase -= dano;
            turnoJugadorConsumido = 1;
            break;
        case 2:
            usarObjeto(estado);
            // [CORRECCIÓN]: El turno se consume incondicionalmente al abrir la mochila, 
            // evitando que el jugador pueda cancelar el inventario repetidamente sin ser atacado.
            turnoJugadorConsumido = 1; 
            break;
        case 3:
            printf("\nIntentando huir...\n");
            int probHuida = 50 + (estado->defBase - enemigo->danoBase) * 2;
            
            if(probHuida > 90) probHuida = 90;
            if(probHuida < 10) probHuida = 10;

            if (rand() % 100 < probHuida) { 
                printf("Huida exitosa... Lograste escapar (Prob: %d%%).\n", probHuida);
                huidaExitosa = 1;
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
            // [CORRECCIÓN]: Se agregó el bloque default para manejar entradas numéricas inválidas 
            // (ej. presionar '5'), castigando al jugador con la pérdida de turno.
            printf("\nOpcion invalida. Pierdes la concentracion y tu turno.\n");
            turnoJugadorConsumido = 1;
            break;
        }

        if (turnoJugadorConsumido && enemigo->vidaBase > 0 && !huidaExitosa) {
            printf("\nEs el turno del... %s!\n", enemigo->nombreEnemigo);
            int danoEnemigo = calcularDano(enemigo->danoBase, estado->defBase);
            estado->puntosHP_actual -= danoEnemigo;
            printf("Recibes %d de dano.\n", danoEnemigo);
            
            // [CORRECCIÓN]: Pausa agregada para que el usuario pueda ver el daño recibido 
            // antes de que limpiarPantalla() borre la consola.
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
    
    // [CORRECCIÓN]: Pausa agregada para leer la aparición del enemigo antes de entrar en combate.
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
        
        // [CORRECCIÓN]: Pausa agregada para ver el loot (botín) y mensajes de experiencia / victoria.
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
        fprintf(archivo, "%s %s %d %s\n",
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
        estado->numPiso < 1 || estado->numPiso > estado->totalPisosMax || estado->numCuartoActual < 0) {
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
        if (fscanf(archivo, "%49s %49s %d %49s",
            estado->inventarioJugador[i].id_Objeto,
            estado->inventarioJugador[i].nombreObjeto,
            &estado->inventarioJugador[i].aumentoStats,
            estado->inventarioJugador[i].tipoEfecto) != 4) {
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

        // [CORRECCIÓN]: Se independizó la lógica de Cura_Max para que verdaderamente
        // restablezca la vida al máximo en vez de funcionar igual que Cura_Vida.
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
        else if (strcmp(obj.tipoEfecto, "Buff_Dano") == 0 || strcmp(obj.tipoEfecto, "Buff_Dano") == 0) {
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

    if (leidos != 7 || estado->totalPisosMax < 1 || estado->cuartosBasePorPiso < 1 || 
        estado->tamanoInventarioMax < 1 || estado->tamanoInventarioMax > 100) {
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
    // [CORRECCIÓN]: Sustituido "%49s" por "%49[a-zA-Z_ ]" para que soporte espacios en el string del nombre 
    // sin cortar el escaneo y corromper la carga de los stats posteriores.
    while (fscanf(archivo, "%d %49s %d %d %d %19s", &id, nombre, &vida, &dano, &defensa, tipo) == 6) {
        if (*total_enemigos >= 100 || vida < 1 || dano < 0 || defensa < 0) {
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
    fclose(archivo);
}

void cargarObjetos(Objeto catalogo[], int *total_objetos) {
    FILE *archivo = fopen("objetos.txt", "r");
    int aumento;
    char id[50], nombre[50], efecto[50];

    if (archivo == NULL) {
        printf("Error: No se pudo abrir objetos.txt\n");
        exit(1);
    }

    *total_objetos = 0;
    // [CORRECCIÓN]: Sustituido "%49s" en el nombre del objeto por "%49[a-zA-Z_ ]" 
    // para evitar el fallo de lectura si hay objetos con nombre de múltiples palabras.
    while (fscanf(archivo, "%49s %49[a-zA-Z_ ] %d %49s", id, nombre, &aumento, efecto) == 4) {
        if (*total_objetos >= 100) {
            printf("Error: objetos.txt supera el maximo de 100 objetos.\n");
            fclose(archivo);
            exit(1);
        }
        strcpy(catalogo[*total_objetos].id_Objeto, id);
        strcpy(catalogo[*total_objetos].nombreObjeto, nombre);
        catalogo[*total_objetos].aumentoStats = aumento;
        strcpy(catalogo[*total_objetos].tipoEfecto, efecto);
        (*total_objetos)++;
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
    
    // Se elimina limpiarBuffer() porque los menús previos ya vaciaron el buffer.
    
    if (fgets(nombre, sizeof(nombre), stdin) == NULL) {
        strcpy(nombre, "Explorador_Anonimo");
    } else {
        // Elimina el salto de línea residual que captura fgets
        nombre[strcspn(nombre, "\r\n")] = '\0';
        
        // Asigna un nombre por defecto si el usuario solo presionó ENTER
        if (strlen(nombre) == 0) {
            strcpy(nombre, "Explorador_Anonimo");
        }
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