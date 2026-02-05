/**
 * @file input_handler.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief 
 * @version 1.0
 * @date 2022-02-
 */

#ifndef input_handler
#define input_handler

/**
 * @brief Vrati delku souboru v bytech.
 * 
 * @param fptr Soubor, u ktereho chceme zjistit delku.
 * @return Delku souboru v bytech.
 */
int32_t get_file_lenght(FILE *fptr);

/**
 * @brief Rozdeli vstup od uzivatele. Delici znak je mezera. Podle prikazu zavola danou
 *        funkci, ktera prikaz vykona.
 * 
 * @param arg_input Vstup od uzivatele.
 * @param fs_name Nazev souboru, kde je fs.
 */
void parse_input(char *arg_input, char *fs_name);

/**
 * @brief Zajistuje nacitani prikazu od uzivatele.
 * 
 * @param fs_name Jmeno souboru, kde je fs.
 */
void listening(char *fs_name);

/**
 * @brief Nacte vstup z konzole.
 * 
 * @return char* Text z konzole.
 */
char * get_line();

/**
 * @brief Rozdeli string na casti podle deliciho znaku.
 * 
 *    neobvykle pripady priklad, delim '/'
 *                "hola" --> numsplits = 1
 *                "/hola" --> numsplits = 2
 *                "hola/" --> numsplits = 2
 *                "/" --> numsplits = 2
 * @param str Srting, ktery se ma rozdelit.
 * @param delim Delici znak.
 * @param numSplits Pocet rozdelni.
 * @return char** Pole stringu.
 */
char** str_split(char *str, char delim, int *numSplits);

/**
 * @brief Odstrani znak nove radky ze vstupu a nahradi ho znakem '\0'.
 * 
 * @param arg_input String, kde se ma znak '\n' nahradit '\0'.
 */
void delete_nl_char(char *arg_input);

#endif