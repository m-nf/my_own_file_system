/**
 * @file create_functions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru create_functions.c.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef create_functions
#define create_functions

/**
 * @brief Zapise novy adresar do fs.
 * 
 * @param new_node_adress Adresa noveho adresare.
 * @param parent_node_address Adresa rodicovskeho adresare.
 */
void create_inode(int32_t new_node_adress, int32_t parent_node_address, bool isDirectory);

/**
 * @brief Vytvori jmeno polozky --> doplni '\0' do 12.
 * 
 * @param name jmeno k vytvoreni.
 */
void create_item_name(char *name);

/**
 * @brief Vytvori kopii stringu.
 * 
 * @param input String jehoz kopii chceme vyvtorit
 * @return Vrati ukazatel na string. Jeho nutno ho potom free().
 */
char * create_string_copy(char *input);

/**
 * @brief Vytvori superblok a zapise ho do fs.
 * 
 * @param fs_size Velikost v bytech zadana uzivatelem.
 */
void create_superblock(int32_t fs_size);

/**
 * @brief Pripravi bitmapy a zapise je do fs.
 * 0 false, 1 true
 * 
 */
void create_bitmaps();

/**
 * @brief Inicializuje uzly a zapise je do fs.
 * 
 */
void create_inodes();

/**
 * @brief Inicializuje datove bloky a zapise je do fs.
 * 
 */
void create_data_blocks();

/**
 * @brief Vytvori domovsky adresar. Zajisti nastaveni cesty.
 * 
 */
void create_home_dir();

#endif