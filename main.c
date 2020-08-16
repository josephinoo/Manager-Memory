

#include "./allocator.h"
#define CMDMAX 1000

int parseCmd(char *cmd, arena_t *arena);

int main(void) {
    char *cmd = (char*)malloc(CMDMAX * sizeof(char));
    arena_t arena;

    if (cmd == NULL) {
        printf("[-]¡No se pudo cargar el comando!\n");
        return -1;
    }

    while (fgets(cmd, CMDMAX, stdin)) {
        if (parseCmd(cmd, &arena) <= 0) {
            break;
        }
    }

    free(cmd);

    return 0;
}

int parseCmd(char *cmd, arena_t *block) {
    int size, pos, value, align;
    const char *delim = " \n";
    char *arg = strtok(cmd, delim);

    if (!strcmp(arg, "ALLOC")) {
        size = atoi(arg = strtok(NULL, delim));
        printf("%d\n", asignar(block, size));
    } else if (!strcmp(arg, "FREE")) {
        pos = atoi(arg = strtok(NULL, delim));
        FREE(block, pos);
    } else if (!strcmp(arg, "FILL")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        value = atoi(arg = strtok(NULL, delim));
        llenar(block, pos, size, value);
    } else if (!strcmp(arg, "SFILL")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        value = atoi(arg = strtok(NULL, delim));
        safe_fill(block, pos, size, value);
    } else if (!strcmp(arg, "DUMP")) {
        volcado(block);
    } else if (!strcmp(arg, "SHOW")) {
        arg = strtok(NULL, delim);
        mostrar(block, arg);
    } else if (!strcmp(arg, "REALLOC")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        printf("%d\n", realloc_(block, pos, size));
    } else if (!strcmp(arg, "ALLOCALIGNED")) {
        size = atoi(arg = strtok(NULL, delim));
        align = atoi(arg = strtok(NULL, delim));
        printf("%d\n", ALLOCALIGNED(block, size, align));
    } else if (!strcmp(arg, "DEFRAGMENT")) {
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
    } else if (!strcmp(arg, "INITIALIZE")) {
        block->len = atoi(arg = strtok(NULL, delim));
        inicializar(block);

        if (block->mem == NULL) {
            printf("[-]¡No se pudo asignar la arena!\n");
            return -1;
        }
    } else if (!strcmp(arg, "FINALIZE")) {
        finalizar(block);
        return 0;
    } else {
        printf("[-]¡Comando desconocido!\n");
    }

    return 1;
}  // functia analizeaza fiecare input si executa comanda aferenta acestuia
