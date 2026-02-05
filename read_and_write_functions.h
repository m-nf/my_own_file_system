/**
 * @file read_and_write_funtions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru read_and_write_functions.c.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef read_and_write_functions
#define read_and_write_functions

/**
 * @brief Vypise bloky ve kterych se soubor nachazi.
 * 
 * @param ind Uzel u ktereho se maji bloky vypsat.
 */
void write_blocks_numbers_to_console(inode *ind, int32_t file_or_dir);

/**
 * @brief Nacte soubor z fs a vrati ho v pobobe *char. Je nutno ukazatel pak uvolnit.
 * 
 * @param ind Uzel, ze ktereho chceme oubor cist.
 * @return Vrati ukazatel na *char, kde je nacten soubor z fs.
 */
char * read_file_from_fs(inode *ind);

/**
 * @brief Zapise soubor do fs.
 * 
 * @param file Soubor v podobe *char, ktery se ma zapsat.
 * @param file_lenght Delka souboru.
 * @param node_address Adresa inodu, kam se ma soubor zapsat.
 */
void write_file_to_fs(char *file, int32_t file_lenght, int32_t node_address);

/**
 * @brief Vypise jmeno polozky adresare. Polozka je ulozena v jednom bloku, ktery je predan parametrem.
 * 
 * @param address_of_block Adresa datoveho bloku, kde se nachazi polozka, jejiz jmeno se ma vypsat.
 */
void write_item_name_to_console(int32_t address_of_block);

/**
 * @brief Zapise polozku do adresare. Polozka je predana v dir_it_tmp.
 * 
 * @param node_address V jakem adresari chceme dir_item zapsat.
 */
void write_directory_item(int32_t node_address);

#endif