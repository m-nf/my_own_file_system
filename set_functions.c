/**
 * @file set_functions.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funkce s funkcionalitou typu set.
 * @version 1.0
 * @date 2022-02-
 */

#include "definitions.h"

/**
 * @brief Uvolni datove bloky souboru nad 5000 bytu.
 * 
 * @param ind Uzel, u ktereho chceme bloky uvolnit.
 */
void set_free_over_5000B(inode *ind) {
    int32_t number_of_blocks = 0;    
    int32_t i = 0;
    int32_t block_address = 0;
    int32_t num_of_block = 0;
    
    number_of_blocks = (ind->file_size / SIZE_OF_CLUSTER) + 1; // chceme zapsat i ten zbytek

    for (i = 0; i < number_of_blocks; i++) {
        block_address = 0;
        num_of_block = i;
    
        if (num_of_block > 4) { // indirect

            num_of_block -= 5;
            if (num_of_block >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect !!!

                int32_t number_of_indirects1 = num_of_block / (SIZE_OF_CLUSTER / sizeof(int32_t)); // cteme odkaz prvnih ostupne
                number_of_indirects1 -= 1;
                fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
                fread(&block_address, sizeof(int32_t), 1, fs);                      

                int32_t number_of_indirects2 = num_of_block % (SIZE_OF_CLUSTER / sizeof(int32_t));  //cteme odkzy druheho stupne
                //number_of_indirects2 -= 1;                                                             // ?????????????  number_of_indirects-1 ????????????
                fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
                fread(&block_address, sizeof(int32_t), 1, fs);

                set_bit_bitmap_data(ind->indirect2, 0);         
            }
            else { // indirect 1            
                fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
                fread(&block_address, sizeof(int32_t), 1, fs);

                set_bit_bitmap_data(ind->indirect1, 0);               
            }
        }
        else { // direct
            if (num_of_block == 0) { // direct 1            
                return;
            }
            else if (num_of_block == 1) { // direct 2
                return;
            }
            else if (num_of_block == 2) {  // direct 3
                block_address = ind->direct3;
            }                    
            else if (num_of_block == 3) { // direct 4
                block_address = ind->direct4;
            }                    
            else { // direct 5
                block_address = ind->direct5;
            }
        }
        set_bit_bitmap_data(block_address, 0);  
    }
}

/**
 * @brief Uvolni datove bloky souboru.
 * 
 * @param ind Uzel, u ktereho chceme bloky uvolnit.
 */
void set_free_blocks_of_file(inode *ind) {    
    int32_t number_of_blocks = 0;    
    int32_t i = 0;
    int32_t block_address = 0;
    int32_t num_of_block = 0;
    
    number_of_blocks = (ind->file_size / SIZE_OF_CLUSTER) + 1; // chceme zapsat i ten zbytek

    for (i = 0; i < number_of_blocks; i++) {
        block_address = 0;
        num_of_block = i;
    
        if (num_of_block > 4) { // indirect

            num_of_block -= 5;
            if (num_of_block >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect !!!

                int32_t number_of_indirects1 = num_of_block / (SIZE_OF_CLUSTER / sizeof(int32_t)); // cteme odkaz prvnih ostupne
                number_of_indirects1 -= 1;
                fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
                fread(&block_address, sizeof(int32_t), 1, fs);                      

                int32_t number_of_indirects2 = num_of_block % (SIZE_OF_CLUSTER / sizeof(int32_t));  //cteme odkzy druheho stupne
                //number_of_indirects2 -= 1;                                                             // ?????????????  number_of_indirects-1 ????????????
                fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
                fread(&block_address, sizeof(int32_t), 1, fs);

                set_bit_bitmap_data(ind->indirect2, 0);         
            }
            else { // indirect 1            
                fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
                fread(&block_address, sizeof(int32_t), 1, fs);

                set_bit_bitmap_data(ind->indirect1, 0);               
            }
        }
        else { // direct
            if (num_of_block == 0) { // direct 1            
                block_address = ind->direct1;  
            }
            else if (num_of_block == 1) { // direct 2
                block_address = ind->direct2;
            }
            else if (num_of_block == 2) {  // direct 3
                block_address = ind->direct3;
            }                    
            else if (num_of_block == 3) { // direct 4
                block_address = ind->direct4;
            }                    
            else { // direct 5
                block_address = ind->direct5;
            }
        }
        set_bit_bitmap_data(block_address, 0);  
    }   
}

/**
 * @brief Nastavi aktualni cestu.
 * 
 * @param path Cesta, kterou zadal uzivatel.
 */
void set_actual_path(char *path) {
    if (path[0] == '/') { // absolutni cesta        
        sprintf(actual_path, "%s", path);
    }
    else {
        int32_t num_of_splits = 0;        
        char *str_cpy = create_string_copy(path);
        char **splitted_path = str_split(str_cpy, '/', &num_of_splits);
                  
        int32_t path_lenght = 0;
        while (path[path_lenght] != '\0') {
            path_lenght++;
        }        
        path_lenght -= 1;

        char *parameter_path = (char *) malloc(sizeof(char) * path_lenght);
        strcpy(parameter_path, actual_path);
        if (strcmp(actual_path, "/") == 0) {
            sprintf(actual_path, "%s%s", parameter_path, splitted_path[num_of_splits - 1]);
        }
        else {
            sprintf(actual_path, "%s/%s", parameter_path, splitted_path[num_of_splits - 1]);
        }
        
        free(parameter_path);
        free(splitted_path);
        free(str_cpy);
    }
}

/**
 * @brief Zmeni aktualni uzel v pameti na uzel zadanej parametrem.
 * 
 * @param node_address adresa uzlu, na ktery se aktualni uzel ma nastavit
 */
void set_actual_inode(int32_t node_address) {
    fseek(fs, node_address, SEEK_SET);    
    fread(&actual_ind->nodeid, sizeof(int32_t), 1, fs);
    fread(&actual_ind->isDirectory, sizeof(bool), 1, fs);
    fread(&actual_ind->parent_directory_address, sizeof(int32_t), 1, fs);
    fread(&actual_ind->file_size, sizeof(int32_t), 1, fs);
    fread(&actual_ind->direct1, sizeof(int32_t), 1, fs);
    fread(&actual_ind->direct2, sizeof(int32_t), 1, fs);
    fread(&actual_ind->direct3, sizeof(int32_t), 1, fs);
    fread(&actual_ind->direct4, sizeof(int32_t), 1, fs);
    fread(&actual_ind->direct5, sizeof(int32_t), 1, fs);
    fread(&actual_ind->indirect1, sizeof(int32_t), 1, fs);
    fread(&actual_ind->indirect2, sizeof(int32_t), 1, fs);
}

/**
 * @brief Nastavi bit v bitmape inodu.
 * 
 * @param node_address adresa uzlu, u ktereho se ma bit nastavit
 * @param isTaken bit, ktery se ma nastavit 0 - volno
 *                                          1 - obsazeno
 */
void set_bit_bitmap_inode(int32_t node_address, char isTaken) {
    int32_t bit_address = sp->bitmapinode_start_address
                            + (node_address - sp->inode_start_address) / sizeof(inode); // o kolikaty uzel, resp. bit se jedna
    
    fseek(fs, bit_address, SEEK_SET);
    fwrite(&isTaken, sizeof(char), 1, fs);
}

/**
 * @brief Nastavi bit v bitmape datovych bloku.
 * 
 * @param datablock_address adresa bloku, u ktereho se ma bit nastavit
 * @param isTaken bit, ktery se ma nastavit 0 - volno
 *                                          1 - obsazeno
 */
void set_bit_bitmap_data(int32_t datablock_address, char isTaken) {
    int32_t bit_address = sp->bitmapdata_start_address
                            + (datablock_address - sp->data_start_address) / SIZE_OF_CLUSTER; // o kolikaty uzel, resp. bit se jedna
    
    fseek(fs, bit_address, SEEK_SET);
    fwrite(&isTaken, sizeof(char), 1, fs);

    //printf("set bit data: zapisuju datovemu bloku: %d, bit: %d\n", datablock_address, isTaken);
    
}