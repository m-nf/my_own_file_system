/**
 * @file definitions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje definice struktur, globálních proměnných, konstant. Jsou zde inkludovany vsechny ostatni hlavickove soubory.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef main_h
#define main_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#define ARGUMENTS_COUNT 2        // pocet argumentu pri spusteni programu
#define MAX_SIZE 2147483647     // in bytes
#define SIZE_OF_CLUSTER 4096    // in bytes, velikost datoveho bloku
#define MB_TO_B 1000000         // konstanta pro prevod MB na B
#define SIZE_OF_DESCRIPTION 252 // velikost popisu superbloku
#define NAME_LENGHT 12          // delka jmena souboru, nebo slozky
#define SIGNATURE_LENGHT 9      // delka podpisu superbloku
#define ACTUAL_PATH_LENGHT 500  // nejvetsi mozna velikost cestz pro vypsani
#define ROW_LENGHT 100          // pocatecni velikost bufferu, pri nacitani prikazu ze souboru


//const int32_t ID_ITEM_FREE = 0;
typedef enum {false = 0, true = !false} bool;


typedef struct the_superblock {
    char signature[SIGNATURE_LENGHT];              //login autora FS
    char volume_descriptor[SIZE_OF_DESCRIPTION];    //popis vygenerovaného FS
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru
    int32_t bitmapinode_start_address;  //adresa pocatku bitmapy i-uzlů
    int32_t bitmapdata_start_address;   //adresa pocatku bitmapy datových bloků
    int32_t inode_start_address;    //adresa pocatku  i-uzlů
    int32_t data_start_address;     //adresa pocatku datovych bloku  
} superblock;

typedef struct pseudo_inode {
    int32_t nodeid;                 //ID i-uzlu, adresa,  pokud ID = ID_ITEM_FREE, je polozka volna
    bool isDirectory;               //soubor, nebo adresar
    int32_t parent_directory_address;              //počet odkazů na i-uzel, používá se pro hardlinky
    int32_t file_size;              //velikost souboru v bytech
    int32_t direct1;                // 1. přímý odkaz na datové bloky -- first is parent directory in datas
    int32_t direct2;                // 2. přímý odkaz na datové bloky
    int32_t direct3;                // 3. přímý odkaz na datové bloky
    int32_t direct4;                // 4. přímý odkaz na datové bloky
    int32_t direct5;                // 5. přímý odkaz na datové bloky
    int32_t indirect1;              // 1. nepřímý odkaz (odkaz - datové bloky)
    int32_t indirect2;              // 2. nepřímý odkaz (odkaz - odkaz - datové bloky)
} inode;

typedef struct the_directory_item {
    int32_t inode;                   // inode odpovídající souboru
    char item_name[NAME_LENGHT];              //8+3 + /0 C/C++ ukoncovaci string znak
} directory_item;

/* global variable - file with file system */
extern FILE *fs;                    // ukazatel na soubor s fs
extern superblock *sp;              // ukazatel na strukturu superblock
extern inode *actual_ind;           // zde je zaznamenany uzel (adresar), kde se prave nachazime
extern directory_item *dir_it_tmp;  // ukazatel na strukturu directory item, slouzi pro predani parametru pouze v pripade zapisovani nove polozky adresare
extern char *actual_path;           // aktualni cesta


#include "input_handler.h"
#include "commands_functions.h"
#include "other_tools.h"
#include "create_functions.h"
#include "set_functions.h"
#include "get_functions.h"
#include "read_and_write_functions.h"

#endif