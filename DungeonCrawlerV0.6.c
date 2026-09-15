#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
int mostrarMenuInicio(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos);
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
    mostrarMenuInicio(&partida, bestiario, total_enemigos, catalogo, total_objetos);

    return 0;
}

// ==========================================
// DEFINICIÓN DE FUNCIONES
// ==========================================

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int mostrarMenuInicio(EstadoJuego *partida, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos){
    int opc;
    do{
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
        scanf("%d", &opc);

        limpiarBuffer();

        switch (opc){
        case 1:
            printf("Iniciando partida...\n");
            menuFueraDeCombate(partida, bestiario, total_enemigos, catalogo, total_objetos);
            break;
        case 2:
            printf("Cargando partida...\n");
            cargarPartida(partida); 
            break;
        case 3:
            printf("\n--- ULTIMAS 5 VICTORIAS ---\n");
            FILE *archivo = fopen("highscore.txt", "r");
            if (archivo == NULL) {
                printf("Aún no hay puntuaciones registradas.\n");
            } else {
                char lineasVictorias[100][200]; // Arreglo para guardar las lineas de victoria temporalmente
                int totalVictorias = 0;
                char buffer[200];
                
                // Leer línea por línea
                while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
                    // Filtrar solo las líneas que contienen la palabra "Victoria"
                    if (strstr(buffer, "Victoria") != NULL) {
                        strcpy(lineasVictorias[totalVictorias], buffer);
                        totalVictorias++;
                    }
                }
                fclose(archivo);
                
                if (totalVictorias == 0) {
                    printf("Aún no se han registrado victorias.\n");
                } else {
                    // Calculamos el inicio para mostrar solo las últimas 5
                    int inicio = (totalVictorias >= 5) ? totalVictorias - 5 : 0;
                    for (int i = inicio; i < totalVictorias; i++) {
                        printf("%s", lineasVictorias[i]);
                    }
                }
            }
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

        // Validar si la partida terminó para sacarlo del menú de exploración
        if (partida->puntosHP_actual <= 0 || partida->resultadoPartida == true) {
            opc = 0; 
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
            int dano = calcularDano(estado->atqBase, enemigo->defensaBase);
            enemigo->vidaBase -= dano;
            turnoJugadorConsumido = true;
            break;
        case 2:
            usarObjeto(estado);
            turnoJugadorConsumido = true;
            break;
        case 3:
            printf("\nIntentando huir...\n");
            // Cálculo: Base de 50% + (Defensa Jugador - Daño Enemigo) * 2
            int probHuida = 50 + (estado->defBase - enemigo->danoBase) * 2;
            
            // Limitamos para que nunca sea 100% segura ni 0% posible
            if (probHuida > 90) probHuida = 90;
            if (probHuida < 10) probHuida = 10;

            if(rand() % 100 < probHuida){ 
                printf("¡Huida exitosa! Lograste escapar (Prob: %d%%).\n", probHuida);
                huidaExitosa = true;
            }else{
                printf("¡Fallaste al intentar huir! (Prob: %d%%)\n", probHuida);
                turnoJugadorConsumido = true;
            }
            break;
        default:
            printf("Saliendo...\n"); 
            break;
        }

        if(turnoJugadorConsumido && enemigo->vidaBase > 0 && !huidaExitosa){
            printf("\n¡Es el turno del %s!\n", enemigo->nombreEnemigo);
            int danoEnemigo = calcularDano(enemigo->danoBase, estado->defBase);
            estado->puntosHP_actual -= danoEnemigo;
            printf("Recibes %d de daño.\n", danoEnemigo);
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

void avanzarCuarto(EstadoJuego *estado, Entidad bestiario[], int total_enemigos, Objeto catalogo[], int total_objetos) { 
    int cuartos_del_piso = estado->cuartosBasePorPiso;
    
    if (estado->numPiso == 1) {
        cuartos_del_piso -= 1; 
    } else if (estado->numPiso == estado->totalPisosMax) {
        cuartos_del_piso += 1; 
    }

   // Parte 1: Bloque de Victoria (dentro del inicio de la función avanzarCuarto)
    if (estado->numCuartoActual < cuartos_del_piso) {
        estado->numCuartoActual++;
    } else {
        if (estado->numPiso < estado->totalPisosMax) {
            estado->numPiso++;
            estado->numCuartoActual = 1; 
            printf("\n--- ¡Has limpiado el piso! Desciendes al piso %d ---\n", estado->numPiso);
        } else {
            printf("\n¡Felicidades! Has superado el ultimo cuarto y conquistado el nucleo.\n");
            estado->resultadoPartida = true; 
            registrarHighscore(*estado, true); 
            gestionarPartidaGuardada(); // <--- Llama la función aquí
            return; 
        }
    }

    // ... (Selección de enemigo e iniciar combate) ...

    // Parte 2: Bloque de Derrota (al final de la función avanzarCuarto)
    if (estado->puntosHP_actual <= 0) {
        printf("\nHas caído en batalla y tu aventura termina aquí...\n");
        registrarHighscore(*estado, false);
        gestionarPartidaGuardada(); // <--- Llama la función aquí también
        return; 
    }

    int es_cuarto_final = 0;
    if (estado->numCuartoActual == cuartos_del_piso) {
        es_cuarto_final = 1;
        printf("\n¡Cuidado! Has entrado al ultimo cuarto de este piso. Una presencia imponente te aguarda...\n");
    }

    // 1. Primero seleccionamos al enemigo
    Entidad enemigoActual = seleccionarEnemigo(bestiario, total_enemigos, es_cuarto_final);
    
    printf("\nTe adentras en el cuarto %d y te encuentras con un %s.\n", estado->numCuartoActual, enemigoActual.nombreEnemigo);
    
    // ==========================================
    // AQUÍ EMPIEZA EL CÓDIGO NUEVO DE INTEGRACIÓN
    // ==========================================

    // 2. Luego iniciamos el combate contra ese enemigo
    iniciarCombate(estado, &enemigoActual);

    // 3. Finalmente, evaluamos qué pasó en el combate
    if (estado->puntosHP_actual <= 0) {
        // Si el jugador muere
        printf("\nHas caido en batalla y tu aventura termina aqui...\n");
        registrarHighscore(*estado, false); // Registramos la derrota
        return; 
    } else if (enemigoActual.vidaBase <= 0) { 
        // Si el enemigo muere (no cuenta si el jugador huyó)
        printf("\n¡Has derrotado al %s!\n", enemigoActual.nombreEnemigo);
        estado->totalEnemigosDerrotados++;
        estado->puntosXP += 50; // Sumamos la XP base
        
        // Invocamos la recompensa (esto elimina la línea amarilla de advertencia)
        otorgarRecompensa(estado, catalogo, total_objetos, enemigoActual.tipo);
        
        // Revisamos si la XP obtenida es suficiente para subir de nivel
        verificarSubidaNivel(estado);
    }
}

void gestionarPartidaGuardada() {
    if (remove("partida_guardada.txt") == 0) {
        printf("Partida anterior eliminada del sistema.\n");
    }
}

void guardarPartida(EstadoJuego estado) {
    // Abrimos en modo texto plano para escribir ("w")
    FILE *archivo = fopen("partida_guardada.txt", "w"); 
    
    if (archivo == NULL) {
        printf("[Error] No se pudo crear el archivo de guardado.\n");
        return;
    }

    // 1. Guardamos todas las variables de tipo entero y booleano (casteado a int)
    fprintf(archivo, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        estado.semilla,
        estado.puntosHP_actual, estado.puntosHP_maximo,
        estado.atqBase, estado.defBase,
        estado.puntosXP, estado.nivelActual,
        estado.numPiso, estado.numCuartoActual,
        estado.tamanoInventarioMax, estado.totalPisosMax,
        estado.cuartosBasePorPiso, estado.totalEnemigosDerrotados,
        estado.objetosConsumidos, (int)estado.resultadoPartida
    );

    // 2. Guardamos la cantidad exacta de objetos que hay en el inventario
    fprintf(archivo, "%d\n", estado.cantidadObjetosActual);

    // 3. Recorremos el arreglo de inventario y guardamos los datos de cada objeto
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
    // Abrimos en modo texto plano para leer ("r")
    FILE *archivo = fopen("partida_guardada.txt", "r"); 
    
    if (archivo == NULL) {
        printf("[Sistema] No se encontró una partida previa.\n");
        return 0;
    }

    int resultadoTemporal;

    // 1. Leemos las variables base (usando & porque estamos modificando sus valores)
    fscanf(archivo, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        &estado->semilla,
        &estado->puntosHP_actual, &estado->puntosHP_maximo,
        &estado->atqBase, &estado->defBase,
        &estado->puntosXP, &estado->nivelActual,
        &estado->numPiso, &estado->numCuartoActual,
        &estado->tamanoInventarioMax, &estado->totalPisosMax,
        &estado->cuartosBasePorPiso, &estado->totalEnemigosDerrotados,
        &estado->objetosConsumidos, &resultadoTemporal
    );

    // Restauramos el booleano
    estado->resultadoPartida = (bool)resultadoTemporal;

    // 2. Leemos la cantidad de objetos que debemos cargar al inventario
    fscanf(archivo, "%d", &estado->cantidadObjetosActual);

    // 3. Iteramos para reconstruir el inventario objeto por objeto
    for (int i = 0; i < estado->cantidadObjetosActual; i++) {
        // Nota: Los strings (como id_Objeto) no llevan '&' en fscanf porque los arreglos ya son punteros en C
        fscanf(archivo, "%s %s %d %s",
            estado->inventarioJugador[i].id_Objeto,
            estado->inventarioJugador[i].nombreObjeto,
            &estado->inventarioJugador[i].aumentoStats,
            estado->inventarioJugador[i].tipoEfecto);
    }

    fclose(archivo);
    printf("[Sistema] Partida cargada exitosamente.\n");
    return 1;
}

void usarObjeto(EstadoJuego *estado) {
    if (estado->cantidadObjetosActual == 0) {
        printf("\nTu mochila esta vacia.\n");
        return;
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
    
    int opc;
    scanf("%d", &opc);
    limpiarBuffer();

    if (opc > 0 && opc <= estado->cantidadObjetosActual) {
        int indice = opc - 1;
        Objeto obj = estado->inventarioJugador[indice];

        printf("\nHas seleccionado: %s\n", obj.nombreObjeto);
        printf("[1] Usar objeto\n");
        printf("[2] Soltar objeto\n");
        printf("[0] Cancelar\n");
        printf("Elige una opcion: ");
        
        int accion;
        scanf("%d", &accion);
        limpiarBuffer();

        if (accion == 1) { // Usar objeto
            if (strcmp(obj.tipoEfecto, "Cura_Vida") == 0 || strcmp(obj.tipoEfecto, "Cura_Max") == 0) {
                estado->puntosHP_actual += obj.aumentoStats;
                if (estado->puntosHP_actual > estado->puntosHP_maximo) 
                    estado->puntosHP_actual = estado->puntosHP_maximo;
            } else if (strcmp(obj.tipoEfecto, "Buff_Daño") == 0 || strcmp(obj.tipoEfecto, "Buff_Dano") == 0) {
                estado->atqBase += obj.aumentoStats;
            } else if (strcmp(obj.tipoEfecto, "Buff_Defensa") == 0) {
                estado->defBase += obj.aumentoStats;
            }
            
            estado->objetosConsumidos++;
            printf("\nConsumiste %s.\n", obj.nombreObjeto);
        } else if (accion == 2) { // Soltar objeto
            printf("\nHas soltado %s.\n", obj.nombreObjeto);
        }

        // Si se usó (1) o se soltó (2), retiramos el objeto del inventario desplazando el arreglo
        if (accion == 1 || accion == 2) {
            for (int i = indice; i < estado->cantidadObjetosActual - 1; i++) {
                estado->inventarioJugador[i] = estado->inventarioJugador[i+1];
            }
            estado->cantidadObjetosActual--;
        }
    }
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
    int probabilidad = (strcmp(tipo_enemigo, "Raro") == 0) ? 100 : 30;

    if ((rand() % 100) < probabilidad) {
        if (estado->cantidadObjetosActual < estado->tamanoInventarioMax) {
            int objAleatorio = rand() % total_objetos;
            estado->inventarioJugador[estado->cantidadObjetosActual] = catalogo[objAleatorio];
            estado->cantidadObjetosActual++;
            printf("\n¡El enemigo dejó caer un objeto! Obtuviste: %s\n", catalogo[objAleatorio].nombreObjeto);
        } else {
            printf("\nEl enemigo soltó un objeto, pero tu inventario está lleno.\n");
        }
    }
}

void verificarSubidaNivel(EstadoJuego *estado) {
    int umbral = estado->nivelActual * 100; 
    
    if (estado->puntosXP >= umbral) {
        estado->nivelActual++;
        estado->puntosXP -= umbral; // Restamos la XP usada, conservando el sobrante
        estado->puntosHP_maximo += 20;
        estado->puntosHP_actual = estado->puntosHP_maximo; // Curar al 100%
        estado->atqBase += 5;
        estado->defBase += 5;
        printf("\n*** ¡SUBISTE DE NIVEL! Ahora eres nivel %d ***\n", estado->nivelActual);
        printf("Estadísticas aumentadas y salud restaurada al máximo.\n");
    }
}

void registrarHighscore(EstadoJuego estado, bool victoria) {
    char nombre[50];
    printf("\nIntroduce tu nombre o tag para el registro de Highscores: ");
    scanf("%49s", nombre);
    limpiarBuffer();

    FILE *archivo = fopen("highscore.txt", "a");
    if (archivo != NULL) {
        fprintf(archivo, "Jugador: %-15s | Resultado: %-8s | Nivel: %02d | XP: %04d | Piso: %02d | Enemigos Derrotados: %d\n",
                nombre,
                victoria ? "Victoria" : "Derrota",
                estado.nivelActual,
                estado.puntosXP,
                estado.numPiso,
                estado.totalEnemigosDerrotados);
        fclose(archivo);
        printf("[Sistema] Tu puntuacion ha sido registrada en highscore.txt\n");
    } else {
        printf("[Error] No se pudo escribir el archivo de Highscores.\n");
    }
}