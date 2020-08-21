
/*
Escuela Superior Politécnica del Litoral (ESPOL)
Author:Joseph Avila 
*/

#include "../include/allocator.h"
#define CMDMAX 1000

int analizar_cmd(char *cmd, block_t *block);

int main(void) {
    char *cmd = (char*)malloc(CMDMAX * sizeof(char));
    block_t block;

    if (cmd == NULL) {
        printf("[-]¡No se pudo cargar el comando!\n");
        return -1;
    }

    while (fgets(cmd, CMDMAX, stdin)) {
        if (analizar_cmd(cmd, &block) <= 0) {
            break;
        }
    }

    free(cmd);

    return 0;
}

int analizar_cmd(char *cmd, block_t *block) {
    int tamanio, posicion, valor, aliniacion;
    const char *delimitador = " \n";
    char *arg = strtok(cmd, delimitador);

    if (!strcmp(arg, "ASIGNAR")) {
        tamanio = atoi(arg = strtok(NULL, delimitador));
        printf("%d\n", asignar(block, tamanio));
    } else if (!strcmp(arg, "FREE")) {
        posicion = atoi(arg = strtok(NULL, delimitador));
        FREE(block, posicion);
    } else if (!strcmp(arg, "LLENAR")) {
        posicion = atoi(arg = strtok(NULL, delimitador));
        tamanio = atoi(arg = strtok(NULL, delimitador));
        valor = atoi(arg = strtok(NULL, delimitador));
        llenar(block, posicion, tamanio, valor);
    } else if (!strcmp(arg, "SLLENAR")) {
        posicion = atoi(arg = strtok(NULL, delimitador));
        tamanio = atoi(arg = strtok(NULL, delimitador));
        valor = atoi(arg = strtok(NULL, delimitador));
        safe_fill(block, posicion, tamanio, valor);
    } else if (!strcmp(arg, "DUMP")) {
        volcado(block);
    } else if (!strcmp(arg, "MOSTRAR")) {
        arg = strtok(NULL, delimitador);
        mostrar(block, arg);
    } else if (!strcmp(arg, "REASIGNAR")) {
        posicion = atoi(arg = strtok(NULL, delimitador));
        tamanio = atoi(arg = strtok(NULL, delimitador));
        printf("%d\n", realloc_(block, posicion, tamanio));
    } else if (!strcmp(arg, "ASIGNADOS")) {
        tamanio = atoi(arg = strtok(NULL, delimitador));
        aliniacion = atoi(arg = strtok(NULL, delimitador));
        printf("%d\n", ALLOCALIGNED(block, tamanio, aliniacion));
    } else if (!strcmp(arg, "DEFRAGMENTAR")) {
        int i;
        defrag_t *def = calloc((block->len - 4) / 13, sizeof(defrag_t));

        if (def == NULL) {
            printf("[-]¡No se pudo realizar la desfragmentación!\n");
            return -1;
        }

        defragmentar(block, def);

        for (i = 0; def[i].oldPos; ++i) {
            printf("Posición antes de desfragmentar: %d\t", def[i].oldPos);
            printf("Posición después de la desfragmentación: %d\n", def[i].newPos);
        }

        free(def);
    } else if (!strcmp(arg, "INICIALIZAR")) {
        block->len = atoi(arg = strtok(NULL, delimitador));
        inicializar(block);

        if (block->mem == NULL) {
            printf("[-]¡No se pudo asignar la arena!\n");
            return -1;
        }
    } else if (!strcmp(arg, "FINALIZAR")) {
        finalizar(block);
        return 0;
    } else {
        printf("[-]¡Comando desconocido!\n");
    }

    return 1;
} 