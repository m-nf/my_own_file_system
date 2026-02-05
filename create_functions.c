/**
 * @file create_functions.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje funkce s funkcionalitou typu create.
 * @version 1.0
 * @date 2022-02-11
 */

#include "definitions.h"

/**
 * @brief Zapise novy adresar do fs.
 * 
 * @param new_node_adress Adresa noveho adresare.
 * @param parent_node_address Adresa rodicovskeho adresare.
 */
void create_inode(int32_t new_node_adress, int32_t parent_node_address, bool isDirectory) {
    inode *ind = (inode *) malloc(sizeof(inode));

    ind->nodeid = new_node_adress;
    ind->isDirectory = isDirectory;
    ind->parent_directory_address = parent_node_address;
    ind->file_size = 0;
    ind->direct1 = -1;
    ind->direct2 = -1;
    ind->direct3 = -1;
    ind->direct4 = -1;
    ind->direct5 = -1;
    ind->indirect1 = -1;
    ind->indirect2 = -1;

    fseek(fs, new_node_adress, SEEK_SET);
    fwrite(&ind->nodeid, sizeof(int32_t), 1, fs);
    fwrite(&ind->isDirectory, sizeof(bool), 1, fs);
    fwrite(&ind->parent_directory_address, sizeof(int32_t), 1, fs);
    fwrite(&ind->file_size, sizeof(int32_t), 1, fs);
    fwrite(&ind->direct1, sizeof(int32_t), 1, fs);
    fwrite(&ind->direct1, sizeof(int32_t), 1, fs);   
    fwrite(&ind->direct2, sizeof(int32_t), 1, fs);
    fwrite(&ind->direct3, sizeof(int32_t), 1, fs);
    fwrite(&ind->direct4, sizeof(int32_t), 1, fs);
    fwrite(&ind->direct5, sizeof(int32_t), 1, fs);
    fwrite(&ind->indirect1, sizeof(int32_t), 1, fs);
    fwrite(&ind->indirect2, sizeof(int32_t), 1, fs);
    
    free(ind);
}

/**
 * @brief Vytvori jmeno polozky --> doplni '\0' do 12.
 * 
 * @param name jmeno k vytvoreni.
 */
void create_item_name(char *name) {
    memset(dir_it_tmp->item_name, '\0', sizeof(char) * NAME_LENGHT);    
    strcpy(dir_it_tmp->item_name, name);
}

/**
 * @brief Vytvori kopii stringu.
 * 
 * @param input String jehoz kopii chceme vyvtorit
 * @return Vrati ukazatel na string. Jeho nutno ho potom free().
 */
char * create_string_copy(char *input) {    
    int32_t lenght = 0;
    char *output;

    while(input[lenght] != '\0') {
        lenght++;
    }

    output = (char *) malloc(sizeof(char) * lenght);    

    strcpy(output, input);

    return output;
}

/**
 * @brief Vytvori superblok a zapise ho do fs.
 * 
 * @param fs_size Velikost v bytech zadana uzivatelem.
 */
void create_superblock(int32_t fs_size) {
    memset(sp->signature, 0, sizeof(char) * SIGNATURE_LENGHT);
    memset(sp->volume_descriptor, 0 , sizeof(char) * SIZE_OF_DESCRIPTION);
    int32_t cluster_count = (fs_size - sizeof(superblock)) / (2 + sizeof(inode) +SIZE_OF_CLUSTER);
    //printf("%d", cluster_count);

    /* setting superblock */    
    sprintf(sp->signature, "nonfried");
    sp->disk_size = fs_size;  
    sp->cluster_size = SIZE_OF_CLUSTER;
    sp->cluster_count = cluster_count;
    sp->bitmapinode_start_address = sizeof(superblock);
    sp->bitmapdata_start_address = sizeof(superblock) + sp->cluster_count;
    sp->inode_start_address = sizeof(superblock) + (sp->cluster_count * 2);
    sp->data_start_address = sizeof(superblock) + (sp->cluster_count * 2) + (sizeof(inode) * sp->cluster_count);
    sprintf(sp->volume_descriptor, "Signature: %s\nDisk size: %d B\nSize of cluster: %d B\nCluster count: %d\n", sp->signature, 
                                                                                                            sp->disk_size,
                                                                                                            sp->cluster_size,
                                                                                                            sp->cluster_count);
    /* writing superblock */
    fseek(fs, 0, SEEK_SET);
    fwrite(sp->signature, sizeof(char), SIGNATURE_LENGHT, fs);
    fwrite(sp->volume_descriptor, sizeof(char), SIZE_OF_DESCRIPTION, fs);
    fwrite(&sp->disk_size, sizeof(int32_t), 1, fs);
    fwrite(&sp->cluster_size, sizeof(int32_t), 1, fs);
    fwrite(&sp->cluster_count, sizeof(int32_t), 1, fs);
    fwrite(&sp->bitmapinode_start_address, sizeof(int32_t), 1, fs);
    fwrite(&sp->bitmapdata_start_address, sizeof(int32_t), 1, fs);
    fwrite(&sp->inode_start_address, sizeof(int32_t), 1, fs);
    fwrite(&sp->data_start_address, sizeof(int32_t), 1, fs);
}

/**
 * @brief Pripravi bitmapy a zapise je do fs.
 * 0 false, 1 true
 * 
 */
void create_bitmaps() {
    char *zeros = (char *) malloc(sizeof(char) * sp->cluster_count);
    memset(zeros, 0, sizeof(char) * sp->cluster_count);

    /* inode bitmap */
    fseek(fs, sp->bitmapinode_start_address, SEEK_SET);
    fwrite(zeros, sizeof(char), sp->cluster_count, fs);

    /* data bitmap */
    fseek(fs, sp->bitmapdata_start_address, SEEK_SET);
    fwrite(zeros, sizeof(char), sp->cluster_count, fs);

    free(zeros);
}

/**
 * @brief Inicializuje uzly a zapise je do fs.
 * 
 */
void create_inodes() {
    int i = 0;

    actual_ind->nodeid = sp->inode_start_address;
    actual_ind->isDirectory = true;
    actual_ind->parent_directory_address = -1;
    actual_ind->file_size = 0;
    actual_ind->direct1 = -1;
    actual_ind->direct2 = -1;
    actual_ind->direct3 = -1;
    actual_ind->direct4 = -1;
    actual_ind->direct5 = -1;
    actual_ind->indirect1 = -1;
    actual_ind->indirect2 = -1;

    for (i = 0; i < sp->cluster_count; i++) {        
        fwrite(&actual_ind->nodeid, sizeof(int32_t), 1, fs);

        fwrite(&actual_ind->isDirectory, sizeof(bool), 1, fs);
        fwrite(&actual_ind->parent_directory_address, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->file_size, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->direct1, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->direct2, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->direct3, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->direct4, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->direct5, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->indirect1, sizeof(int32_t), 1, fs);
        fwrite(&actual_ind->indirect2, sizeof(int32_t), 1, fs);
        
        actual_ind->nodeid += sizeof(inode);
    }
}

/**
 * @brief Inicializuje datove bloky a zapise je do fs.
 * 
 */
void create_data_blocks() {
    int32_t i = 0;    
    char *new_block = (char *) malloc(sizeof(char) * SIZE_OF_CLUSTER); // one block
    memset(new_block, 0, sizeof(char) * SIZE_OF_CLUSTER);

    fseek(fs, sp->data_start_address, SEEK_SET);
    for (i = 0; i < sp->cluster_count; i++) { // zapisujem jeden blok po cluster_count
        fwrite(new_block, sizeof(char), SIZE_OF_CLUSTER, fs);
    }

    free(new_block);
}

/**
 * @brief Vytvori domovsky adresar. Zajisti nastaveni cesty.
 * 
 */
void create_home_dir() {
    set_bit_bitmap_inode(sp->inode_start_address, 1);
    set_bit_bitmap_data(sp->data_start_address, 1);
    
    actual_ind->nodeid = sp->inode_start_address;
    actual_ind->isDirectory = true;
    actual_ind->parent_directory_address = sp->inode_start_address;
    actual_ind->file_size = sizeof(directory_item);
    actual_ind->direct1 = sp->data_start_address;
    
    fseek(fs, sp->inode_start_address, SEEK_SET);
    fwrite(&actual_ind->nodeid, sizeof(int32_t), 1, fs);
    fwrite(&actual_ind->isDirectory, sizeof(bool), 1, fs);
    fwrite(&actual_ind->parent_directory_address, sizeof(int32_t), 1, fs);
    fwrite(&actual_ind->file_size, sizeof(int32_t), 1, fs);
    fwrite(&actual_ind->direct1, sizeof(int32_t), 1, fs);

    fseek(fs, sp->data_start_address, SEEK_SET);
    fwrite(&actual_ind->nodeid, sizeof(int32_t), 1, fs);
    create_item_name("/");
    fwrite(&dir_it_tmp->item_name, sizeof(char), NAME_LENGHT, fs);
}