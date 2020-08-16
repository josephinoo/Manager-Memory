

#include "./allocator.h"

int32_t asignar(arena_t *block, int32_t size) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t prevFinal = 4;

    size += 12;

    if (size > block->len - 4) {
        return 0;
    }  
    if (!*prevIndex) {
        *prevIndex = 4;
        *(prevIndex + 1) = 0;
        *(prevIndex + 2) = 0;
        *(prevIndex + 3) = size;
        return (*prevIndex + 12);
    } 

    while (*nextIndex && *prevIndex - prevFinal < size) {
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  

    if (*prevIndex - prevFinal >= size) {
        allocBtw(block, prevIndex, nextIndex, prevFinal, size);
        return (*prevIndex + 12);
    }  
    if (block->len - *prevIndex - *(nextIndex + 2) < size) {
        return 0;
    }  
    prevFinal = *prevIndex + *(nextIndex + 2);
    allocLast(block, prevIndex, nextIndex, prevFinal, size);
    return  (*nextIndex + 12);
    
}  

void allocBtw(arena_t *block, int32_t *prev, int32_t *next,
              int32_t start, int32_t size) {
    int32_t *newIndex = (int32_t*)(block->mem + start);

    *newIndex = *prev;
    *(newIndex + 1) = *(next + 1);
    *(newIndex + 2) = size;
    *prev = start;
    *(next + 1) = *prev;
}

void allocLast(arena_t *block, int32_t *prev, int32_t *next,
               int32_t start, int32_t size) {
    int32_t *newIndex = (int32_t*)(block->mem + start);

    *next = start;
    *newIndex = 0;
    *(newIndex + 1) = *prev;
    *(newIndex + 2) = size;
}

void volcado(arena_t *block) {
    uchar_t *set, *byte, *end = block->mem + block->len - 1;
    int32_t i;

    for (set = block->mem; set <= end; set += 16) {
        printf("%08X\t", (int32_t)(set - block->mem));
        
        byte = set;

        for (i = 0; i < 8 && byte <= end; ++i) {
            printf("%02X ", (int32_t)*(byte++));
        }  

        for (; i < 16 && byte <= end; ++i) {
            printf(" %02X", (int32_t)*(byte++));
        }  
        printf("\n");
    }

    printf("%08X\n", block->len);
}

void FREE(arena_t *block, int32_t pos) {
    int32_t *currIndex = (int32_t*)(block->mem + pos - 12);
    int32_t *nextIndex = (int32_t*)(block->mem + *currIndex);
    int32_t *prevIndex = (int32_t*)(block->mem + *(currIndex + 1));

    if (*currIndex) {  
        *prevIndex = *currIndex;
        *(nextIndex + 1) = *(currIndex + 1);
    } else {
        *prevIndex = 0;
    }  
}

void llenar(arena_t *block, int32_t pos, int32_t  size, int32_t value) {
    uchar_t *byte = block->mem + pos;
    uchar_t *end = byte + size - 1;

    for (; byte <= end; ++byte) {
        *byte = (uchar_t)value;
    }  
}

void mostrar(arena_t *block, char *arg) {
    const char *delim = " \n";
    int32_t length;

    if (!strcmp(arg, "FREE")) {
        mostrar_free(block);
    } else if (!strcmp(arg, "USAGE")) {
         mostrar_usage(block);
    } else if (!strcmp(arg, "ALLOCATIONS")) {
        mostrar_alloc(block);
    } else {
        length = atoi(arg = strtok(NULL, delim));
        mostrar_map(block, length);
    }
}

void mostrar_free(arena_t *block) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t prevFinal = 4, freeBytes = block->len - 4, freeBlocks = 1;

    while (*prevIndex) {
        freeBytes -= *(nextIndex + 2);

        if (*prevIndex - prevFinal > 0) {
            ++freeBlocks;
        }

        if (*prevIndex + *(nextIndex + 2) == block->len) {
            --freeBlocks;
        }  
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  

    printf("%d blocks (%d bytes) free\n", freeBlocks, freeBytes);
}

void mostrar_usage(arena_t *block) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t eff, frag, prevFinal = 4;
    int32_t freeBytes = block->len - 4, freeBlocks = 1;
    int32_t usedBytes = 0, usedBlocks = 0;

    while (*prevIndex) {
        usedBytes += (*(nextIndex + 2) - 12);
        ++usedBlocks;
        freeBytes -= *(nextIndex + 2);

        if (*prevIndex - prevFinal > 0) {
            ++freeBlocks;
        } 

        if (*prevIndex + *(nextIndex + 2) == block->len) {
            --freeBlocks;
        }  
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
        prevFinal = *(prevIndex + 2) + *(nextIndex + 1);
    }  
    printf("%d blocks (%d bytes) usados\n", usedBlocks, usedBytes);

    eff = 100 * usedBytes / (block->len - freeBytes);
    printf("%d%% Eficiencia", eff);

    frag = usedBlocks ? 100 * (freeBlocks - 1) / usedBlocks : 0;
    printf("%d%% Fragmentacion\n", frag);
}

void mostrar_alloc(arena_t *block) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t freeBytes, prevFinal = 4;

    printf("Ocupados 4 bytes\n");  

    while (*prevIndex) {
        freeBytes = *prevIndex - prevFinal;

        if (freeBytes) {
            printf("Libres%d bytes\n", freeBytes);
        }

        printf("OCupados %d bytes\n", *(nextIndex + 2));

        prevFinal = *(nextIndex + 2) + *prevIndex;
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
    }  

    if (block->len - prevFinal) {
        printf("Libres %d bytes\n", block->len - prevFinal);
    }  
}

void mostrar_map(arena_t *block, int32_t tamanio) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t toPrint, prevFinal = 4, count = 1;
    double x = (double)block->len / (double)tamanio;
    double bytes = 4, extra;

    toPrint = (int32_t)(bytes / x);
    extra = x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
    printMemoria(toPrint, &count, '*');

    while (*prevIndex) {
        bytes = (double)(*prevIndex - prevFinal) - extra;
        toPrint = (int32_t)(bytes / x);
        extra = bytes - x * (double)toPrint;
        printMemoria(toPrint, &count, '.');
    
        bytes = (double)*(nextIndex + 2) + extra;
        if (bytes > 0) {
            toPrint = (int32_t)(bytes / x);
            extra =
            x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
            printMemoria(toPrint, &count, '*');
        } else {
            extra = -bytes;
        }  

        prevFinal = *(nextIndex + 2) + *prevIndex;
        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
    } 
    toPrint = (int32_t)(((double)(block->len - prevFinal) - extra) / x);
    printMemoria(toPrint, &count, '.');
   

    if (count % 80) {
        printf("\n");
    }  
}

void printMemoria(int32_t toPrint, int32_t *contador, char c) {
    int32_t i;

    for (i = 0; i < toPrint; ++i, ++*contador) {
        printf("%c", c);
        if (!(*contador % 80)) {
            printf("\n");
        }
    }
}

int32_t ALLOCALIGNED(arena_t *block, int32_t size, int32_t align) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t *newIndex;
    int32_t prevFinal = 4, pos = align < 16 ? 16 : align;

    if (!*prevIndex) {
        if (pos + size - 1 > block->len) {
            return 0;  
        }

        newIndex = (int*)(block->mem + pos - 12);
        *newIndex = 0;
        *(newIndex + 1) = 0;
        *(newIndex + 2) = size + 12;
        *prevIndex = pos - 12;
        return pos;  
    }

    while (*nextIndex) {
        pos = buscarPos(pos, align, size, prevFinal, *prevIndex);

        if (pos - 12 >= prevFinal && pos + size - 1 < *prevIndex) {
            allocBtw(block, prevIndex, nextIndex, pos - 12, size + 12);
            return pos;
        }  

        prevIndex = nextIndex;
        nextIndex = (int*)(block->mem + *nextIndex);
        prevFinal = *(nextIndex + 1) + *(prevIndex + 2);
    }  
    pos = buscarPos(pos, align, size, prevFinal, *prevIndex);

    if (pos - 12 >= prevFinal && pos + size <= *prevIndex) {
        allocBtw(block, prevIndex, nextIndex, pos - 12, size + 12);
        return pos;
    }  

    prevFinal = *prevIndex + *(nextIndex + 2);
    pos = buscarPos(pos, align, size, prevFinal, block->len);

    if (pos + size > block->len) {
        return 0;
    }  

    allocLast(block, prevIndex, nextIndex, pos - 12, size + 12);
    return pos;  
}  

int32_t buscarPos(int32_t pos, int32_t align, int32_t size,
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
    
}

int32_t realloc_(arena_t *block, int32_t pos, int32_t size) {
    int32_t *currIndex = (int32_t*)(block->mem + pos - 12);
    int32_t *nextIndex = (int32_t*)(block->mem + *currIndex);
    int32_t *prevIndex = (int32_t*)(block->mem + *(currIndex + 1));
    int32_t newPos;

    FREE(block, pos);

    if (!(newPos = asignar(block, size))) {
        *prevIndex = pos - 12;
        if (*currIndex) {
            *(nextIndex + 1) = pos - 12;
        }  
        return 0;
    }  
    copiarMemoria(block, currIndex, newPos, size);

    return newPos;
}

void copiarMemoria(arena_t *block, int32_t *currIndex, int newPos, int memSize) {
    uchar_t *endB, *endS, *byte, *startPos;

    byte = (uchar_t*)currIndex + 12;
    endB = byte + *(currIndex + 2) - 13;  
    startPos = block->mem + newPos;
    endS = startPos + memSize - 1;  

    for (; startPos <= endS && byte <= endB; ++startPos, ++byte) {
        *startPos = *byte;
    }  
}

void defragmentar(arena_t *block, defrag_t *def) {
    int32_t *prevIndex = (int32_t*)(block->mem);
    int32_t *currIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t *nextIndex = (int32_t*)(block->mem + *currIndex);
    int32_t prevFinal = 4, i = 0;

    while (*currIndex) {
        if (prevFinal != *prevIndex) {
            def[i].oldPos = *prevIndex + 12;
            def[i++].newPos = prevFinal + 12;

            FREE(block, *prevIndex + 12);
            prevFinal = *(currIndex + 1) ?
                *(currIndex + 1) + *(prevIndex + 2) : 4;
            allocBtw(block, prevIndex, nextIndex,
                     prevFinal, *(currIndex + 2));
            copiarMemoria(block, currIndex, *prevIndex + 12,
                    *(currIndex + 2) - 12);
            currIndex = (int32_t*)(block->mem + *prevIndex);
        }

        prevIndex = currIndex;
        currIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
        prevFinal = *(currIndex + 1) + *(prevIndex + 2);
    }  
    if (prevFinal != *prevIndex && *prevIndex) {
        def[i].oldPos = *prevIndex + 12;
        def[i++].newPos = prevFinal + 12;

        *prevIndex = *(currIndex + 1) ?
            *(currIndex + 1) + *(prevIndex + 2) : 4;
        
        nextIndex = currIndex;

        currIndex = (int32_t*)(block->mem + *prevIndex);
        *currIndex = *nextIndex;
        *(currIndex + 1) = *(nextIndex + 1);
        *(currIndex + 2) = *(nextIndex + 2);
        copiarMemoria(block, nextIndex, *prevIndex + 12, *(currIndex + 2) - 12);
    }  
}

void safe_fill(arena_t *block, int32_t pos, int32_t  size, int32_t value) {
    int32_t *currIndex;
    uchar_t *byte, *end;
    int32_t startPos = verificarPos(block, pos), finPos;

    if (!startPos) {
        printf("[-]¡Acceso no válido a la memoria!\n");
    } else {
        currIndex = (int32_t*)(block->mem + startPos - 12);
        finPos = startPos + *(currIndex + 2) - 13 < pos + size ?
              startPos + *(currIndex + 2) - 13 : pos + size - 1;
        end = block->mem + finPos;
        

        for (byte = block->mem + pos; byte <= end; ++byte) {
            *byte = (uchar_t)value;
        }  
        printf("[+] %d byte(s) modificados\n", finPos - pos + 1);
    }
}

int32_t verificarPos(arena_t *block, int32_t pos) {
    int32_t *prevIndex = (int32_t*)(block->mem);
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);

    if (block->len <= pos || pos < 4) {
        return 0;
    }  
    while (*prevIndex && pos > *prevIndex) {
        if (pos >= *prevIndex + 12 && pos < *prevIndex + *(nextIndex + 2)) {
            return (*prevIndex + 12);
        }

        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
    }  

    return 0;
    
}

void inicializar(arena_t *block) {
    block->mem = (uchar_t*)calloc(block->len, sizeof(uchar_t));
}  

void finalizar(arena_t *block) {
    free(block->mem);
} 
