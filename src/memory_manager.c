

/*
Escuela Superior Politécnica del Litoral (ESPOL)
Author:Joseph Avila 
*/
#include "../include/allocator.h"

int32_t asignar(block_t *block, int32_t size) {
    int32_t *anteriorIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *anteriorIndex);
    int32_t prevFinal = 4;

    size += 12;

    if (size > block->len - 4) {
        return 0;
    }  
    if (!*anteriorIndex) {
        *anteriorIndex = 4;
        *(anteriorIndex + 1) = 0;
        *(anteriorIndex + 2) = 0;
        *(anteriorIndex + 3) = size;
        return (*anteriorIndex + 12);
    } 

    while (*siguienteIndex && *anteriorIndex - prevFinal < size) {
        anteriorIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
        prevFinal = *(anteriorIndex + 2) + *(siguienteIndex + 1);
    }  

    if (*anteriorIndex - prevFinal >= size) {
        asignarBtw(block, anteriorIndex, siguienteIndex, prevFinal, size);
        return (*anteriorIndex + 12);
    }  
    if (block->len - *anteriorIndex - *(siguienteIndex + 2) < size) {
        return 0;
    }  
    prevFinal = *anteriorIndex + *(siguienteIndex + 2);
    asignarLast(block, anteriorIndex, siguienteIndex, prevFinal, size);
    return  (*siguienteIndex + 12);
    
}  

void asignarBtw(block_t *block, int32_t *prev, int32_t *next,
              int32_t start, int32_t size) {
    int32_t *nuevoIndex = (int32_t*)(block->mem + start);

    *nuevoIndex = *prev;
    *(nuevoIndex + 1) = *(next + 1);
    *(nuevoIndex + 2) = size;
    *prev = start;
    *(next + 1) = *prev;
}

void asignarLast(block_t *block, int32_t *anterior, int32_t *siguiente,
               int32_t inicio, int32_t size) {
    int32_t *nuevoIndex = (int32_t*)(block->mem + inicio);

    *siguiente = inicio;
    *nuevoIndex = 0;
    *(nuevoIndex + 1) = *anterior;
    *(nuevoIndex + 2) = size;
}

void volcado(block_t *block) {
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

void FREE(block_t *block, int32_t pos) {
    int32_t *currentIndex = (int32_t*)(block->mem + pos - 12);
    int32_t *siguienteIndex = (int32_t*)(block->mem + *currentIndex);
    int32_t *prevIndex = (int32_t*)(block->mem + *(currentIndex + 1));

    if (*currentIndex) {  
        *prevIndex = *currentIndex;
        *(siguienteIndex + 1) = *(currentIndex + 1);
    } else {
        *prevIndex = 0;
    }  
    block->liberaciones++;
}

void llenar(block_t *block, int32_t pos, int32_t  size, int32_t value) {
    uchar_t *byte = block->mem + pos;
    uchar_t *end = byte + size - 1;

    for (; byte <= end; ++byte) {
        *byte = (uchar_t)value;
    }  
}

void mostrar(block_t *block, char *arg) {
    const char *delimitador = " \n";
    int32_t tamanio;

    if (!strcmp(arg, "FREE")) {
        mostrar_free(block);
    } else if (!strcmp(arg, "USO")) {
         mostrar_usage(block);
    } else if (!strcmp(arg, "ASIGNACIONES")) {
        mostrar_alloc(block);
    } else {
        tamanio = atoi(arg = strtok(NULL, delimitador));
        mostrar_map(block, tamanio);
    }
}

void mostrar_free(block_t *block) {
    int32_t *anteriorIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *anteriorIndex);
    int32_t prevFinal = 4, freeBytes = block->len - 4, freeBlocks = 1;

    while (*anteriorIndex) {
        freeBytes -= *(siguienteIndex + 2);

        if (*anteriorIndex - prevFinal > 0) {
            ++freeBlocks;
        }

        if (*anteriorIndex + *(siguienteIndex + 2) == block->len) {
            --freeBlocks;
        }  
        anteriorIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
        prevFinal = *(anteriorIndex + 2) + *(siguienteIndex + 1);
    }  

    printf("%d blocks (%d bytes) free\n", freeBlocks, freeBytes);
    printf("%d Liberaciones Totales\n",block->liberaciones);
}

void mostrar_usage(block_t *block) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t efficiencia, fragmentacion, prevFinal = 4;
    int32_t libresBytes = block->len - 4, libresBlocks = 1;
    int32_t usadosBytes = 0, usadosdBlocks = 0;

    while (*prevIndex) {
        usadosBytes += (*(siguienteIndex + 2) - 12);
        ++usadosdBlocks;
        libresBytes -= *(siguienteIndex + 2);

        if (*prevIndex - prevFinal > 0) {
            ++libresBlocks;
        } 

        if (*prevIndex + *(siguienteIndex + 2) == block->len) {
            --libresBlocks;
        }  
        prevIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
        prevFinal = *(prevIndex + 2) + *(siguienteIndex + 1);
    }  
    printf("%d blocks (%d bytes) usados\n", usadosdBlocks, usadosBytes);

    efficiencia = 100 * usadosBytes / (block->len - libresBytes);
    printf("%d%% Eficiencia ", efficiencia);

    fragmentacion = usadosdBlocks ? 100 * (libresBlocks - 1) / usadosdBlocks : 0;
    printf("%d%% Fragmentacion\n", fragmentacion);
}

void mostrar_alloc(block_t *block) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t freeBytes, prevFinal = 4;

    printf("Ocupados 4 bytes\n");  

    while (*prevIndex) {
        freeBytes = *prevIndex - prevFinal;

        if (freeBytes) {
            printf("Libres%d bytes\n", freeBytes);
        }

        printf("OCupados %d bytes\n", *(siguienteIndex + 2));

        prevFinal = *(siguienteIndex + 2) + *prevIndex;
        prevIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
    }  

    if (block->len - prevFinal) {
        printf("Libres %d bytes\n", block->len - prevFinal);
    }  
}

void mostrar_map(block_t *block, int32_t tamanio) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t toPrint, prevFinal = 4, contador = 1;
    double x = (double)block->len / (double)tamanio;
    double bytes = 4, extra;

    toPrint = (int32_t)(bytes / x);
    extra = x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
    printMemoria(toPrint, &contador, '*');

    while (*prevIndex) {
        bytes = (double)(*prevIndex - prevFinal) - extra;
        toPrint = (int32_t)(bytes / x);
        extra = bytes - x * (double)toPrint;
        printMemoria(toPrint, &contador, '.');
    
        bytes = (double)*(siguienteIndex + 2) + extra;
        if (bytes > 0) {
            toPrint = (int32_t)(bytes / x);
            extra =
            x * (double)toPrint == bytes ? 0 : x * (double)(++toPrint) - bytes;
            printMemoria(toPrint, &contador, '*');
        } else {
            extra = -bytes;
        }  

        prevFinal = *(siguienteIndex + 2) + *prevIndex;
        prevIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
    } 
    toPrint = (int32_t)(((double)(block->len - prevFinal) - extra) / x);
    printMemoria(toPrint, &contador, '.');
   

    if (contador % 80) {
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

int32_t ALLOCALIGNED(block_t *block, int32_t size, int32_t align) {
    int32_t *prevIndex = (int32_t*)block->mem;
    int32_t *siguienteIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t *nuevoIndex;
    int32_t prevFinal = 4, pos = align < 16 ? 16 : align;

    if (!*prevIndex) {
        if (pos + size - 1 > block->len) {
            return 0;  
        }

        nuevoIndex = (int*)(block->mem + pos - 12);
        *nuevoIndex = 0;
        *(nuevoIndex + 1) = 0;
        *(nuevoIndex + 2) = size + 12;
        *prevIndex = pos - 12;
        return pos;  
    }

    while (*siguienteIndex) {
        pos = buscarPos(pos, align, size, prevFinal, *prevIndex);

        if (pos - 12 >= prevFinal && pos + size - 1 < *prevIndex) {
            asignarBtw(block, prevIndex, siguienteIndex, pos - 12, size + 12);
            return pos;
        }  

        prevIndex = siguienteIndex;
        siguienteIndex = (int*)(block->mem + *siguienteIndex);
        prevFinal = *(siguienteIndex + 1) + *(prevIndex + 2);
    }  
    pos = buscarPos(pos, align, size, prevFinal, *prevIndex);

    if (pos - 12 >= prevFinal && pos + size <= *prevIndex) {
        asignarBtw(block, prevIndex, siguienteIndex, pos - 12, size + 12);
        return pos;
    }  

    prevFinal = *prevIndex + *(siguienteIndex + 2);
    pos = buscarPos(pos, align, size, prevFinal, block->len);

    if (pos + size > block->len) {
        return 0;
    }  

    asignarLast(block, prevIndex, siguienteIndex, pos - 12, size + 12);
    return pos;  
}  

int32_t buscarPos(int32_t posicion, int32_t aliniacion, int32_t size,
                int32_t prevFinal, int32_t siguienteFirst) {
    while (posicion < prevFinal) {
        posicion <<= 1;
    }

    if (posicion > aliniacion) {
        posicion >>= 1;
    }

    while (posicion - 12 < prevFinal && posicion + size <= siguienteFirst) {
        posicion += aliniacion;
    }

    return posicion;
    
}

int32_t realloc_(block_t *block, int32_t pos, int32_t size) {
    int32_t *currentIndex = (int32_t*)(block->mem + pos - 12);
    int32_t *siguienteIndex = (int32_t*)(block->mem + *currentIndex);
    int32_t *prevIndex = (int32_t*)(block->mem + *(currentIndex + 1));
    int32_t nuevaPos;

    FREE(block, pos);

    if (!(nuevaPos = asignar(block, size))) {
        *prevIndex = pos - 12;
        if (*currentIndex) {
            *(siguienteIndex + 1) = pos - 12;
        }  
        return 0;
    }  
    copiarMemoria(block, currentIndex, nuevaPos, size);

    return nuevaPos;
}

void copiarMemoria(block_t *block, int32_t *currentIndex, int nuevaPos, int memTamanio) {
    uchar_t *endB, *endS, *byte, *startPos;

    byte = (uchar_t*)currentIndex + 12;
    endB = byte + *(currentIndex + 2) - 13;  
    startPos = block->mem + nuevaPos;
    endS = startPos + memTamanio - 1;  

    for (; startPos <= endS && byte <= endB; ++startPos, ++byte) {
        *startPos = *byte;
    }  
}

void defragmentar(block_t *block, defrag_t *def) {
    int32_t *prevIndex = (int32_t*)(block->mem);
    int32_t *currentIndex = (int32_t*)(block->mem + *prevIndex);
    int32_t *siguienteIndex = (int32_t*)(block->mem + *currentIndex);
    int32_t prevFinal = 4, i = 0;

    while (*currentIndex) {
        if (prevFinal != *prevIndex) {
            def[i].oldPos = *prevIndex + 12;
            def[i++].newPos = prevFinal + 12;

            FREE(block, *prevIndex + 12);
            prevFinal = *(currentIndex + 1) ?
                *(currentIndex + 1) + *(prevIndex + 2) : 4;
            asignarBtw(block, prevIndex, siguienteIndex,
                     prevFinal, *(currentIndex + 2));
            copiarMemoria(block, currentIndex, *prevIndex + 12,
                    *(currentIndex + 2) - 12);
            currentIndex = (int32_t*)(block->mem + *prevIndex);
        }

        prevIndex = currentIndex;
        currentIndex = siguienteIndex;
        siguienteIndex = (int32_t*)(block->mem + *siguienteIndex);
        prevFinal = *(currentIndex + 1) + *(prevIndex + 2);
    }  
    if (prevFinal != *prevIndex && *prevIndex) {
        def[i].oldPos = *prevIndex + 12;
        def[i++].newPos = prevFinal + 12;

        *prevIndex = *(currentIndex + 1) ?
            *(currentIndex + 1) + *(prevIndex + 2) : 4;
        
        siguienteIndex = currentIndex;

        currentIndex = (int32_t*)(block->mem + *prevIndex);
        *currentIndex = *siguienteIndex;
        *(currentIndex + 1) = *(siguienteIndex + 1);
        *(currentIndex + 2) = *(siguienteIndex + 2);
        copiarMemoria(block, siguienteIndex, *prevIndex + 12, *(currentIndex + 2) - 12);
    }  
}

void safe_fill(block_t *block, int32_t posicion, int32_t  tamanio, int32_t valor) {
    int32_t *currentIndex;
    uchar_t *byte, *final;
    int32_t startPos = verificarPos(block, posicion), finPos;

    if (!startPos) {
        printf("[-]¡Acceso no válido a la memoria!\n");
    } else {
        currentIndex = (int32_t*)(block->mem + startPos - 12);
        finPos = startPos + *(currentIndex + 2) - 13 < posicion + tamanio ?
              startPos + *(currentIndex + 2) - 13 : posicion + tamanio - 1;
        final = block->mem + finPos;
        

        for (byte = block->mem + posicion; byte <= final; ++byte) {
            *byte = (uchar_t)valor;
        }  
        printf("[+] %d byte(s) modificados\n", finPos - posicion + 1);
    }
}

int32_t verificarPos(block_t *block, int32_t posicion) {
    int32_t *prevIndex = (int32_t*)(block->mem);
    int32_t *nextIndex = (int32_t*)(block->mem + *prevIndex);

    if (block->len <= posicion || posicion < 4) {
        return 0;
    }  
    while (*prevIndex && posicion > *prevIndex) {
        if (posicion >= *prevIndex + 12 && posicion < *prevIndex + *(nextIndex + 2)) {
            return (*prevIndex + 12);
        }

        prevIndex = nextIndex;
        nextIndex = (int32_t*)(block->mem + *nextIndex);
    }  

    return 0;
    
}

void inicializar(block_t *block) {
    block->mem = (uchar_t*)calloc(block->len, sizeof(uchar_t));
    block->liberaciones=0;
}  

void finalizar(block_t *block) {
    free(block->mem);
} 
