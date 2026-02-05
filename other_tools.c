/**
 * @file other_tools.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje všechny ostatni funkce, ktere neodpovidaly zarazeni do ostatnich souboru.
 * @version 1.0
 * @date 2022-02-11
 */

#include "definitions.h"

/**
 * @brief Odstrani soubor a vymaze ho z rodicovskeho adresare. Uvolni uzel souboru.
 * 
 * @param parent_ind Adresar, ze ktereho se ma soubor odstranit. 
 * @param ind_to_remove Uzel, ktery se ma odstranit.
 * @param item_to_remove Jmeno souboru, ktery se ma odstranit.
 */
void remove_file(inode *parent_ind, inode *ind_to_remove, char *item_to_remove) {
    set_free_blocks_of_file(ind_to_remove);

    remove_dir_item(parent_ind, item_to_remove);
        
    set_bit_bitmap_inode(ind_to_remove->nodeid, 0); // uvolni uzel po odstranene slozce
}

/**
 * @brief Zjisti, zda je polozka v adresari ulozena v poslendim datovem bloku v uzlu, nebo ne.
 * 
 * @param parent_ind Adresar k prohledani
 * @param item Polozka, kterou hledame.
 * @return true Je posledni.
 * @return false Neni posledni.
 */
bool is_it_last_item(inode *parent_ind, char* item) {
    int32_t return_value = -1;    
    int32_t i = 0;
    int32_t block_address1 = 0, block_address2 = 0;
    int32_t number_of_blocks = parent_ind->file_size / sizeof(directory_item);

    for (i = 1; i <= number_of_blocks; i++) { // prohledavame po jednom datove bloky uzlu, do file_size                  
        if (i < 6) { // prohledavame direct
            if (i == 1) { // direct 1
                return_value = get_item_from_datablock(parent_ind->direct1, item);
                //printf("is_it_last_item: direct1: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 2) { // direct 2
                return_value = get_item_from_datablock(parent_ind->direct2, item);
                //printf("is_it_last_item: direct2: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                   
            else if (i == 3) {  // direct 3
                return_value = get_item_from_datablock(parent_ind->direct3, item);
                //printf("is_it_last_item: direct3: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 4) { // direct 4
                return_value = get_item_from_datablock(parent_ind->direct4, item);
                //printf("is_it_last_item: direct4: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }                    
            else if (i == 5) { // direct 5
                return_value = get_item_from_datablock(parent_ind->direct5, item);
               //printf("is_it_last_item: direct5: polozka k dostani: %s, retrun value: %d\n", item, return_value);
            }
        }
        else { // prohledavame indirect
            if ((i - 6) >= (int32_t)(SIZE_OF_CLUSTER/sizeof(int32_t))) { // indirect 2             
                if ((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t)) == 0) { // indirecty 2 stupne
                    fseek(fs, parent_ind->indirect2 + (((i - 6) / (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // -1 ???
                    fread(&block_address1, sizeof(int32_t), 1, fs);
                }

                fseek(fs, block_address1 + (((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // indirecty 1 stupne
                fread(&block_address2, sizeof(int32_t), 1, fs);
                
                return_value = get_item_from_datablock(block_address2, item);
            }
            else { // indirect 1                
                fseek(fs, parent_ind->indirect1 + (i - 6) * sizeof(int32_t), SEEK_SET); // prohledavame od prvniho bloku (nulteho :), ktery je v odkazech
                fread(&block_address1, sizeof(int32_t), 1, fs);
                return_value = get_item_from_datablock(block_address1, item);
            }
        }     

        if (return_value != -1 && i == number_of_blocks) {
            //printf("is_it_last_item: return: true");
            return true;    
        }

    } 
    //printf("is_it_last_item: return: false");
    return false;
}

/**
 * @brief Ostrani z adresare referenci na posledni polozku.
 * 
 * @param ind Adresar, odkud se ma reference odstranit.
 */
void remove_ref_to_last_data_block(inode *ind) {
    int32_t num_of_block = ind->file_size / sizeof(directory_item);
    
    if (num_of_block > 5) { // indirect

        num_of_block -= 6;
        if (num_of_block >= (int32_t)(SIZE_OF_CLUSTER / sizeof(int32_t))) { //indirect 2 !!!
            printf("ERROR - remove_ref_to_last_data_block - double indirect\n");

        }
        else { // indirect 1
            if (num_of_block == 0) {
                set_bit_bitmap_data(ind->indirect1, 0); // uvolnime odkazovy blok indirect 1
                fseek(fs, ind->nodeid + sizeof(int32_t) * 8 + sizeof(bool), SEEK_SET); // indirect 1 v inodu
            }
            else {
                fseek(fs, ind->indirect1 + (num_of_block * sizeof(int32_t)), SEEK_SET);
            }           
        }
    }
    else { // direct
        if (num_of_block == 1) { // direct 1            
            fseek(fs, ind->nodeid + sizeof(int32_t) * 3 + sizeof(bool), SEEK_SET);               
        }
        else if (num_of_block == 2) { // direct 2
            fseek(fs, ind->nodeid + sizeof(int32_t) * 4 + sizeof(bool), SEEK_SET);
        }
        else if (num_of_block == 3) {  // direct 3
            fseek(fs, ind->nodeid + sizeof(int32_t) * 5 + sizeof(bool), SEEK_SET);
        }                    
        else if (num_of_block == 4) { // direct 4
            fseek(fs, ind->nodeid + sizeof(int32_t) * 6 + sizeof(bool), SEEK_SET);
        }                    
        else { // direct 5
           fseek(fs, ind->nodeid + sizeof(int32_t) * 7 + sizeof(bool), SEEK_SET);
        }
    }

    int32_t ref = -1;
    fwrite(&ref, sizeof(int32_t), 1, fs);
}

/**
 * @brief Prepise odkaz na polozku v adresari na jinou polozku. Prepise jak jmeno, tak adresu.
 * 
 * @param parent_ind Rodicovska slozka.
 * @param item_to_remove Jmeno polozky k odstraneni/prepsani.
 * @param item_to_write Jmeno polozky k zapsani.
 * @param address_to_write Adresa polozky k zapsani.
 */
void rewrite_item_in_dir(inode *parent_ind, char *item_to_remove, char *item_to_write, int32_t address_to_write) {
    int32_t return_value = -1;    
    int32_t i = 0;
    int32_t block_address1 = 0, block_address2 = 0;
    int32_t number_of_blocks = parent_ind->file_size / sizeof(directory_item);

    for (i = 1; i <= number_of_blocks; i++) { // prohledavame po jednom datove bloky uzlu, do file_size                  
        if (i < 6) { // prohledavame direct
            if (i == 1) { // direct 1
                return_value = get_item_from_datablock(parent_ind->direct1, item_to_remove);
                //printf("get item from directory: direct1: polozka k dostani: %s, retrun value: %d\n", item, return_value);
                if (return_value != -1) {
                    fseek(fs, parent_ind->direct1, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }                    
            else if (i == 2) { // direct 2
                return_value = get_item_from_datablock(parent_ind->direct2, item_to_remove);
                //printf("get item from directory: direct2: polozka k dostani: %s, retrun value: %d\n", item, return_value);
                if (return_value != -1) {
                    fseek(fs, parent_ind->direct2, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }                   
            else if (i == 3) {  // direct 3
                return_value = get_item_from_datablock(parent_ind->direct3, item_to_remove);
                //printf("get item from directory: direct3: polozka k dostani: %s, retrun value: %d\n", item, return_value);
                if (return_value != -1) {
                    fseek(fs, parent_ind->direct3, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }                    
            else if (i == 4) { // direct 4
                return_value = get_item_from_datablock(parent_ind->direct4, item_to_remove);
                //printf("get item from directory: direct4: polozka k dostani: %s, retrun value: %d\n", item, return_value);
                if (return_value != -1) {
                    fseek(fs, parent_ind->direct4, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }                    
            else if (i == 5) { // direct 5
                return_value = get_item_from_datablock(parent_ind->direct5, item_to_remove);
                //printf("get item from directory: direct5: polozka k dostani: %s, retrun value: %d\n", item, return_value);
                if (return_value != -1) {
                    fseek(fs, parent_ind->direct5, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }
        }
        else { // prohledavame indirect
            if ((i - 6) >= (int32_t)(SIZE_OF_CLUSTER/sizeof(int32_t))) { // indirect 2             
                if ((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t)) == 0) { // indirecty 2 stupne
                    fseek(fs, parent_ind->indirect2 + (((i - 6) / (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // -1 ?????
                    fread(&block_address1, sizeof(int32_t), 1, fs);
                }

                fseek(fs, block_address1 + (((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // indirecty 1 stupne
                fread(&block_address2, sizeof(int32_t), 1, fs);
                
                return_value = get_item_from_datablock(block_address2, item_to_remove);

                if (return_value != -1) {
                    fseek(fs, block_address2, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }
            else { // indirect 1                
                fseek(fs, parent_ind->indirect1 + (i - 6) * sizeof(int32_t), SEEK_SET); // prohledavame od prvniho bloku (nulteho :), ktery je v odkazech
                fread(&block_address1, sizeof(int32_t), 1, fs);
                return_value = get_item_from_datablock(block_address1, item_to_remove);

                if (return_value != -1) {
                    fseek(fs, block_address1, SEEK_SET);
                    fwrite(&address_to_write, sizeof(int32_t), 1, fs);
                    fwrite(item_to_write, sizeof(char), NAME_LENGHT, fs);
                }
            }
        }
        if (return_value != -1) {
            break;
        }  
    }    
}

/**
 * @brief Odstrani slozku z rodicovskeho adresare. Neuvolni uzel polozky.
 * 
 * @param parent_ind Adresar, ze ktereho se ma polozka odstranit. 
 * @param ind_to_remove Uzel, ktery se ma odstranit.
 * @param item_to_remove Jmeno polozky, ktery se ma odstranit.
 */
void remove_dir_item(inode *parent_ind, char *item_to_remove) {
   
    if (is_it_last_item(parent_ind, item_to_remove) == false) {
        //printf("remove_dir_item: neni to poslendi polozka\n");
        
        char *name = get_name_of_last_item(parent_ind);
        int32_t address_of_last_item = get_item_from_directory(parent_ind, name);
        

        //printf("remove_dir_item: itemtoremove: %s, lastname: %s, address_of_last: %d\n", item_to_remove, name, address_of_last_item);
        
        rewrite_item_in_dir(parent_ind, item_to_remove, name, address_of_last_item);

        free(name);
    }

    remove_ref_to_last_data_block(parent_ind); // nastavime odkaz posledniho bloku v rodicovske slozce na -1
    int32_t last_data_block = get_last_data_block(parent_ind, sizeof(directory_item));
    //printf("remove_dir_item: last_data_block: %d, direct3: %d\n", last_data_block, parent_ind->direct3);
    set_bit_bitmap_data(last_data_block, 0); // uvolnime blok po ostranene polozce
    
    fseek(fs, parent_ind->nodeid + sizeof(int32_t) * 2 + sizeof(bool), SEEK_SET); // zmensime velikost rodicovske slozky
    parent_ind->file_size -= sizeof(directory_item);
    fwrite(&parent_ind->file_size, sizeof(int32_t), 1, fs);   
}

/**
 * @brief Rozhodne, zda je dany uzel prazdny.
 * 
 * @param ind Uzel u ktereho zjistujem, zda je prazdny.
 * @return true - kdyz dany uzel nic neobsahuje.
 * @return false - kdyz dany uzel neco obsahuje (slozku, nebo soubor).
 */
bool is_ind_empty(inode *ind) {
    if (ind->direct1 == -1) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief Vybere novy datovy/odkazovy blok a zapise jeho adresu do uzlu.
 * 
 * @param ind Uzel, kteremu novy blok hledame.
 * @param num_direct_data_blok Znacka, kam mame novy blok zapsat.
 */
void assign_direct_data_block(inode *ind, int32_t num_direct_data_blok) {
    int32_t new_block_address = get_free_datablock();    
    set_bit_bitmap_data(new_block_address, 1);

    if (num_direct_data_blok == 1) {
        fseek(fs, ind->nodeid + sizeof(int32_t) * 3 + sizeof(bool), SEEK_SET);
        ind->direct1 = new_block_address;
        //printf("assign direct: prirazujeme do directu: 1, prirazujeme uzlu: %d, adresa noveho datoveho bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 2) {
        fseek(fs, ind->nodeid + sizeof(int32_t) * 4 + sizeof(bool), SEEK_SET);
        ind->direct2 = new_block_address;
        //printf("assign direct: prirazujeme do directu: 2, prirazujeme uzlu: %d, adresa noveho datoveho bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 3) {
        fseek(fs, ind->nodeid + sizeof(int32_t) * 5 + sizeof(bool), SEEK_SET);
        ind->direct3 = new_block_address;
        //printf("assign direct: prirazujeme do directu: 3, prirazujeme uzlu: %d, adresa noveho datoveho bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 4) {
        fseek(fs, ind->nodeid + sizeof(int32_t) * 6 + sizeof(bool), SEEK_SET);
        ind->direct4 = new_block_address;
        //printf("assign direct: prirazujeme do directu: 4, prirazujeme uzlu: %d, adresa noveho datoveho bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 5) {
        fseek(fs, ind->nodeid + sizeof(int32_t) * 7 + sizeof(bool), SEEK_SET);
        ind->direct5 = new_block_address;
        //printf("assign direct: prirazujeme do directu: 5, prirazujeme uzlu: %d, adresa noveho datoveho bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 6) {
        // nastaveni odkazu v indirect bloku na -1
        int32_t *c = (int32_t *) malloc(sizeof(int32_t) * (SIZE_OF_CLUSTER / sizeof(int32_t)));
        memset(c, -1, sizeof(int32_t) * (SIZE_OF_CLUSTER / sizeof(int32_t)));
        fseek(fs, new_block_address, SEEK_SET);
        fwrite(c, sizeof(int32_t), (SIZE_OF_CLUSTER / sizeof(int32_t)), fs);
        free(c);
        
        // zapsani odkazu
        fseek(fs, ind->nodeid + sizeof(int32_t) * 8 + sizeof(bool), SEEK_SET);        
        ind->indirect1 = new_block_address;

        //printf("assign direct: prirazujeme do indirectu 1, prirazujeme uzlu: %d, adresa noveho odkazoveho1 bloku: %d", ind->nodeid, new_block_address);
    }
    else if (num_direct_data_blok == 7) {
        // nastaveni odkazu v indirect bloku na -1
        int32_t *c = (int32_t *) malloc(SIZE_OF_CLUSTER);
        memset(c, -1, SIZE_OF_CLUSTER);
        fseek(fs, new_block_address, SEEK_SET);
        fwrite(c, sizeof(int32_t), (SIZE_OF_CLUSTER / sizeof(int32_t)), fs);
        free(c);

        fseek(fs, ind->nodeid + sizeof(int32_t) * 9 + sizeof(bool), SEEK_SET);
        ind->indirect2 = new_block_address;

        //printf("assign direct: prirazujeme do indirectu 2, prirazujeme uzlu: %d, adresa noveho odkazoveho2 bloku: %d", ind->nodeid, new_block_address);
    }
            
    fwrite(&new_block_address, sizeof(int32_t), 1, fs);
    /*
    int32_t addr = 0;
    fseek(fs, actual_ind->nodeid + sizeof(int32_t) * 3 + sizeof(bool), SEEK_SET);
    fread(&addr, sizeof(int32_t), 1, fs);
    printf("aktulni direct1: %d", addr);
    */
}

/**
 * @brief Priradi novy datovy blok a jeho adresu zapise do odkazoveho bloku.
 * 
 * @param address Adresa, kam se adresa noveho datoveho bloku ma zapsat.
 */
void assign_data_block_to_indirect1(int32_t address) {
    int32_t new_block_address = get_free_datablock();
    set_bit_bitmap_data(new_block_address, 1);

    fseek(fs, address, SEEK_SET);
    fwrite(&new_block_address, sizeof(int32_t), 1, fs);

    //printf("assign data block to indirect1: prirazujeme na adresu: %d, adresa noveho datoveho bloku: %d\n", address, new_block_address);
}

/**
 * @brief Priradi novy odkazovy blok a zapise jeho adresu do odkazoveho bloku uzlu.
 *        Novy blok se naplni -1, po intech.
 * 
 * @param address adresa, kam chceme zapsat adresu noveho odkazoveho bloku.
 */
void assign_reference_block_to_indirect2(int32_t address) {
    int32_t new_block_address = get_free_datablock();
    set_bit_bitmap_data(new_block_address, 1);
        
    int32_t *c = (int32_t *) malloc(SIZE_OF_CLUSTER);
    memset(c, -1, SIZE_OF_CLUSTER);

    fseek(fs, new_block_address, SEEK_SET); // do noveho bloku nasazime -1
    fwrite(c, sizeof(int32_t), (SIZE_OF_CLUSTER / sizeof(int32_t)), fs);
    free(c);

    fseek(fs, address, SEEK_SET); // zapsani adresy na odkazovy blok do odkazoveho bloku
    fwrite(&new_block_address, sizeof(int32_t), 1, fs);
    
    //printf("assign odkazovy block to indirect2: prirazujeme na adresu: %d, adresa noveho odkazoveho bloku: %d\n", address, new_block_address);  
}

/**
 * @brief Vrati adresu predposleni polozky cesty predane v path.
 * 
 * @param path Cesta zadana uzivatelem.
 * @return -1 neplatna cesta
 *         adresu predposledni polozky cesty
 */
int32_t parse_path(char *path) {
    int32_t num_of_splits_path = 0, split = 0;
    int32_t return_value = -1;    
    inode *ind;
    char *str_cpy;
    
    //printf("jsem v parse_path, path: %s\n", path);
    str_cpy = create_string_copy(path);
    char **splited_path = str_split(str_cpy, '/', &num_of_splits_path);    
    //printf("parse_path: numsplits: %d path[0]: %c path: %s\n", num_of_splits_path, path[0], path);
    
    if (num_of_splits_path == 1) { // je to rovnou nazev slozky / souboru
        free(splited_path);
        free(str_cpy);
        return actual_ind->nodeid;
    }
    else {
        if (path[0] == '/') { // absolutni cesta
        //printf("jsem v abs ceste");
            if (num_of_splits_path == 2) { // jedna se o domovsky adresar
                //printf("jsem v prvni podmince\n");
                free(splited_path);
                free(str_cpy);
                return sp->inode_start_address;
            }

            num_of_splits_path -= 1; // vracime preci predposledni prvek cesty
            ind = get_inode(sp->inode_start_address); // zaciname prohledavat od prvniho inodu          
            split = 1; // viz popis funkce str_split

            for (split = 1; split < num_of_splits_path; split++) { // casti/polozky cesty                
                return_value = get_item_from_directory(ind, splited_path[split]);

                if (return_value != -1) { // neco se naslo                        
                    free(ind);
                    ind = get_inode(return_value);
                    
                    if (ind->isDirectory == false) { // cesta neexistuje
                        free(splited_path);
                        free(ind);
                        free(str_cpy);
                        return -1;
                    }                    
                }
                else { // nic se nenaslo
                    free(splited_path);
                    free(ind);
                    free(str_cpy);
                    return -1;
                }                    
            }

            free(splited_path);
            free(ind);
            free(str_cpy);
            return return_value;
        }
        else { // relativni cesta           
            //printf("jsem v relativni ceste\n");
            num_of_splits_path -= 1; // vracime preci predposledni prvek cesty
            ind = get_inode(actual_ind->nodeid); // zaciname prohledavat od aktualniho inodu            

            for (split = 0; split < num_of_splits_path; split++) { // casti/polozky cesty                
                return_value = get_item_from_directory(ind, splited_path[split]);

                if (return_value != -1) { // neco se naslo                        
                    free(ind);
                    ind = get_inode(return_value);
                    
                    if (ind->isDirectory == false) { // cesta neexistuje
                        free(splited_path);
                        free(ind);
                        free(str_cpy);
                        return -1;
                    }                    
                }
                else { // nic se nenaslo
                    free(splited_path);
                    free(ind);
                    free(str_cpy);
                    return -1;
                }                    
            }

            free(splited_path);
            free(ind);
            free(str_cpy);
            return return_value;
        }        
    }    
}