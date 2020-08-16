// Copyright 2017 Dutu Teodor-Stefan

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

void INITIALIZE(arena_t *arena);
void FINALIZE(arena_t *arena);
int32_t ALLOC(arena_t *arena, int32_t size);
void allocBtw(arena_t *arena, int32_t *prev, int32_t *next,
              int32_t start, int32_t size);
// insereaza o zona intre 2 indici (pointeri) din arena
void allocLast(arena_t *arena, int32_t *prev, int32_t *next,
               int32_t start, int32_t size);
// adauga o zona dupa ultima alocata anterior
void DUMP(arena_t *arena);
void FREE(arena_t *arena, int32_t pos);
void FILL(arena_t *arena, int32_t pos, int32_t size, int32_t value);
void SAFE_FILL(arena_t *arena, int32_t pos, int32_t size, int32_t value);
// este un FILL care nu suprascrie zone invalide sau nealocate
void SHOW(arena_t *arena, char *arg);
void SHOW_FREE(arena_t *arena);
void SHOW_USAGE(arena_t *arena);
void SHOW_ALLOC(arena_t *arena);
void SHOW_MAP(arena_t *arena, int32_t l);
void printMem(int32_t toPrint, int32_t *count, char c);
// afiseaza caracteul c (* sau .), de toPrint ori, in formatul cerut de SHOW_MAP
int32_t ALLOCALIGNED(arena_t *arena, int32_t size, int32_t align);
int32_t findPos(int32_t pos, int32_t align, int32_t size,
                int32_t prevFinal, int32_t nextFirst);
// functia cauta pozitia la care ar putea fi alocata noua zona aliniata la align
int32_t REALLOC(arena_t *arena, int32_t pos, int32_t size);
void copyMem(arena_t *arena, int32_t *currIndex, int newPos, int size);
/* copiaza octetii din zona currIndex in zona newPos,
tinand seama de dimensiunile acestor zone*/
void DEFRAG(arena_t *arena, defrag_t *def);
/* alipeste toate zonele alocate din arena si intoarce
un vector cu pozitiile vechi si noi ale acestora */
int32_t checkPos(arena_t *arena, int32_t pos);
/* returneaza 0 daca pos nu face parte dintr-o zona disponibila sau
pozitia de inceput a zonei din care acesta face parte in caz contrar*/

#endif
