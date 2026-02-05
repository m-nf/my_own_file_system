/**
 * @file commands_functions.h
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Obsahuje dekladrace funcki k souboru commands_functions.c.
 * @version 1.0
 * @date 2022-02-11
 */

#ifndef commands_functions
#define commands_functions

/**
 * @brief Vypise info o uzlu. Format:
 *        NAME – SIZE – i-node NUMBER – přímé a nepřímé odkazy
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru.
 */
void info(char **input);

/**
 * @brief Pokud je soubor delsi nez 5000 bytu, zkrati ho na 5000 bytu.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru.
 */
void short_func(char **input);

/**
 * @brief Spoji dva soubory dohromady do tretiho.
 * 
 * @param input [1] cesta k souboru 1.
 *              [2] cesta k souboru 2.
 *              [3] cesta k souboru 3.
 */
void xcp(char **input);

/**
 * @brief Presune souboru z umisteni na fs do umizteni na fs. Pripadne ho prejmenuje.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] Odkud chceme soubor presunout.
 *              [2] Kam chceme soubor presunout.
 */
void mv(char **input);

/**
 * @brief Zkopiruje soubor.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] Odkud chceme soubor kopirovat.
 *              [2] Kam chceme soubor kopirovat.
 */
void cp(char **input);

/**
 * @brief Vypise obsah souboru.
 * 
 * @param input 
 */
void cat(char **input);

/**
 * @brief Nacte soubor z fs a ulozi ho na pevny disk.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru na fs pro cteni
 *              [2] cesta k souboru na disku pro zapis
 */
void outcp(char **input);

/**
 * @brief Nahraje soubor z pevneho disku a ulozi ho do fs.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 *              [1] cesta k souboru na disku pro cteni
 *              [2] cesta k souboru na fs pro zapis
 */
void incp(char **input);

/**
 * @brief Nacte soubor s prikazy a vykona je. Jeden prikaz - jedna radka. 
 *        Konec radky '\n'.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void load(char **input, char *fs_name);

/**
 * @brief Odstrani soubor z adresare.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void rm(char **input);

/**
 * @brief Smaze prazdny adresar.
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void rmdir(char **input);

/**
 * @brief Vstoupi do slozky. Zajisti zmenu aktualni cesty a aktualniho inodu. Zatim nefunguje cd ..
 * 
 * @param input Vstup od uzivatele, rozdeleny mezerama.
 */
void cd(char **input);

/**
 * @brief Vypise obsah aktualniho adresare do konzole.
 * 
 */
void ls(char **input);

/**
 * @brief Vytvori slozku.
 * 
 * @param input [1] Nazev slozky, nebo cesta zadana uzivatelem.
 */
void mkdir(char **input);

/**
 * @brief Vypise aktualni cestu do konzole.
 * 
 */
void pwd();

/**
 * @brief Zajisti vsechny potrebne veci pro zformatovani souboru.
 * 
 * @param input vstup od uzivatele
 */
void format(char **input, char *fs_name);

#endif