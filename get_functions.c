/**
 * @file get_functions.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funkce s funkcionalitou typu get.
 * @version 1.0
 * @date 2022-02-
 */

#include "definitions.h"

/**
 * @brief Vrati adresu na blok, kde lze cist.
 * 
 * @param ind Uzel, ze ktereho chceme cist.
 * @param num_of_block Poradi datoveho bloku, ze ktereho chceme cist, v uzlu.
 * @return Adresa na datovz blok.
 */
int32_t get_block_to_read(inode *ind, int32_t num_of_block) {    
    int32_t block_address = 0;
    
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

            return block_address;
        }
        else { // indirect 1   

            fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);

            return block_address;
        }
    }
    else { // direct
        if (num_of_block == 0) { // direct 1            
            //printf("find block to write: direct1\n");
            return ind->direct1;                 
        }
        else if (num_of_block == 1) { // direct 2
            //printf("find block to write: direct2\n");
            return ind->direct2;
        }
        else if (num_of_block == 2) {  // direct 3
            //printf("find block to write: direct3\n");          
            return ind->direct3;
        }                    
        else if (num_of_block == 3) { // direct 4
            //printf("find block to write: direct4\n"); 
            return ind->direct4;
        }                    
        else { // direct 5
            //printf("find block to write: direct5\n");
            return ind->direct5;
        }
    }
}

/**
 * @brief Vrati adresu datoveho bloku uzlu, kam lze psat.
 * 
 * @param ind Uzel, u ktereho chceme blok ke psani najit.
 * @param file_or_dir == SIZE_OF_CLUSTER, kdyz soubor,
 *                    == sizeof(directory_item), kdyz slozka.
 * @return Adresu datoveho bloku.
 */
int32_t get_block_to_write(inode *ind, int32_t file_or_dir) {
    int32_t number_of_blocks = 0;
    int32_t block_address = 0;    
    int32_t block_address1 = 0;

    number_of_blocks = ind->file_size / file_or_dir;
    if (number_of_blocks > 4) { // indirect

        number_of_blocks -= 5;
        if (number_of_blocks >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect 2
            if (ind->indirect2 == -1) { // zda mame prirazenej indirect2 blok
                assign_direct_data_block(ind, 7);
            }

            int32_t number_of_indirects1 = number_of_blocks / (SIZE_OF_CLUSTER / sizeof(int32_t)); // cteme odkaz prvnih ostupne
            number_of_indirects1 -= 1;
            fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
            fread(&block_address, sizeof(int32_t), 1, fs);
            
            //printf("find_block_to_write1: blockaddress: %d, number_of_indirects1: %d\n", block_address, number_of_indirects1);

            if (block_address == -1) { // v indirect2 bloku neni prirazenej odkazovej blok
                assign_reference_block_to_indirect2(ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)));
                fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
                fread(&block_address, sizeof(int32_t), 1, fs);  
            } 

            //printf("find_block_to_write2: blockaddress: %d\n", block_address);
            
            int32_t number_of_indirects2 = number_of_blocks % (SIZE_OF_CLUSTER / sizeof(int32_t));  //cteme odkzy druheho stupne
            //number_of_indirects2 -= 1;                                                             // ?????????????  number_of_indirects-1 ????????????
            fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
            fread(&block_address1, sizeof(int32_t), 1, fs);

            //printf("find_block_to_write3: blockaddress: %d, number_of_indirects2: %d\n", block_address, number_of_indirects2);
            if (block_address1 == -1) { // v odkazovym bloku neni prirazenej datovej blok
                assign_data_block_to_indirect1(block_address + (number_of_indirects2 * sizeof(int32_t)));
                fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
                fread(&block_address, sizeof(int32_t), 1, fs);
            }     

            return block_address;
        }
        else { // indirect 1
            if (ind->indirect1 == -1) { // zda mame prirazenej indirect1 blok
                assign_direct_data_block(ind, 6);
            }

            fseek(fs, ind->indirect1 + (number_of_blocks * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);

            if (block_address == -1) { // odkaz v indirect bloku neni prirazenej datovej blok
                assign_data_block_to_indirect1(ind->indirect1 + (number_of_blocks * sizeof(int32_t)));
            } 

            fseek(fs, ind->indirect1 + (number_of_blocks * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);

            return block_address;
        }
    }
    else { // direct
        if (number_of_blocks == 0) { // direct 1            
            if (ind->direct1 == -1) {
                //printf("find block to write: direct1\n");
                assign_direct_data_block(ind, 1);
            }
            return ind->direct1;                 
        }
        else if (number_of_blocks == 1) { // direct 2
            if (ind->direct2 == -1) {
                //printf("find block to write: direct2\n");
                assign_direct_data_block(ind, 2);
            }           
            return ind->direct2;
        }
        else if (number_of_blocks == 2) {  // direct 3
            if (ind->direct3 == -1) {
                assign_direct_data_block(ind, 3);
                //printf("find block to write: direct3\n");
            }
            return ind->direct3;
        }                    
        else if (number_of_blocks == 3) { // direct 4
            if (ind->direct4 == -1) {
                //printf("find block to write: direct4\n");
                assign_direct_data_block(ind, 4);
            }
            return ind->direct4;
        }                    
        else { // direct 5
            if (ind->direct5 == -1) {
                //printf("find block to write: direct5\n");
               assign_direct_data_block(ind, 5);                
            }
            return ind->direct5;
        }
    }
}

/**
 * @brief Vrati jmeno posledni polozky v adresari.
 * 
 * @param ind Adresar, ve kterem hledame.
 * @return char* Jmeno posledni polozky.
 */
char *get_name_of_last_item(inode *ind) {
    int32_t block_address = 0;
    int32_t num_of_block = ind->file_size / sizeof(directory_item);
    char *name = (char *) malloc(sizeof(char) * NAME_LENGHT);
    
    if (num_of_block > 5) { // indirect

        num_of_block -= 6;
        if (num_of_block >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect !!!

            int32_t number_of_indirects1 = num_of_block / (SIZE_OF_CLUSTER / sizeof(int32_t)); // cteme odkaz prvnih ostupne
            number_of_indirects1 -= 1;
            fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
            fread(&block_address, sizeof(int32_t), 1, fs);                      

            int32_t number_of_indirects2 = num_of_block % (SIZE_OF_CLUSTER / sizeof(int32_t));  //cteme odkzy druheho stupne
            //number_of_indirects2 -= 1;                                                             // ?????????????  number_of_indirects-1 ????????????
            fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);              

            fseek(fs, block_address + sizeof(int32_t), SEEK_SET); // cteme uz jmeno z datovyho bloku
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }
        else { // indirect 1   

            fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);

            fseek(fs, block_address + sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }
    }
    else { // direct
        if (num_of_block == 1) { // direct 1            
            fseek(fs, ind->direct1 + sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }
        else if (num_of_block == 2) { // direct 2
            fseek(fs, ind->direct2 + sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }
        else if (num_of_block == 3) {  // direct 3
            fseek(fs, ind->direct3 + sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }                    
        else if (num_of_block == 4) { // direct 4
            fseek(fs, ind->direct4 + sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }                    
        else { // direct 5
            fseek(fs, ind->direct5+ sizeof(int32_t), SEEK_SET);
            fread(name, sizeof(char), NAME_LENGHT, fs);
        }
    }

    return name;
}

/**
 * @brief Vrati adresu posledniho datoveho bloku uzlu.
 * 
 * @param ind Uzel u ktereho chceme zjisit ten posledni dat. blok.
 * @param file_or_dir Zda se jedna o slozku nebo soubor,
 *                    SIZE_OF_CLUSTER - slozka,
 *                    sizeof(directory_item) - soubor.
 * @return int32_t 
 */
int32_t get_last_data_block(inode *ind, int32_t file_or_dir) {
    int32_t block_address = 0;
    int32_t num_of_block = ind->file_size / file_or_dir;
    
    if (num_of_block > 5) { // indirect

        num_of_block -= 6;
        if (num_of_block >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect !!!

            int32_t number_of_indirects1 = num_of_block / (SIZE_OF_CLUSTER / sizeof(int32_t)); // cteme odkaz prvnih ostupne
            number_of_indirects1 -= 1;
            fseek(fs, ind->indirect2 + (number_of_indirects1 * sizeof(int32_t)), SEEK_SET); 
            fread(&block_address, sizeof(int32_t), 1, fs);                      

            int32_t number_of_indirects2 = num_of_block % (SIZE_OF_CLUSTER / sizeof(int32_t));  //cteme odkzy druheho stupne
            //number_of_indirects2 -= 1;                                                             // ?????????????  number_of_indirects-1 ????????????
            fseek(fs, block_address + (number_of_indirects2 * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);              

            return block_address;
        }
        else { // indirect 1   

            fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
            fread(&block_address, sizeof(int32_t), 1, fs);

            return block_address;
        }
    }
    else { // direct
        if (num_of_block == 1) { // direct 1            
            //printf("find block to write: direct1\n");
            return ind->direct1;                 
        }
        else if (num_of_block == 2) { // direct 2
            //printf("find block to write: direct2\n");
            return ind->direct2;
        }
        else if (num_of_block == 3) {  // direct 3
            //printf("find block to write: direct3\n");          
            return ind->direct3;
        }                    
        else if (num_of_block == 4) { // direct 4
            //printf("find block to write: direct4\n"); 
            return ind->direct4;
        }                    
        else { // direct 5
            //printf("find block to write: direct5\n");
            return ind->direct5;
        }
    }
}

/**
 * @brief Vrati adresu posledni polozky cesty. (Parse path vraci predposleni polozku cesty.)
 * 
 * @param path 
 * @return -1 kdyz cesta neexistuje,
 *         adresu polozky, kdyz cesta existuje.
 */
int32_t get_last_item_of_path(char *path) {
    int32_t num_of_splits = 0;
    inode *ind;
    int32_t last_item_address = 0, parent_dir_address = 0;

    char *str_cpy = create_string_copy(path); 
    char **splitted_path = str_split(str_cpy, '/', &num_of_splits); // parsujeme cestu

    parent_dir_address = parse_path(path); // ziskame rodicovksy adresar
    
    if (parent_dir_address == -1) {
        free(str_cpy);
        free(splitted_path);
        return parent_dir_address;
    }

    ind = get_inode(parent_dir_address);

    if (strcmp(path, "/") == 0) { // chceme adresu adresare, ze ktereho budeme vypisovat
        last_item_address = get_item_from_directory(ind, "/");
    }
    else {
        last_item_address = get_item_from_directory(ind, splitted_path[num_of_splits - 1]);
    }

    free(ind);
    free(str_cpy);
    free(splitted_path);

    return last_item_address;
}

/**
 * @brief Vrati adresu polozky, kdyz ji obsahuje.
 * 
 * @param data_block_addr Blok, ve kterem hledame.
 * @param item Polozk, kterou hledame.
 * @return -1 kdyz blok polozku neobsahuje
 *          adresu polozky, kdyz adresu blok obsahuje.
 */
int32_t get_item_from_datablock(int32_t data_block_addr, char *item) {
    char *name = (char *) malloc(sizeof(char) * NAME_LENGHT);
    int32_t ind_addr = -1;

    fseek(fs, data_block_addr + sizeof(int32_t), SEEK_SET);
    fread(name, sizeof(char), NAME_LENGHT, fs);

    //printf("name: %s item: %s\n", name, item);
    if (strcmp(name, item) == 0) {
        fseek(fs, data_block_addr, SEEK_SET);
        fread(&ind_addr, sizeof(int32_t), 1, fs);
    }

    free(name);
    //printf("get item from datablock: addr: %d\n", ind_addr);
    return ind_addr;
}

/**
 * @brief Prohleda adresar, hleda item.
 * 
 * @param ind Adresar k proheldani
 * @param item Polozka kterou hledame
 * @return int32_t  -1, kdyz nenalezeno
 *                  adresu polozku, kdyz nalezeno
 */
int32_t get_item_from_directory(inode *ind, char *item) {
    int32_t return_value = -1;    
    int32_t i = 0;
    int32_t block_address1 = 0, block_address2 = 0;
    int32_t number_of_blocks = ind->file_size / sizeof(directory_item);

    for (i = 1; i <= number_of_blocks; i++) { // prohledavame po jednom datove bloky uzlu, do file_size                  
        if (i < 6) { // prohledavame direct
            if (i == 1) { // direct 1
                return_value = get_item_from_datablock(ind->direct1, item);
                //printf("get item from directory: direct1: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 2) { // direct 2
                return_value = get_item_from_datablock(ind->direct2, item);
                //printf("get item from directory: direct2: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                   
            else if (i == 3) {  // direct 3
                return_value = get_item_from_datablock(ind->direct3, item);
                //printf("get item from directory: direct3: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 4) { // direct 4
                return_value = get_item_from_datablock(ind->direct4, item);
                //printf("get item from directory: direct4: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 5) { // direct 5
                return_value = get_item_from_datablock(ind->direct5, item);
                //printf("get item from directory: direct5: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }
        }
        else { // prohledavame indirect
            if ((i - 6) >= (int32_t)(SIZE_OF_CLUSTER/sizeof(int32_t))) { // indirect 2             
                if ((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t)) == 0) { // indirecty 2 stupne
                    fseek(fs, ind->indirect2 + (((i - 6) / (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // -1 ?????
                    fread(&block_address1, sizeof(int32_t), 1, fs);
                }

                fseek(fs, block_address1 + (((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // indirecty 1 stupne
                fread(&block_address2, sizeof(int32_t), 1, fs);
                
                return_value = get_item_from_datablock(block_address2, item);
            }
            else { // indirect 1                
                fseek(fs, ind->indirect1 + (i - 6) * sizeof(int32_t), SEEK_SET); // prohledavame od prvniho bloku (nulteho :), ktery je v odkazech
                fread(&block_address1, sizeof(int32_t), 1, fs);
                return_value = get_item_from_datablock(block_address1, item);
            }
        }     

        if (return_value != -1) {
            return return_value;    
        }

    }  
    return return_value;
}

/**
 * @brief Vrati adresu volneho uzlu.
 * 
 * @return Adresu volneho uzlu. 
 *         -1, kdyz neexistuje volny uzel.
 */
int32_t get_free_inode() {
    int32_t i = 0;
    int32_t free_node_address = 0;
    char isTaken = 0;
    
    fseek(fs, sp->bitmapinode_start_address, SEEK_SET);
    
    for (i = 0; i < sp->cluster_count; i++) {
        fread(&isTaken, sizeof(char), 1, fs);

        if (isTaken == 0) {
            free_node_address = sp->bitmapinode_start_address
                                + sp->cluster_count * sizeof(char) // bitmapa inodu
                                + sp->cluster_count * sizeof(char) // bitmapa datovych bloku
                                + i * sizeof(inode); // inody pred nasim volnym inodem
            return free_node_address;
        }
    }
    return -1;
}

/**
 * @brief Vrati adresu volneho datoveho bloku.
 * 
 * @return Adresu volneho datoveho bloku.
 *         -1 pokud nezbyva volny blok.
 */
int32_t get_free_datablock() {
    int32_t i = 0;
    int32_t free_datablock_address = 0;
    char isTaken = 0;
    
    fseek(fs, sp->bitmapdata_start_address, SEEK_SET);

    for (i = 0; i < sp->cluster_count; i++) {
        fread(&isTaken, sizeof(char), 1, fs);
        
        if (isTaken == 0) {
            free_datablock_address = sp->bitmapdata_start_address
                                    + sp->cluster_count * sizeof(char) // bitmapa datovych bloku
                                    + sp->cluster_count * sizeof(inode) // inody
                                    + i * SIZE_OF_CLUSTER; // datove bloky pred nasim volnym blokem
            //printf("get free datablock: vracim: %d\n", free_datablock_address);
            return free_datablock_address;
        }        
    }
    return -1;
}

/**
 * @brief Vrati ukazatel na inode, podle zadane adresy uzlu.
 * 
 * @param node_address Adresa uzlu, ktery chceme najit.
 * @return Ukazatel na uzel.
 */
inode * get_inode(int32_t node_address) {
    inode *ind = (inode *) malloc(sizeof(inode));
    
    fseek(fs, node_address, SEEK_SET);
    fread(&ind->nodeid, sizeof(int32_t), 1, fs);
    fread(&ind->isDirectory, sizeof(bool), 1,fs);
    fread(&ind->parent_directory_address, sizeof(int32_t), 1, fs);
    fread(&ind->file_size, sizeof(int32_t), 1, fs);
    fread(&ind->direct1, sizeof(int32_t), 1, fs);
    fread(&ind->direct2, sizeof(int32_t), 1, fs);
    fread(&ind->direct3, sizeof(int32_t), 1, fs);
    fread(&ind->direct4, sizeof(int32_t), 1, fs);
    fread(&ind->direct5, sizeof(int32_t), 1, fs);
    fread(&ind->indirect1, sizeof(int32_t), 1, fs);
    fread(&ind->indirect2, sizeof(int32_t), 1, fs);

    return ind;
}

