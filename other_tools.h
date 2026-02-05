/**
 * @file other_tools.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru other_tools.c
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef other_tools
#define other_tools

/**
 * @brief Odstrani soubor a vymaze ho z rodicovskeho adresare. Uvolni uzel souboru.
 * 
 * @param parent_ind Adresar, ze ktereho se ma soubor odstranit. 
 * @param ind_to_remove Uzel, ktery se ma odstranit.
 * @param item_to_remove Jmeno souboru, ktery se ma odstranit.
 */
void remove_file(inode *parent_ind, inode *ind_to_remove, char *item_to_remove);

/**
 * @brief Zjisti, zda je polozka v adresari ulozena v poslendim datovem bloku v uzlu, nebo ne.
 * 
 * @param parent_ind Adresar k prohledani
 * @param item Polozka, kterou hledame.
 * @return true Je posledni.
 * @return false Neni posledni.
 */
bool is_it_last_item(inode *parent_ind, char* item);

/**
 * @brief Ostrani z adresare referenci na posledni polozku.
 * 
 * @param ind Adresar, odkud se ma reference odstranit.
 */
void remove_ref_to_last_data_block(inode *ind);

/**
 * @brief Odstrani slozku z rodicovskeho adresare. Neuvolni uzel polozky.
 * 
 * @param parent_ind Adresar, ze ktereho se ma polozka odstranit. 
 * @param ind_to_remove Uzel, ktery se ma odstranit.
 * @param item_to_remove Jmeno polozky, ktery se ma odstranit.
 */
void remove_dir_item(inode *parent_ind, char *item_to_remove);

/**
 * @brief Prepise odkaz na polozku v adresari na jinou polozku. Prepise jak jmeno, tak adresu.
 * 
 * @param parent_ind Rodicovska slozka.
 * @param item_to_remove Jmeno polozky k odstraneni/prepsani.
 * @param item_to_write Jmeno polozky k zapsani.
 * @param address_to_write Adresa polozky k zapsani.
 */
void rewrite_item_in_dir(inode *parent_ind, char *item_to_remove, char *item_to_write, int32_t address_to_write);

/**
 * @brief Rozhodne, zda je dany uzel prazdny.
 * 
 * @param ind Uzel u ktereho zjistujem, zda je prazdny.
 * @return true - kdyz dany uzel nic neobsahuje.
 * @return false - kdyz dany uzel neco obsahuje (slozku, nebo soubor).
 */
bool is_ind_empty(inode *ind);

/**
 * @brief Vybere novy datovy/odkazovy blok a zapise jeho adresu do uzlu.
 * 
 * @param ind Uzel, kteremu novy blok hledame.
 * @param num_direct_data_blok Znacka, kam mame novy blok zapsat.
 */
void assign_direct_data_block(inode *ind, int32_t num_direct_data_blok);

/**
 * @brief Priradi novy datovy blok a jeho adresu zapise do odkazoveho bloku.
 * 
 * @param address Adresa, kam se adresa noveho datoveho bloku ma zapsat.
 */
void assign_data_block_to_indirect1(int32_t address);

/**
 * @brief Priradi novy odkazovy blok a zapise jeho adresu do odkazoveho bloku uzlu.
 *        Novy blok se naplni -1, po intech.
 * 
 * @param address adresa, kam chceme zapsat adresu noveho odkazoveho bloku.
 */
void assign_reference_block_to_indirect2(int32_t address);

/**
 * @brief Vrati adresu predposleni polozky cesty predane v path.
 * 
 * @param path Cesta zadana uzivatelem.
 * @return -1 neplatna cesta
 *         adresu predposledni polozky cesty
 */
int32_t parse_path(char *path);

#endif 
