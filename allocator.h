

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char uchar_t;

typedef struct {
    int32_t len;
    uchar_t *mem;
} arena_t;

typedef struct {
    int32_t oldPos, newPos;
} defrag_t;

void inicializar(arena_t *block);
void finalizar(arena_t *block);
int32_t asignar(arena_t *block, int32_t size);
void allocBtw(arena_t *block, int32_t *prev, int32_t *next,
              int32_t start, int32_t size);
void allocLast(arena_t *arena, int32_t *prev, int32_t *next,
               int32_t start, int32_t size);
void volcado(arena_t *arena);
void FREE(arena_t *arena, int32_t pos);
void llenar(arena_t *arena, int32_t pos, int32_t size, int32_t value);
void safe_fill(arena_t *arena, int32_t pos, int32_t size, int32_t value);
void mostrar(arena_t *arena, char *arg);
void mostrar_free(arena_t *arena);
void mostrar_usage(arena_t *arena);
void mostrar_alloc(arena_t *arena);
void mostrar_map(arena_t *arena, int32_t l);
void printMemoria(int32_t toPrint, int32_t *count, char c);
int32_t ALLOCALIGNED(arena_t *arena, int32_t size, int32_t align);
int32_t buscarPos(int32_t pos, int32_t align, int32_t size,
                int32_t prevFinal, int32_t nextFirst);

int32_t realloc_(arena_t *arena, int32_t pos, int32_t size);
void copiarMemoria(arena_t *arena, int32_t *currIndex, int newPos, int size);
void defragmentar(arena_t *arena, defrag_t *def);
int32_t verificarPos(arena_t *arena, int32_t pos);


#endif
