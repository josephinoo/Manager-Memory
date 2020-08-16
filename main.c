// Copyright 2017 Dutu Teodor-Stefan

#include "./allocator.h"
#define CMDMAX 1000

int parseCmd(char *cmd, arena_t *arena);

int main(void) {
    char *cmd = (char*)malloc(CMDMAX * sizeof(char));
    arena_t arena;

    if (cmd == NULL) {
        printf("Command could not be loaded!\n");
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

int parseCmd(char *cmd, arena_t *arena) {
    int size, pos, value, align;
    const char *delim = " \n";
    char *arg = strtok(cmd, delim);

    if (!strcmp(arg, "ALLOC")) {
        size = atoi(arg = strtok(NULL, delim));
        printf("%d\n", ALLOC(arena, size));
    } else if (!strcmp(arg, "FREE")) {
        pos = atoi(arg = strtok(NULL, delim));
        FREE(arena, pos);
    } else if (!strcmp(arg, "FILL")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        value = atoi(arg = strtok(NULL, delim));
        FILL(arena, pos, size, value);
    } else if (!strcmp(arg, "SFILL")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        value = atoi(arg = strtok(NULL, delim));
        SAFE_FILL(arena, pos, size, value);
    } else if (!strcmp(arg, "DUMP")) {
        DUMP(arena);
    } else if (!strcmp(arg, "SHOW")) {
        arg = strtok(NULL, delim);
        SHOW(arena, arg);
    } else if (!strcmp(arg, "REALLOC")) {
        pos = atoi(arg = strtok(NULL, delim));
        size = atoi(arg = strtok(NULL, delim));
        printf("%d\n", REALLOC(arena, pos, size));
    } else if (!strcmp(arg, "ALLOCALIGNED")) {
        size = atoi(arg = strtok(NULL, delim));
        align = atoi(arg = strtok(NULL, delim));
        printf("%d\n", ALLOCALIGNED(arena, size, align));
    } else if (!strcmp(arg, "DEFRAGMENT")) {
        int i;
        defrag_t *def = calloc((arena->len - 4) / 13, sizeof(defrag_t));

        if (def == NULL) {
            printf("The defragmentation could not be performed!\n");
            return -1;
        }

        DEFRAG(arena, def);

        for (i = 0; def[i].oldPos; ++i) {
            printf("Position before defrag: %d\t", def[i].oldPos);
            printf("Position after defrag: %d\n", def[i].newPos);
        }

        free(def);
    } else if (!strcmp(arg, "INITIALIZE")) {
        arena->len = atoi(arg = strtok(NULL, delim));
        INITIALIZE(arena);

        if (arena->mem == NULL) {
            printf("The arena could not be allocated!\n");
            return -1;
        }
    } else if (!strcmp(arg, "FINALIZE")) {
        FINALIZE(arena);
        return 0;
    } else {
        printf("Unknown command!\n");
    }

    return 1;
}  // functia analizeaza fiecare input si executa comanda aferenta acestuia
