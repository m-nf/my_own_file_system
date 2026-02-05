/**
 * @file commands_functions.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funkce pro vykonavani jednotlivych prikazu.
 * @version 1.0
 * @date 2022-02-11
 */

#include "definitions.h"

/**
 * @brief Vypise info o uzlu. Format:
 *        NAME – SIZE – i-node NUMBER – přímé a nepřímé odkazy
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru.
 */
void info(char **input) {
    int32_t ind_address = 0;
    int32_t num_of_splits_path = 0;
    inode *ind;    
    char *str_cpy, **items;
    
    ind_address = get_last_item_of_path(input[1]);
    if (ind_address == -1) { //soubor neexistuje
        printf("FILE NOT FOUND\n");        
        return;
    }
    
    str_cpy = create_string_copy(input[1]); // jak se novy soubor bude jmenovat ve fs
    items = str_split(str_cpy, '/', &num_of_splits_path);
    ind = get_inode(ind_address);
    
    printf("%s - %d - %d", items[num_of_splits_path - 1], ind->file_size, ind->nodeid);    
    
    if (ind->isDirectory == true) {
        write_blocks_numbers_to_console(ind, sizeof(directory_item));
    }
    else {
        write_blocks_numbers_to_console(ind, SIZE_OF_CLUSTER);
    }
    
    printf("\n");

    free(str_cpy);
    free(items);
    free(ind);
}

/**
 * @brief Pokud je soubor delsi nez 5000 bytu, zkrati ho na 5000 bytu.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru.
 */
void short_func(char **input) {
    int32_t file_address = 0;
    inode *file_ind;
    
    file_address = get_last_item_of_path(input[1]);
    if (file_address == -1) { //soubor neexistuje
        printf("FILE NOT FOUND\n");        
        return;
    }
    file_ind = get_inode(file_address);
    if (file_ind->isDirectory == true) { // zdrojovy soubor neni soubor, je to slozka
        printf("FILE NOT FOUND\n");        
        free(file_ind);
        return;
    }

    if (file_ind->file_size > 5000) {
        set_free_over_5000B(file_ind);

        // doladime velikost ve fs
        fseek(fs, file_ind->nodeid + sizeof(int32_t) * 2 + sizeof(bool), SEEK_SET);
        file_ind->file_size = 5000;
        fwrite(&file_ind->file_size, sizeof(int32_t), 1, fs);
    }

    printf("OK\n");

    free(file_ind);
}

/**
 * @brief Spoji dva soubory dohromady do tretiho.
 * 
 * @param input [1] cesta k souboru 1.
 *              [2] cesta k souboru 2.
 *              [3] cesta k souboru 3.
 */
void xcp(char **input) {
    int32_t file_address1 = 0, file_address2 = 0, file_address3 = 0, parent_address3 = 0;
    int32_t num_of_splits_path = 0;
    int32_t i = 0;
    inode *file_ind1, *file_ind2;
    char *file1, *file2, *file3;
    char *str_cpy, **items;

    file_address1 = get_last_item_of_path(input[1]); // k zdrojovy souborum neni cesta
    file_address2 = get_last_item_of_path(input[2]);
    if (file_address1 == -1 || file_address2 == -1) {        
        printf("FILE NOT FOUND\n");
        return;
    }

    file_ind1 = get_inode(file_address1);
    file_ind2 = get_inode(file_address2);
    if (file_ind1->isDirectory == true || file_ind2->isDirectory == true) { // zdrojovy soubor neni soubor, je to slozka
        printf("FILE NOT FOUND\n");        
        free(file_ind1);
        free(file_ind2);
        return;
    }

    parent_address3 = parse_path(input[3]);
    if (parent_address3 == -1) { // rodicovksa slozka noveho souboru neexistuje
        printf("PATH NOT FOUND\n");
        free(file_ind1);
        free(file_ind2);
        return;
    }

    file_address3 = get_last_item_of_path(input[3]);
    if (file_address3 != -1) { // novy soubor jiz existuje
        printf("PATH NOT FOUND\n");
        free(file_ind1);
        free(file_ind2);
        return;
    }   

    file_address3 = get_free_inode(); // vytvarime novy uzel souboru
    if (file_address3 == -1) {
        printf("EROR - disk is full.");
        free(file_ind1);
        free(file_ind2);
        return;
    }
    set_bit_bitmap_inode(file_address3, 1); // obsad uzel

    str_cpy = create_string_copy(input[3]); // jak se novy soubor bude jmenovat ve fs
    items = str_split(str_cpy, '/', &num_of_splits_path);

    dir_it_tmp->inode = file_address3; // zapiseme polozku do rodicovskyho adesare
    create_item_name(items[num_of_splits_path - 1]);
    write_directory_item(parent_address3);

    create_inode(file_address3, parent_address3, false); // zapiseme novy uzel do fs
    
    file1 = read_file_from_fs(file_ind1); // cteme soubory z fs
    file2 = read_file_from_fs(file_ind2);
    file3 = (char *) malloc(sizeof(char) * (file_ind1->file_size + file_ind2->file_size));

    for (i = 0; i < file_ind1->file_size; i++) { // spojujeme soubory z fs
        file3[i] = file1[i];
    }
    for (i = file_ind1->file_size; i < (file_ind1->file_size + file_ind2->file_size); i++) {
        file3[i] = file2[i - file_ind1->file_size];
    }

    write_file_to_fs(file3, (file_ind1->file_size + file_ind2->file_size), file_address3); // zapisujeme spojene soubory do fs

    printf("OK\n");
    
    free(file_ind1);
    free(file_ind2);
    free(file1);
    free(file2);
    free(file3);
    free(str_cpy);
    free(items);  
}

/**
 * @brief Presune souboru z umisteni na fs do umizteni na fs. Pripadne ho prejmenuje.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] Odkud chceme soubor presunout. - file1
 *              [2] Kam chceme soubor presunout.  - file2
 */
void mv(char **input) {
    int32_t file_address1 = 0, parent_address1 = 0;
    int32_t parent_address2 = 0, file_address2 = 0;
    int32_t num_of_splits_path = 0;
    char **path_items, *str_cpy; 
    inode *file_ind1, *parent_ind1;   

    // kontrolujem zdroj
    file_address1 = get_last_item_of_path(input[1]);
    //printf("mv: input: %s\n", input[1]);
    if (file_address1 == -1) {        
        printf("FILE NOT FOUND\n");
        return;
    }
    file_ind1 = get_inode(file_address1);
    if (file_ind1->isDirectory == true) { // neni to soubor, je to slozka
        printf("FILE NOT FOUND\n");        
        free(file_ind1);        
        return;
    }

    // kontrolujem cil
    parent_address2 = parse_path(input[2]); // neexistuje cesta
    if (parent_address2 == -1) {        
        printf("PATH NOT FOUND\n");
        free(file_ind1);
        return;
    }
    file_address2 = get_last_item_of_path(input[2]);
    if (file_address2 != -1) { // soubor uz existuje
        printf("PATH NOT FOUND\n");
        free(file_ind1);
        return;
    }
    
    // odstranujem zdroj
    str_cpy = create_string_copy(input[1]); // ziskavame jmeno
    path_items = str_split(str_cpy, '/', &num_of_splits_path);
    parent_address1 = parse_path(input[1]);  
    parent_ind1 = get_inode(parent_address1); // pozor

    remove_dir_item(parent_ind1, path_items[num_of_splits_path - 1]);

    free(str_cpy);
    free(path_items);

    // zapisujem cil
    str_cpy = create_string_copy(input[2]); // ziskavame jmeno
    path_items = str_split(str_cpy, '/', &num_of_splits_path);
    
    dir_it_tmp->inode = file_address1;
    create_item_name(path_items[num_of_splits_path - 1]); // vytvarime jmeno    
    write_directory_item(parent_address2);   
    

    printf("OK\n");

    free(str_cpy);
    free(path_items);
    free(file_ind1);
    free(parent_ind1);
}

/**
 * @brief Zkopiruje soubor.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] Odkud chceme soubor kopirovat.
 *              [2] Kam chceme soubor kopirovat.
 */
void cp(char **input) {
    int32_t file_address = 0;
    int32_t new_node_address = 0;
    int32_t target_parent_address = 0, target_address =0;
    int32_t num_of_splits_path_target = 0;
    char **target_path_items, *str_cpy; 
    inode *file_ind;   
    char *file;    

    // kontrolujem zdroj
    file_address = get_last_item_of_path(input[1]);
    if (file_address == -1) {        
        printf("FILE NOT FOUND\n");
        return;
    }
    file_ind = get_inode(file_address);
    if (file_ind->isDirectory == true) { // neni to soubor, je to slozka
        printf("FILE NOT FOUND\n");        
        free(file_ind);        
        return;
    }

    // kontrolujem cil
    target_parent_address = parse_path(input[2]);
    if (target_parent_address == -1) {        
        printf("PATH NOT FOUND\n");
        free(file_ind);
        return;
    }
    target_address = get_last_item_of_path(input[2]);
    if (target_address != -1) { // soubor uz existuje
        printf("PATH NOT FOUND\n");
        free(file_ind);
        return;
    }   
    
    new_node_address = get_free_inode(); // adresa noveho uzlu
    if (new_node_address == -1) {
        printf("EROR - disk is full.");
        free(file_ind);     
        return;
    }
    set_bit_bitmap_inode(new_node_address, 1); // obsad uzel 
    
    
    str_cpy = create_string_copy(input[2]); // ziskavame jmeno
    target_path_items = str_split(str_cpy, '/', &num_of_splits_path_target);
    
    dir_it_tmp->inode = new_node_address;
    create_item_name(target_path_items[num_of_splits_path_target - 1]); // vytvarime jmeno
    write_directory_item(target_parent_address); // zapiseme polozku do rodicovskyho adesare    

    create_inode(new_node_address, target_parent_address, false); // vytvarime uzel
        
    file = read_file_from_fs(file_ind); // cteme soubor
    write_file_to_fs(file, file_ind->file_size, new_node_address); // zapisujem soubor
    
    free(str_cpy);
    free(target_path_items);
    free(file_ind);
    free(file);
}

/**
 * @brief Vypise obsah souboru.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void cat(char **input) {
    int32_t file_address = 0;
    inode *file_ind;   
    char *file;
    int32_t i = 0;

    file_address = get_last_item_of_path(input[1]);
    if (file_address == -1) {        
        printf("FILE NOT FOUND\n");
        return;
    }
    file_ind = get_inode(file_address);
    if (file_ind->isDirectory == true) { // neni to soubor, je to slozka
        printf("FILE NOT FOUND\n");        
        free(file_ind);        
        return;
    }

    file = read_file_from_fs(file_ind);    

    for (i = 0; i < file_ind->file_size; i++) {
        printf("%c", file[i]);
    }

    printf("\n");
    free(file_ind);
    free(file);
}

/**
 * @brief Nacte soubor z fs a ulozi ho na pevny disk.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru na fs pro cteni
 *              [2] cesta k souboru na disku pro zapis
 */
void outcp(char **input) {    
    int32_t file_address = 0;
    inode *file_ind;    
    FILE *fp;
    char *file;    

    fp = fopen(input[2], "wb");
    if (fp == NULL) {
        printf("FILE NOT FOUND\n");
        return;
    }

    file_address = get_last_item_of_path(input[1]);
    if (file_address == -1) {
        fclose(fp);
        printf("PATH NOT FOUND\n");
        return;
    }
    file_ind = get_inode(file_address);
    if (file_ind->isDirectory == true) { // neni to soubor, je to slozka
        printf("PATH NOT FOUND\n");
        fclose(fp);
        free(file_ind);        
        return;
    }

    file = read_file_from_fs(file_ind);
    fseek(fp, 0, SEEK_SET);
    fwrite(file, sizeof(char), file_ind->file_size, fp);


    printf("OK\n");

    fclose(fp);
    free(file_ind);
    free(file);
}

/**
 * @brief Nahraje soubor z pevneho disku a ulozi ho do fs.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru na disku pro cteni
 *              [2] cesta k souboru na fs pro zapis
 */
void incp(char **input) {
    int32_t file_lenght = 0, parent_address = 0, file_address;
    int32_t num_of_splits_path = 0;
    int32_t new_node_address = 0;
    inode *parent_ind;
    FILE *fp;
    char *file;
    char *str_cpy, **items;    

    fp = fopen(input[1], "rb");
    if (fp == NULL) {
        printf("FILE NOT FOUND\n");
        return;
    }

    parent_address = parse_path(input[2]); // cesta neexistuje
    if (parent_address == -1) {
        fclose(fp);
        printf("PATH NOT FOUND\n");
        return;
    }
    parent_ind = get_inode(parent_address);
    if (parent_ind->isDirectory == false) { // rodicovska slozka je ve skutecnosti soubor
        printf("PATH NOT FOUND\n");
        fclose(fp);
        free(parent_ind);        
        return;
    }
    file_address = get_last_item_of_path(input[2]); // soubor jiz existuje
    if (file_address != -1) {
        printf("PATH NOT FOUND\n");
        fclose(fp);
        free(parent_ind);
        return;
    }

    file_lenght = get_file_lenght(fp);    
    file = (char *) malloc(sizeof(char) * file_lenght); // cteme soubor z disku
    fread(file, sizeof(char), file_lenght, fp);

    //printf("incp: file_lenght: %d\n", file_lenght);

    str_cpy = create_string_copy(input[2]); // jak se soubor bude jmenovat ve fs
    items = str_split(str_cpy, '/', &num_of_splits_path);

    new_node_address = get_free_inode();
    if (new_node_address == -1) {
        printf("EROR - disk is full.");
        fclose(fp);
        free(parent_ind);
        free(file);
        free(items);
        free(str_cpy);
        return;
    }
    set_bit_bitmap_inode(new_node_address, 1); // obsad uzel 

    dir_it_tmp->inode = new_node_address; // zapiseme polozku do rodicovskyho adesare
    create_item_name(items[num_of_splits_path - 1]);
    write_directory_item(parent_address);

    create_inode(new_node_address, parent_address, false); // zapiseme novy uzel do fs

    write_file_to_fs(file, file_lenght, new_node_address); // zapiseme soubor do noveho uzlu
    //printf("incp: file_lenght: %d\n", file_lenght);
    printf("OK\n");

    fclose(fp);
    free(parent_ind);
    free(file);
    free(items);
    free(str_cpy);
}

/**
 * @brief Nacte soubor s prikazy a vykona je. Jeden prikaz - jedna radka. 
 *        Konec radky '\n'.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void load(char **input, char *fs_name) {
    int32_t line_lenght_counter = 0;
    int32_t file_lenght_counter = 0;
    int32_t file_lenght = 0;
    char *line;
    char c = 'a';

    FILE *fp_instructions;
    fp_instructions = fopen(input[1], "r");

    if (fp_instructions == NULL) {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    file_lenght = get_file_lenght(fp_instructions);
    //printf("filelenght: %d\n", file_lenght);   

    for (file_lenght_counter = 0; file_lenght_counter < file_lenght; file_lenght_counter += line_lenght_counter) {
        line_lenght_counter = 0;
        line = (char *) malloc(sizeof(char) * ROW_LENGHT);
        c = 'a';
        
        while(c != '\n') {
            fread(&c, sizeof(char), 1, fp_instructions);

            line[line_lenght_counter] = c;

            line_lenght_counter++;

            if (line_lenght_counter % ROW_LENGHT == 0) {
                line = (char *) realloc(line, line_lenght_counter * 2);
            }            
        }
        
        delete_nl_char(line);        
        parse_input(line, fs_name);

        free(line);
        //printf("filelenghtcounter: %d, linelenghtcounter: %d\n",file_lenght_counter, line_lenght_counter);
        line_lenght_counter++; // ve windows je '\n' dva byty.. ???    
    }

    printf("OK\n");
}

/**
 * @brief Odstrani soubor z adresare.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void rm(char **input) {
    int32_t address_to_remove = 0, parent_dir_address = 0;    
    int32_t num_of_splits_path = 0;
    inode *ind_to_remove, *parent_ind;
    char *str_cpy, **items;
    
    address_to_remove = get_last_item_of_path(input[1]);
    
    if (address_to_remove == -1) { // slozka, kterou chceme smazat neexistuje        
        printf("FILE NOT FOUND\n");        
        return;
    }    
    
    ind_to_remove = get_inode(address_to_remove);

    if (ind_to_remove->isDirectory == true) { // soubor ke smazani je ve skutecnosti slozka
        printf("FILE NOT FOUND\n");
        free(ind_to_remove);
        return;
    }  

    parent_dir_address = parse_path(input[1]); // uz vime, ze cesta existuje - nekontrolujem
    parent_ind = get_inode(parent_dir_address);    
    
    str_cpy = create_string_copy(input[1]); // chceme vedet jmeno polozky, kterou chceme vymazat
    items = str_split(str_cpy, '/', &num_of_splits_path);

    remove_file(parent_ind, ind_to_remove, items[num_of_splits_path - 1]);

    printf("OK\n");

    free(str_cpy);
    free(items);
    free(ind_to_remove);
    free(parent_ind);
}

/**
 * @brief Smaze prazdny adresar.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void rmdir(char **input) {
    int32_t address_to_remove = 0, parent_dir_address = 0;    
    int32_t num_of_splits_path = 0;
    inode *ind_to_remove, *parent_ind;
    char *str_cpy, **items;
    
    address_to_remove = get_last_item_of_path(input[1]);
    
    if (address_to_remove == -1) { // slozka, kterou chceme smazat neexistuje        
        printf("FILE NOT FOUND\n");        
        return;
    }    
    
    ind_to_remove = get_inode(address_to_remove);

    if (ind_to_remove->isDirectory == false) { // slozka ke smazani je ve skutecnosti soubor
        printf("FILE NOT FOUND\n");
        free(ind_to_remove);
        return;
    }

    if (is_ind_empty(ind_to_remove) == false) { // slozka neni prazdna
        printf("NOT EMPTY\n");
        free(ind_to_remove);
        return;
    }

    parent_dir_address = parse_path(input[1]); // uz vime, ze cesta existuje - nekontrolujem
    parent_ind = get_inode(parent_dir_address);
    
    if (strcmp(input[1], "/") != 0) { // nechceme si smazat domovsky adresar :)
        str_cpy = create_string_copy(input[1]); // chceme vedet jmeno polozky, kterou chceme vymazat
        items = str_split(str_cpy, '/', &num_of_splits_path);

        remove_dir_item(parent_ind, items[num_of_splits_path - 1]);
        set_bit_bitmap_inode(ind_to_remove->nodeid, 0); // uvolni uzel po odstranene slozce

        free(str_cpy);
        free(items);
    }

    printf("OK\n");
    
    free(ind_to_remove);
    free(parent_ind);
}

/**
 * @brief Vstoupi do slozky. Zajisti zmenu aktualni cesty a aktualniho inodu. Zatim nefunguje cd ..
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void cd(char **input) {
    int32_t target_ind_address = 0;    
    inode *ind;
    
    target_ind_address = get_last_item_of_path(input[1]);
    
    if (target_ind_address == -1) { // slozka do ktere chceme neexistuje
        //printf("cd: druha podmiinka\n");   
        printf("PATH NOT FOUND\n");        
        return;
    }
    
    ind = get_inode(target_ind_address);

    if (ind->isDirectory == false) { // cilova slozka je ve skutecnosti soubor
        printf("PATH NOT FOUND\n");        
        free(ind);        
        return;
    }   
    
    set_actual_path(input[1]);
    //printf("cd: target addres: %d", target_ind_address);
    set_actual_inode(target_ind_address);
    
    free(ind);    
    printf("OK\n");    
}

/**
 * @brief Vypise obsah aktualniho adresare do konzole.
 * 
 */
void ls(char **input) {     
    int32_t i = 0;
    int32_t block_address1 = 0, block_address2 = 0; 
    int32_t target_address = 0;    
    int32_t number_of_blocks = 0;
    inode *ind;
    
    target_address = get_last_item_of_path(input[1]);
    if (target_address == -1) {
        printf("PATH NOT FOUND\n");
        return;
    }

    ind = get_inode(target_address); // ziskavame adresar, ze ktereho budeme vypisovat

    if (ind->isDirectory == false) { // slozka k vypsani je ve skutecnosti soubor
        printf("PATH NOT FOUND\n");        
        free(ind);        
        return;
    }

    //printf("ls: inode: %d\n", ind->nodeid);

    number_of_blocks = ind->file_size / sizeof(directory_item);
    for (i = 1; i <= number_of_blocks; i++) { // prohledavame po jednom datove bloky uzlu, do file_size                  
        if (i < 6) { // prohledavame direct
            if (i == 1) { // direct 1
                write_item_name_to_console(ind->direct1);
            }                    
            else if (i == 2) { // direct 2
                write_item_name_to_console(ind->direct2);
            }                   
            else if (i == 3) {  // direct 3
                write_item_name_to_console(ind->direct3);
            }                    
            else if (i == 4) { // direct 4
                write_item_name_to_console(ind->direct4);
            }                    
            else if (i == 5) { // direct 5
                write_item_name_to_console(ind->direct5);
            }
        }
        else { // prohledavame indirect
            if ((i - 6) >= (int32_t)(SIZE_OF_CLUSTER/sizeof(int32_t))) { // indirect 2             
                if ((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t)) == 0) { // indirecty 2 stupne
                    fseek(fs, ind->indirect2 + (((i - 6) / (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // -1 ???
                    fread(&block_address1, sizeof(int32_t), 1, fs);
                }

                fseek(fs, block_address1 + (((i - 6) % (SIZE_OF_CLUSTER / sizeof(int32_t))) * sizeof(int32_t)), SEEK_SET); // indirecty 1 stupne
                fread(&block_address2, sizeof(int32_t), 1, fs);
                
                write_item_name_to_console(block_address2);
            }
            else { // indirect 1                
                fseek(fs, ind->indirect1 + (i - 6) * sizeof(int32_t), SEEK_SET); // prohledavame od prvniho bloku (nulteho :), ktery je v odkazech
                fread(&block_address1, sizeof(int32_t), 1, fs);

                write_item_name_to_console(block_address1);
            }
        }      
    }    
    free(ind);
}

/**
 * @brief Vytvori slozku.
 * 
 * @param input [1] Nazev slozky, nebo cesta zadana uzivatelem.
 */
void mkdir(char **input) {
    int32_t parent_directory_addr = 0; // slozka, kde chceme danou slozku vytvorit
    int32_t num_of_splits_path = 0;
    int32_t contain = 0;
    char **items;
    char *str_cpy;
    inode *ind;
    
    parent_directory_addr = parse_path(input[1]);
    //printf("mkdir : parent_dir_addr: %d, adresa zacatku uzlu: %d\n", parent_directory_addr, sp->inode_start_address);

    if (parent_directory_addr == -1) {
        printf("PATH NOT FOUND\n");        
        return;     
    }    
    else {
        str_cpy = create_string_copy(input[1]);
        items = str_split(str_cpy, '/', &num_of_splits_path);
        ind = get_inode(parent_directory_addr);
        
        contain = get_item_from_directory(ind, items[num_of_splits_path - 1]); // kontrola, zda to slozka jiz neobsahuje
        if (contain != -1) { // obsahuje
            printf("EXIST\n");
            free(ind);
            free(items);
            free(str_cpy);
            return;
        }
        else { // je to v poho            
            int32_t new_node_addr = get_free_inode();
            if (new_node_addr == -1) {
                printf("EROR - disk is full.");
                free(ind);
                free(items);
                free(str_cpy);
                return;
            }
                      
            set_bit_bitmap_inode(new_node_addr, 1); // obsad uzel    
            
            // napis directory item do rodicovske slozky            
            dir_it_tmp->inode = new_node_addr;
            create_item_name(items[num_of_splits_path - 1]);
            write_directory_item(parent_directory_addr); // co zapsat predano v dir_it_tmp
            
            //printf("mkdir: adresa noveho uzlu: %d\n", new_node_addr);

            /*
            int32_t addr = 0;
            fseek(fs, actual_ind->nodeid + sizeof(int32_t) * 3 + sizeof(bool), SEEK_SET);
            fread(&addr, sizeof(int32_t), 1, fs);            
            */
            create_inode(new_node_addr, parent_directory_addr, true); // zapiseme novy uzel do fs
            /*
            fseek(fs, actual_ind->nodeid + sizeof(int32_t) * 3 + sizeof(bool), SEEK_SET);
            fread(&addr, sizeof(int32_t), 1, fs);            
            */
            printf("OK\n");

            free(ind);
            free(items);
            free(str_cpy);
        }
    }
}

/**
 * @brief Vypise aktualni cestu do konzole.
 * 
 */
void pwd() {
    printf("%s\n", actual_path);
}

/**
 * @brief Zajisti vsechny potrebne veci pro zformatovani souboru.
 * 
 * @param input vstup od uzivatele
 */
void format(char **input, char *fs_name) {
    int num_of_splits = 0;
    char *str_cpy = create_string_copy(input[1]);
    char **sizeC = str_split(str_cpy, 'M', &num_of_splits);
    double sizeD = 0;
    int32_t sizeI = 0;    
    
    if (sscanf(sizeC[0], "%lf", &sizeD) != 1) { // neni cislo
        printf("Size isn't number.");
        free(sizeC);
        free(str_cpy);
        return;
    } 

    sizeD = sizeD * MB_TO_B;

    if (sizeD > MAX_SIZE) { // zadane cislo je moc velke
        printf("Entered size is too big.\n");
        free(sizeC);
        free(str_cpy);
        return;
    }

    fclose(fs);    // znovu otevirame soubor, pro smazani dat
    fs = fopen(fs_name, "wb+");
    if (fs == NULL) {
        printf("CANNOT CREATE FILE\n");
        free(sizeC);
        free(str_cpy);
        return;
    }

    sizeI = sizeD;
    sizeI = sizeI - (sizeI % SIZE_OF_CLUSTER);    
    
    create_superblock(sizeI);
    create_bitmaps();
    create_inodes();
    create_data_blocks();
    create_home_dir();

    set_actual_inode(sp->inode_start_address);
    memset(actual_path, '\0', sizeof(char) * ACTUAL_PATH_LENGHT);
    actual_path[0] = '/';    

    printf("OK\n");
    free(sizeC);
    free(str_cpy);
}