/**
 * @file get_functions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru get_functions.c.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef get_functions
#define get_functions

/**
 * @brief Vrati jmeno posledni polozky v adresari.
 * 
 * @param ind Adresar, ve kterem hledame.
 * @return char* Jmeno posledni polozky.
 */
char *get_name_of_last_item(inode *ind);

/**
 * @brief Vrati adresu na blok, kde lze cist.
 * 
 * @param ind Uzel, ze ktereho chceme cist.
 * @param num_of_block Poradi datoveho bloku, ze ktereho chceme cist, v uzlu.
 * @return Adresa na datovy blok.
 */
int32_t get_block_to_read(inode *ind, int32_t num_of_block);

/**
 * @brief Vrati adresu datoveho bloku uzlu, kam lze psat.
 * 
 * @param ind Uzel, u ktereho chceme blok ke psani najit.
 * @param file_or_dir == SIZE_OF_CLUSTER, kdyz soubor,
 *                    == sizeof(directory_item), kdyz slozka.
 * @return Adresu datoveho bloku.
 */
int32_t get_block_to_write(inode *ind, int32_t file_or_dir);

/**
 * @brief Vrati adresu posledniho datoveho bloku uzlu.
 * 
 * @param ind Uzel u ktereho chceme zjisit ten posledni dat. blok.
 * @param file_or_dir Zda se jedna o slozku nebo soubor,
 *                    SIZE_OF_CLUSTER - slozka,
 *                    sizeof(directory_item) - soubor.
 * @return int32_t 
 */
int32_t get_last_data_block(inode *ind, int32_t file_or_dir);

/**
 * @brief Vrati adresu posledni polozky cesty. (Parse path vraci predposleni polozku cesty.)
 * 
 * @param path 
 * @return -1 kdyz cesta neexistuje,
 *         adresu polozky, kdyz cesta existuje.
 */
int32_t get_last_item_of_path(char *path);

/**
 * @brief Vrati adresu polozky, kdyz ji obsahuje.
 * 
 * @param data_block_addr Blok, ve kterem hledame.
 * @param item Polozk, kterou hledame.
 * @return -1 kdyz blok polozku neobsahuje
 *          adresu polozky, kdyz adresu blok obsahuje.
 */
int32_t get_item_from_datablock(int32_t data_block_addr, char *item);

/**
 * @brief Prohleda adresar, hleda item.
 * 
 * @param ind Adresar k proheldani
 * @param item Polozka kterou hledame
 * @return int32_t  -1, kdyz nenalezeno
 *                  adresu polozku, kdyz nalezeno
 */
int32_t get_item_from_directory(inode *ind, char *item);

/**
 * @brief Vrati adresu volneho uzlu.
 * 
 * @return Adresu volneho uzlu. 
 *         -1, kdyz neexistuje volny uzel.
 */
int32_t get_free_inode();

/**
 * @brief Vrati adresu volneho datoveho bloku.
 * 
 * @return Adresu volneho datoveho bloku.
 *         -1 pokud nezbyva volny blok.
 */
int32_t get_free_datablock();

/**
 * @brief Vrati ukazatel na inode, podle zadane adresy uzlu.
 * 
 * @param node_address Adresa uzlu, ktery chceme najit.
 * @return Ukazatel na uzel.
 */
inode * get_inode(int32_t node_address);

#endif