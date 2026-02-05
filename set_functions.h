/**
 * @file set_functions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru create_functions.c.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef set_functions
#define set_functions

/**
 * @brief Uvolni datove bloky souboru nad 5000 bytu.
 * 
 * @param ind Uzel, u ktereho chceme bloky uvolnit.
 */
void set_free_over_5000B(inode *ind);

/**
 * @brief Uvolni datove bloky souboru.
 * 
 * @param ind Uzel, u ktereho chceme bloky uvolnit.
 */
void set_free_blocks_of_file(inode *ind);

/**
 * @brief Nastavi aktualni cestu.
 * 
 * @param path Cesta, kterou zadal uzivatel.
 */
void set_actual_path(char *path);

/**
 * @brief Zmeni aktualni uzel v pameti na uzel zadanej parametrem.
 * 
 * @param node_address adresa uzlu, na ktery se aktualni uzel ma nastavit
 */
void set_actual_inode(int32_t node_address);

/**
 * @brief Nastavi bit v bitmape inodu.
 * 
 * @param node_address adresa uzlu, u ktereho se ma bit nastavit
 * @param isTaken bit, ktery se ma nastavit 0 - volno
 *                                          1 - obsazeno
 */
void set_bit_bitmap_inode(int32_t node_address, char isTaken);

/**
 * @brief Nastavi bit v bitmape datovych bloku.
 * 
 * @param datablock_address adresa bloku, u ktereho se ma bit nastavit
 * @param isTaken bit, ktery se ma nastavit 0 - volno
 *                                          1 - obsazeno
 */
void set_bit_bitmap_data(int32_t datablock_address, char isTaken);

#endif