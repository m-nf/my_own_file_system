/**
 * @file read_and_write_functions.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funkce s funkcionalitou typu read a write.
 * @version 1.0
 * @date 2022-02-11
 */

#include "definitions.h"

/**
 * @brief Vypise bloky ve kterych se soubor nachazi.
 * 
 * @param ind Uzel u ktereho se maji bloky vypsat.
 */
void write_blocks_numbers_to_console(inode *ind, int32_t file_or_dir) {
    int32_t number_of_blocks = 0;    
    int32_t i = 0;
    int32_t block_address = 0;
    int32_t num_of_block = 0;
    
    number_of_blocks = (ind->file_size / file_or_dir) + 1; // chceme zapsat i ten zbytek

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
        printf(" - %d", block_address);
    }   
}

/**
 * @brief Nacte soubor z fs a vrati ho v pobobe *char. Je nutno ukazatel pak uvolnit.
 * 
 * @param ind Uzel, ze ktereho chceme oubor cist.
 * @return Vrati ukazatel na *char, kde je nacten soubor z fs.
 */
char * read_file_from_fs(inode *ind) { 
    //printf("read_file:\n");  
    int32_t lenght_to_read = 0;
    int32_t number_of_blocks = 0;
    int32_t i = 0;
    int32_t addr_of_block_to_read = 0;
    char *file;

    file = (char *) malloc(sizeof(char) * ind->file_size);    
    number_of_blocks = (ind->file_size / SIZE_OF_CLUSTER) + 1; // chceme zapsat i ten zbytek
    //printf("read_file_from_fs: ind->filesize: %d\n", ind->file_size);

    for (i = 0; i < number_of_blocks; i++) {
        addr_of_block_to_read = get_block_to_read(ind, i);
        //printf("read_file_from_fs: adresa bloku odkud cist: %d\n", addr_of_block_to_read);   

        if ((ind->file_size - (SIZE_OF_CLUSTER * (i + 1))) >= 0) {
            lenght_to_read = SIZE_OF_CLUSTER;            
        }
        else {
            lenght_to_read = (ind->file_size % SIZE_OF_CLUSTER);
            //printf("read_file_from_fs: zbytek: %d\n", lenght_to_read);       
        }

        fseek(fs, addr_of_block_to_read, SEEK_SET); 
        fread(&file[SIZE_OF_CLUSTER * i], sizeof(char), lenght_to_read, fs);

        //printf("read_file_from_fs: cteme: adresa uzlu: %d", ind->nodeid);        
    }  

    return(file);
}

/**
 * @brief Zapise soubor do fs.
 * 
 * @param file Soubor v podobe *char, ktery se ma zapsat.
 * @param file_lenght Delka souboru.
 * @param node_address Adresa inodu, kam se ma soubor zapsat.
 */
void write_file_to_fs(char *file, int32_t file_lenght, int32_t node_address) {
    inode *ind = get_inode(node_address);
    int32_t lenght_to_write = 0;
    int32_t number_of_blocks = 0;
    int32_t addr_of_block_to_write = 0;
    int32_t i = 0;
    char *file_part = NULL;
    
    number_of_blocks = (file_lenght / SIZE_OF_CLUSTER) + 1; // chceme zapsat i ten zbytek

    for (i = 0; i < number_of_blocks; i++) {
        addr_of_block_to_write = get_block_to_write(ind, SIZE_OF_CLUSTER);
        //printf("\nwrite file: adresa bloku kam psat: %d\n", addr_of_block_to_write);   

        if ((file_lenght - (SIZE_OF_CLUSTER * (i+1))) >= 0) { // chcemem bejt o krok napred
            lenght_to_write = SIZE_OF_CLUSTER;
            file_part = &file[SIZE_OF_CLUSTER * i];
        }
        else {
            lenght_to_write = file_lenght % SIZE_OF_CLUSTER;
            //printf("write_file_to_fs: file_lenght: %d, kam chceme psat: %d\n", file_lenght, file_lenght - lenght_to_write - 1);
            file_part = &file[file_lenght - lenght_to_write];
            //printf("write_file_to_fs: zapisujem zbytek: %d\n", lenght_to_write);
        }

        fseek(fs, addr_of_block_to_write, SEEK_SET);        
        fwrite(file_part, sizeof(char), lenght_to_write, fs);    

        //printf("write file: zapisujem: adresa noveho uzlu: %d, jmeno noveho uzlu: %s\n", dir_it_tmp->inode, dir_it_tmp->item_name);

        /* doladime velikost */
        fseek(fs, node_address + sizeof(int32_t) * 2 + sizeof(bool), SEEK_SET);
        ind->file_size += lenght_to_write;
        fwrite(&ind->file_size, sizeof(int32_t), 1, fs);
    }
    
    //printf("write_file_to_fs: filesize: %d\n", ind->file_size);
    free(ind);
}

/**
 * @brief Vypise jmeno polozky adresare. Polozka je ulozena v jednom bloku, ktery je predan parametrem.
 * 
 * @param address_of_block Adresa datoveho bloku, kde se nachazi polozka, jejiz jmeno se ma vypsat.
 */
void write_item_name_to_console(int32_t address_of_block) {
    char *name = (char *) malloc(sizeof(char) * NAME_LENGHT);
    int32_t address = 0;
    inode *ind;

    fseek(fs, address_of_block, SEEK_SET); // ziskavame uzel, polozky, kterou chceme vypsat
    fread(&address, sizeof(int32_t), 1, fs);
    ind = get_inode(address);

    fseek(fs, address_of_block + sizeof(int32_t), SEEK_SET); // ziskavame jmeno, polozky, kterou chceme vypsat
    fread(name, sizeof(char), NAME_LENGHT, fs);

    if (ind->isDirectory == true) { // slozka
        printf("+%s\n", name);
    }
    else {                         // soubor
        printf("-%s\n", name);
    }

    free(ind);
    free(name);
}

/**
 * @brief Zapise polozku do adresare. Polozka je predana v dir_it_tmp.
 * 
 * @param node_address V jakem adresari chceme dir_item zapsat.
 */
void write_directory_item(int32_t node_address) {
    inode *ind = get_inode(node_address);
    int32_t addr_of_block_to_write = get_block_to_write(ind, sizeof(directory_item));
    //printf("\nadresa bloku kam psat: %d\n", addr_of_block_to_write);   

    /* zapiseme polozku do adresare*/
    fseek(fs, addr_of_block_to_write, SEEK_SET);
    fwrite(&dir_it_tmp->inode, sizeof(int32_t), 1 , fs);
    fwrite(dir_it_tmp->item_name, sizeof(char), NAME_LENGHT, fs);

    //printf("write dir item: zapisujem: adresa noveho uzlu: %d, jmeno noveho uzlu: %s\n", dir_it_tmp->inode, dir_it_tmp->item_name);

    /* doladime velikost */
    fseek(fs, node_address + sizeof(int32_t) * 2 + sizeof(bool), SEEK_SET);
    ind->file_size += sizeof(directory_item);
    fwrite(&ind->file_size, sizeof(int32_t), 1, fs);
    
    free(ind);
}





