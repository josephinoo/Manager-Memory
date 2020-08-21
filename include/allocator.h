

/*
Escuela Superior Politécnica del Litoral (ESPOL)
Author:Joseph Avila 
*/
#ifndef ALOCATOR_H
#define ALOCATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef unsigned char uchar_t;
typedef struct {
    int32_t oldPos, newPos;
} defrag_t;
typedef struct {
    int32_t len;
    uchar_t *mem;
    int32_t liberaciones;
} block_t;



int32_t ALLOCALIGNED(block_t *block, int32_t size, int32_t align);
int32_t buscarPos(int32_t pos, int32_t align, int32_t size,
                int32_t prevFinal, int32_t nextFirst);

int32_t realloc_(block_t *block, int32_t pos, int32_t size);
void copiarMemoria(block_t *block, int32_t *currIndex, int newPos, int size);
void defragmentar(block_t *block, defrag_t *def);
void FREE(block_t *block, int32_t pos);
void llenar(block_t *block, int32_t pos, int32_t size, int32_t value);
void safe_fill(block_t *block, int32_t pos, int32_t size, int32_t value);
void mostrar(block_t *block, char *arg);
void mostrar_map(block_t *block, int32_t l);
void printMemoria(int32_t toPrint, int32_t *count, char c);
int32_t asignar(block_t *block, int32_t size);
void asignarBtw(block_t *block, int32_t *prev, int32_t *next,
              int32_t start, int32_t size);
void asignarLast(block_t *block, int32_t *prev, int32_t *next,
               int32_t start, int32_t size);
void volcado(block_t *block);
void mostrar_free(block_t *block);
void mostrar_usage(block_t *block);
void mostrar_alloc(block_t *block);
int32_t verificarPos(block_t *block, int32_t pos);
void inicializar(block_t *block);
void finalizar(block_t *block);


#endif