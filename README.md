# Memory-Allocator

Un alocator de memorie simplificat, ce implementeaza urmatoarele functii

## INITIALIZE N

- prima comanda apelata;

- aloca `N` octeti care reprezinta zona de memorie cu care se va lucra.

## FINALIZE

- ultima comanda apelata;

- dealoca acei `N` octeti alocati de `INITIALIZE`.

## DUMP

- afiseaza octetii hartii in format hexazecimal;

- similar cu hexdump.


## ALLOC SIZE

- aloca `SIZE` octeti pe prima pozitie de la stanga la dreapta din arena in care acest lucru este posibil;

- returneaza adresa de inceput a zonei alocate.

## FREE INDEX

- elibereaza memoria ce incepe la pozitia `INDEX`;

- `INDEX` este o pozitie returnata de `ALLOC`.

## FILL INDEX SIZE VALUE

- seteaza `SIZE` octeti incepand cu pozitia `INDEX` la valoarea `VALUE`;

- similar cu un `memset`.

## SHOW INFO

- ofera statistici despre starea memoriei;

- **INFO** poate fi:

	- **FREE**: numarul de octeti nealocati si numarul de zone continue libere din memorie;
	
	- **USAGE**: numarul de octeti folositi din arena, eficienta si fragmentarea alocarii;
	
	- **ALLOCATIONS**: afiseaza pentru fiecare zona, daca este libera sau alocata impreuna cu dimensiunea acestora;
	
	- **MAP LENGTH**: afiseaza un sir de `LENGTH` caractere, care ilustreaza memoria gestionata: `*` reprezinta o zona alocata, iar `.` una nealocata.
	
## ALLOCALIGNED SIZE ALIGN

- face acelasi lucru ca `ALLOC`, dar aloca doar la un index care este multiplu de `ALIGN`, unde `ALIGN` este o putere a lui 2.

## REALLOC INDEX SIZE

- realoca zona de memorie de la pozitia `INDEX` cu noua dimensiune de `SIZE`;

- pozitia in care se face realocarea este prima de la stanga la dreapta in care incap `SIZE` octeti.

## DEFRAG

- alipeste toate zonele alocate la stanga, astfel incat, dupa rularea comenzii, fragmentarea scade la *0%*;

- returneaza un vector care face legatura dintre vechii indecsi ai zonelor de memorie si cei noi.

## SAFE_FILL INDEX SIZE VALUE

- functia `FILL` risca sa suprascrie octeti unde nu ar trebuis sa aiba acces;

- functia `SAFE_FILL` verifica sa se scrie la un `INDEX` valid (alocat) si un numar de octeti care nu depaseste zona in care se face scrierea.
