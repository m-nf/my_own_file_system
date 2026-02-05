/**
 * @file main.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funcki main a funkce pro pripravu a ukonceni programu.
 * @version 1.0
 * @date 2022-02-11
 */

#include "definitions.h"

FILE *fs;
superblock *sp;
inode *actual_ind;
directory_item *dir_it_tmp;
char *actual_path;

/**
 * @brief Uklidi po programu. Uvolni pamet. Zavre soubor s fs.
 * 
 */
void cleanup() {
    fclose(fs);

    free(sp);
    free(actual_ind);
    free(dir_it_tmp);
    free(actual_path);   
}

/**
 * @brief Nacte superblock z fs do pameti do struktury superblock.
 * 
 */
void prepare_superblock() {
    fseek(fs, 0, SEEK_SET);
    fread(sp->signature, sizeof(char), SIGNATURE_LENGHT, fs);
    fread(sp->volume_descriptor, sizeof(char), SIZE_OF_DESCRIPTION, fs);
    fread(&sp->disk_size, sizeof(int32_t), 1, fs);
    fread(&sp->cluster_size, sizeof(int32_t), 1, fs);
    fread(&sp->cluster_count, sizeof(int32_t), 1, fs);
    fread(&sp->bitmapinode_start_address, sizeof(int32_t), 1, fs);
    fread(&sp->bitmapdata_start_address, sizeof(int32_t), 1, fs);
    fread(&sp->inode_start_address, sizeof(int32_t), 1, fs);
    fread(&sp->data_start_address, sizeof(int32_t), 1, fs);

    printf("%s", sp->volume_descriptor);
}

/**
 * @brief Otevre, ci vytvori soubor s fs. Alokuje pamet u globanich promennych.
 * 
 * @param arg Argument s nazvem souboru, kde ma byt fs.
 */
void prepare(char *arg) {
    sp = (superblock *) malloc(sizeof(superblock));   

    fs = fopen(arg, "rb+");

    actual_ind = (inode *) malloc(sizeof(inode));
    dir_it_tmp = (directory_item  *) malloc(sizeof(directory_item));
    actual_path = (char *) malloc(sizeof(char) * ACTUAL_PATH_LENGHT);       

    if (fs != NULL) { // soubor jiz existuje, nacteme data
        prepare_superblock();
        set_actual_inode(sp->inode_start_address);
        memset(actual_path, '\0', sizeof(char) * ACTUAL_PATH_LENGHT);
        actual_path[0] = '/';
    }    
    else { // soubor neexistuje, vytvorime ho.
        fs = fopen(arg, "wb+");

        if (fs == NULL) { // neco se pri vytvareni pokazilo, ukoncujeme program
            printf("Input file loading ERROR\n");
            free(sp);
            free(actual_ind);
            free(dir_it_tmp);
            free(actual_path);           
            exit(EXIT_FAILURE);
        }     
    }   
}

/**
 * @brief Hlavni funkce programu.
 * 
 * @param argc Pocet argumentu.
 * @param argv Argumenty.
 * @return int 
 */
int main(int argc, char *argv[]) {
    if (argc != ARGUMENTS_COUNT) {
        printf("Wrong number of arguments.");
        return EXIT_FAILURE;
    }
    else {
        prepare(argv[1]);
        listening(argv[1]);              
        cleanup();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}