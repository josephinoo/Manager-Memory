// Copyright 2017 Dutu Teodor-Stefan

#include "./allocator.h"

int32_t ALLOC(arena_t *arena, int32_t size) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t prevFinal = 4;

    size += 12;

    if (size > arena->len - 4) {
        return 0;
    }  // daca trebuie alocati mai multi octeti decat dimensiunea arenei

    if (!*prevIndex) {
        *prevIndex = 4;
        *(prevIndex + 1) = 0;
        *(prevIndex + 2) = 0;
        *(prevIndex + 3) = size;
        return (*prevIndex + 12);
    }  // daca nu exista nicio zona alocata

    while (*nextIndex && *prevIndex - prevFinal < size) {
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  // se cauta pozitia in care poate fi alocata zona

    if (*prevIndex - prevFinal >= size) {
        allocBtw(arena, prevIndex, nextIndex, prevFinal, size);
        return (*prevIndex + 12);
    }  // daca zona noua se poate aloca intre 2 zone deja alocate

    if (arena->len - *prevIndex - *(nextIndex + 2) < size) {
        return 0;
    }  // daca noua zona trebuie alocata la finalul arenei, dar nu are loc

    prevFinal = *prevIndex + *(nextIndex + 2);
    allocLast(arena, prevIndex, nextIndex, prevFinal, size);
    return  (*nextIndex + 12);
    // daca zona trebuie alocata dupa ultima zona disponibila in arena
}  // functia returneaza pozitia de incepere a zonei utilizabile alocate

void allocBtw(arena_t *arena, int32_t *prev, int32_t *next,
              int32_t start, int32_t size) {
    int32_t *newIndex = (int32_t*)(arena->mem + start);

    *newIndex = *prev;
    *(newIndex + 1) = *(next + 1);
    *(newIndex + 2) = size;
    *prev = start;
    *(next + 1) = *prev;
}

void allocLast(arena_t *arena, int32_t *prev, int32_t *next,
               int32_t start, int32_t size) {
    int32_t *newIndex = (int32_t*)(arena->mem + start);

    *next = start;
    *newIndex = 0;
    *(newIndex + 1) = *prev;
    *(newIndex + 2) = size;
}

void DUMP(arena_t *arena) {
    uchar_t *set, *byte, *end = arena->mem + arena->len - 1;
    int32_t i;

    for (set = arena->mem; set <= end; set += 16) {
        printf("%08X\t", (int32_t)(set - arena->mem));
        // se afiseaza indicele liniei
        byte = set;

        for (i = 0; i < 8 && byte <= end; ++i) {
            printf("%02X ", (int32_t)*(byte++));
        }  // se afiseaza primul grup de 8 octeti

        for (; i < 16 && byte <= end; ++i) {
            printf(" %02X", (int32_t)*(byte++));
        }  // se afiseaza al doilea grup de 8 octeti
        printf("\n");
    }

    printf("%08X\n", arena->len);
}

void FREE(arena_t *arena, int32_t pos) {
    int32_t *currIndex = (int32_t*)(arena->mem + pos - 12);
    int32_t *nextIndex = (int32_t*)(arena->mem + *currIndex);
    int32_t *prevIndex = (int32_t*)(arena->mem + *(currIndex + 1));

    if (*currIndex) {  // daca zona de dealocat nu este ultima din arena
        *prevIndex = *currIndex;
        *(nextIndex + 1) = *(currIndex + 1);
    } else {
        *prevIndex = 0;
    }  // daca zona de dealocat este ultima
}

void FILL(arena_t *arena, int32_t pos, int32_t  size, int32_t value) {
    uchar_t *byte = arena->mem + pos;
    uchar_t *end = byte + size - 1;

    for (; byte <= end; ++byte) {
        *byte = (uchar_t)value;
    }  // se modifica size octeti unul cate unul
}

void SHOW(arena_t *arena, char *arg) {
    const char *delim = " \n";
    int32_t length;

    if (!strcmp(arg, "FREE")) {
        SHOW_FREE(arena);
    } else if (!strcmp(arg, "USAGE")) {
         SHOW_USAGE(arena);
    } else if (!strcmp(arg, "ALLOCATIONS")) {
        SHOW_ALLOC(arena);
    } else {
        length = atoi(arg = strtok(NULL, delim));
        SHOW_MAP(arena, length);
    }
}

void SHOW_FREE(arena_t *arena) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t prevFinal = 4, freeBytes = arena->len - 4, freeBlocks = 1;

    while (*prevIndex) {
        freeBytes -= *(nextIndex + 2);

        if (*prevIndex - prevFinal > 0) {
            ++freeBlocks;
        }

        if (*prevIndex + *(nextIndex + 2) == arena->len) {
            --freeBlocks;
        }  // daca nu exista octeti liberi la sfarsitul arenei

        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  // se verifica zonele libere dintre fiecare 2 zone alocate

    printf("%d blocks (%d bytes) free\n", freeBlocks, freeBytes);
}

void SHOW_USAGE(arena_t *arena) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t eff, frag, prevFinal = 4;
    int32_t freeBytes = arena->len - 4, freeBlocks = 1;
    int32_t usedBytes = 0, usedBlocks = 0;

    while (*prevIndex) {
        usedBytes += (*(nextIndex + 2) - 12);
        ++usedBlocks;
        freeBytes -= *(nextIndex + 2);

        if (*prevIndex - prevFinal > 0) {
            ++freeBlocks;
        }  // daca exista un spatiu gol intre cele 2 zone

        if (*prevIndex + *(nextIndex + 2) == arena->len) {
            --freeBlocks;
        }  // daca nu exista octeti liberi la sfarsitul arenei

        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  // se analizeaza zonele alocate una cate una

    printf("%d blocks (%d bytes) used\n", usedBlocks, usedBytes);

    eff = 100 * usedBytes / (arena->len - freeBytes);
    printf("%d%% efficiency\n", eff);

    frag = usedBlocks ? 100 * (freeBlocks - 1) / usedBlocks : 0;
    printf("%d%% fragmentation\n", frag);
}

void SHOW_ALLOC(arena_t *arena) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t freeBytes, prevFinal = 4;

    printf("OCCUPIED 4 bytes\n");  // zona de start

    while (*prevIndex) {
        freeBytes = *prevIndex - prevFinal;

        if (freeBytes) {
            printf("FREE %d bytes\n", freeBytes);
        }

        printf("OCCUPIED %d bytes\n", *(nextIndex + 2));

        prevFinal = *(nextIndex + 2) + *prevIndex;
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
    }  // se analizeaza arena zona cu zona

    if (arena->len - prevFinal) {
        printf("FREE %d bytes\n", arena->len - prevFinal);
    }  // daca raman octeti liberi la finalul arenei, se afisaza
}

void SHOW_MAP(arena_t *arena, int32_t length) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t toPrint, prevFinal = 4, count = 1;
    double x = (double)arena->len / (double)length;
    double bytes = 4, extra;

    toPrint = (int32_t)(bytes / x);
    extra = x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
    printMem(toPrint, &count, '*');

    while (*prevIndex) {
        bytes = (double)(*prevIndex - prevFinal) - extra;
        toPrint = (int32_t)(bytes / x);
        extra = bytes - x * (double)toPrint;
        printMem(toPrint, &count, '.');
        // se calculeaza numarul de octeti liberi si se afiseaza '.'

        bytes = (double)*(nextIndex + 2) + extra;
        if (bytes > 0) {
            toPrint = (int32_t)(bytes / x);
            extra =
            x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
            printMem(toPrint, &count, '*');
        } else {
            extra = -bytes;
        }  // daca octetii ocupati nu au fost analizati inainte se afiseaza '*'

        prevFinal = *(nextIndex + 2) + *prevIndex;
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
    }  // sunt parcurse zonele ocupate si se afiseaza '.' sau '*' dupa caz

    toPrint = (int32_t)(((double)(arena->len - prevFinal) - extra) / x);
    printMem(toPrint, &count, '.');
    // se analizeaza ultimii octeti liberi din arena

    if (count % 80) {
        printf("\n");
    }  // se va pune cursorul pe un rand nou daca acest lucur nu s-a facut deja
}

void printMem(int32_t toPrint, int32_t *count, char c) {
    int32_t i;

    for (i = 0; i < toPrint; ++i, ++*count) {
        printf("%c", c);
        if (!(*count % 80)) {
            printf("\n");
        }
    }
}

int32_t ALLOCALIGNED(arena_t *arena, int32_t size, int32_t align) {
    int32_t *prevIndex = (int32_t*)arena->mem;
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t *newIndex;
    int32_t prevFinal = 4, pos = align < 16 ? 16 : align;

    if (!*prevIndex) {
        if (pos + size - 1 > arena->len) {
            return 0;  // daca nu este suficient loc in matrice
        }

        newIndex = (int*)(arena->mem + pos - 12);
        *newIndex = 0;
        *(newIndex + 1) = 0;
        *(newIndex + 2) = size + 12;
        *prevIndex = pos - 12;
        return pos;  // daca nu mai exista nicio alta zona alocata
    }

    while (*nextIndex) {
        pos = findPos(pos, align, size, prevFinal, *prevIndex);

        if (pos - 12 >= prevFinal && pos + size - 1 < *prevIndex) {
            allocBtw(arena, prevIndex, nextIndex, pos - 12, size + 12);
            return pos;
        }  // noua zona se poate aloca intre 2 zone deja alocate

        prevIndex = nextIndex;
        nextIndex = (int*)(arena->mem + *nextIndex);
        prevFinal = *(nextIndex + 1) + *(prevIndex + 2);
    }  // se cauta sa se aloce noua zona intre 2 zone deja alocate

    pos = findPos(pos, align, size, prevFinal, *prevIndex);

    if (pos - 12 >= prevFinal && pos + size <= *prevIndex) {
        allocBtw(arena, prevIndex, nextIndex, pos - 12, size + 12);
        return pos;
    }  // noua zona se poate aloca intre 2 zone deja alocate

    prevFinal = *prevIndex + *(nextIndex + 2);
    pos = findPos(pos, align, size, prevFinal, arena->len);

    if (pos + size > arena->len) {
        return 0;
    }  // noua zona nu are loc in arena

    allocLast(arena, prevIndex, nextIndex, pos - 12, size + 12);
    return pos;  // zona cea noua are loc dupa utima zona alocata
}  // algoritmul este similar cu cel folosit la ALLOC

int32_t findPos(int32_t pos, int32_t align, int32_t size,
                int32_t prevFinal, int32_t nextFirst) {
    while (pos < prevFinal) {
        pos <<= 1;
    }

    if (pos > align) {
        pos >>= 1;
    }

    while (pos - 12 < prevFinal && pos + size <= nextFirst) {
        pos += align;
    }

    return pos;
    /* se cauta o pozitie aliniata la align care sa incapa intre
    prevFinal si nextFirst */
}

int32_t REALLOC(arena_t *arena, int32_t pos, int32_t size) {
    int32_t *currIndex = (int32_t*)(arena->mem + pos - 12);
    int32_t *nextIndex = (int32_t*)(arena->mem + *currIndex);
    int32_t *prevIndex = (int32_t*)(arena->mem + *(currIndex + 1));
    int32_t newPos;

    FREE(arena, pos);

    if (!(newPos = ALLOC(arena, size))) {
        *prevIndex = pos - 12;
        if (*currIndex) {
            *(nextIndex + 1) = pos - 12;
        }  // se reface structura pe care o avea arena inainte de realocare
        return 0;
    }  // daca zona nu poate fi realocata

    copyMem(arena, currIndex, newPos, size);

    return newPos;
}

void copyMem(arena_t *arena, int32_t *currIndex, int newPos, int memSize) {
    uchar_t *endB, *endS, *byte, *startPos;

    byte = (uchar_t*)currIndex + 12;
    endB = byte + *(currIndex + 2) - 13;  // sfarsitul zonei de realocat
    startPos = arena->mem + newPos;
    endS = startPos + memSize - 1;  // sfarsitul zonei realocate

    for (; startPos <= endS && byte <= endB; ++startPos, ++byte) {
        *startPos = *byte;
    }  // copierea memoriei din zona initiala in cea realocata
}

void DEFRAG(arena_t *arena, defrag_t *def) {
    int32_t *prevIndex = (int32_t*)(arena->mem);
    int32_t *currIndex = (int32_t*)(arena->mem + *prevIndex);
    int32_t *nextIndex = (int32_t*)(arena->mem + *currIndex);
    int32_t prevFinal = 4, i = 0;

    while (*currIndex) {
        if (prevFinal != *prevIndex) {
            def[i].oldPos = *prevIndex + 12;
            def[i++].newPos = prevFinal + 12;

            FREE(arena, *prevIndex + 12);
            prevFinal = *(currIndex + 1) ?
                *(currIndex + 1) + *(prevIndex + 2) : 4;
            allocBtw(arena, prevIndex, nextIndex,
                     prevFinal, *(currIndex + 2));
            copyMem(arena, currIndex, *prevIndex + 12,
                    *(currIndex + 2) - 12);
            currIndex = (int32_t*)(arena->mem + *prevIndex);
        }

        prevIndex = currIndex;
        currIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
        prevFinal = *(currIndex + 1) + *(prevIndex + 2);
    }  // se muta toate zonele pana la ultima

    if (prevFinal != *prevIndex && *prevIndex) {
        def[i].oldPos = *prevIndex + 12;
        def[i++].newPos = prevFinal + 12;

        *prevIndex = *(currIndex + 1) ?
            *(currIndex + 1) + *(prevIndex + 2) : 4;
        // daca exista o singura zona in arena, aceasta se muta pe pozitia 4
        nextIndex = currIndex;

        currIndex = (int32_t*)(arena->mem + *prevIndex);
        *currIndex = *nextIndex;
        *(currIndex + 1) = *(nextIndex + 1);
        *(currIndex + 2) = *(nextIndex + 2);
        copyMem(arena, nextIndex, *prevIndex + 12, *(currIndex + 2) - 12);
    }  // se trateaza cazul ultimei zone din memorie
}

void SAFE_FILL(arena_t *arena, int32_t pos, int32_t  size, int32_t value) {
    int32_t *currIndex;
    uchar_t *byte, *end;
    int32_t startPos = checkPos(arena, pos), finPos;

    if (!startPos) {
        printf("Invalid access to memory!\n");
    } else {
        currIndex = (int32_t*)(arena->mem + startPos - 12);
        finPos = startPos + *(currIndex + 2) - 13 < pos + size ?
              startPos + *(currIndex + 2) - 13 : pos + size - 1;
        end = arena->mem + finPos;
        // se determina pozitia de dupa ultimul octet ce trebuie modificat

        for (byte = arena->mem + pos; byte <= end; ++byte) {
            *byte = (uchar_t)value;
        }  // se modifica maximum size octeti, dintre cei alocati

        printf("%d byte(s) modified\n", finPos - pos + 1);
    }
}

int32_t checkPos(arena_t *arena, int32_t pos) {
    int32_t *prevIndex = (int32_t*)(arena->mem);
    int32_t *nextIndex = (int32_t*)(arena->mem + *prevIndex);

    if (arena->len <= pos || pos < 4) {
        return 0;
    }  // daca pozitia verificata se afla in afara arenei sau in zona de start

    while (*prevIndex && pos > *prevIndex) {
        if (pos >= *prevIndex + 12 && pos < *prevIndex + *(nextIndex + 2)) {
            return (*prevIndex + 12);
        }

        prevIndex = nextIndex;
        nextIndex = (int32_t*)(arena->mem + *nextIndex);
    }  // se cauta zona in care se afla pos

    return 0;
    /* functia retruneaza indicele de inceput al zonei in care se afla pos
    sau 0 daca acesta nu se gaseste in arena sau nu este intr-o zona alocata */
}

void INITIALIZE(arena_t *arena) {
    arena->mem = (uchar_t*)calloc(arena->len, sizeof(uchar_t));
}  // alocarea memoriei pentru arena

void FINALIZE(arena_t *arena) {
    free(arena->mem);
}  // se dealoca memoria alocata la inceput
