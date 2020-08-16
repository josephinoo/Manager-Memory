# Memory-Allocator

Un asignador de memoria simplificado que implementa las siguientes funciones

## INITIALIZE N

- el primer comando llamado;

- asignar `N` bytes que representan el área de memoria con la que se trabajará.

## FINALIZE
- último comando llamado;

- Asignar esos `N` bytes asignados por` INICIALIZAR`.

## DUMP
- muestra los bytes del mapa en formato hexadecimal;

- similar a hexdump.


## ALLOC SIZE
- Asignar bytes `SIZE` en la primera posición de izquierda a derecha en la arena donde sea posible;

- devuelve la dirección de inicio del área asignada.

## FREE INDEX

- libera la memoria a partir de la posición `INDICE`;

- `INDEX` es una posición devuelta por` ALLOC`.

## FILL INDEX SIZE VALUE
- establece bytes `SIZE` comenzando con la posición` INDEX` en el valor` VALUE`;

- similar a un `memset`.

## SHOW INFO

- proporciona estadísticas sobre el estado de la memoria;

- ** INFO ** puede ser:

    - ** FREE **: el número de bytes no asignados y el número de zonas continuas libres en la memoria;

    - ** USO **: el número de bytes usados ​​en la arena, la eficiencia y fragmentación de la asignación;

    - ** ASIGNACIONES **: muestra para cada área, si es libre o asignada junto con su tamaño;

    - ** MAP LENGTH **: muestra una cadena de caracteres `LENGTH`, que ilustra la memoria administrada:` * `representa un área asignada y` .` una no asignada.
	
## ALLOCALIGNED SIZE ALIGN
- hace lo mismo que "ALLOC", pero asigna solo a un índice que es un múltiplo de "ALINEAR", donde "ALINEAR" es una potencia de 2.

## REALLOC INDEX SIZE

- reasigne el área de memoria desde la posición `INDEX` con la nueva dimensión` SIZE`;

- la posición en la que se realiza la reasignación es la primera de izquierda a derecha en la que caben los bytes "TAMAÑO".
## DEFRAG

- pega todas las áreas asignadas a la izquierda, de modo que, después de ejecutar el comando, la fragmentación disminuye a * 0% *;

- devuelve un vector que hace la conexión entre los índices antiguos de las áreas de memoria y los nuevos.

## SAFE_FILL INDEX SIZE VALUE

- la función `FILL` corre el riesgo de sobrescribir bytes donde no deberían tener acceso;

- la función `SAFE_FILL` comprueba la escritura en un` INDICE` válido (asignado) y un número de bytes que no exceda el área donde se realiza la escritura.
