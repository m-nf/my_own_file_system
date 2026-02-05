/**
 * @file input_handler.c
 * @author Max Nonfried (nonfried@students.zcu.cz)
 * @brief Zajistuje vstup od uzivatele, ktery nacita z konzole a podle prikazu vola danou funkci.
 * @version 1.0
 * @date 2022-02-
 */

#include "definitions.h"

/**
 * @brief Zajistuje nacitani prikazu od uzivatele.
 * 
 * @param fs_name Jmeno souboru, kde je fs.
 */
void listening(char *fs_name) {
    char *input;
    while (1) {
        input = get_line();
        
        if (strcmp(input, "exit\n") == 0) {
            free(input);
            break;
        }        

        parse_input(input, fs_name);

        free(input);
    }
    
}

/**
 * @brief Odstrani znak nove radky ze vstupu a nahradi ho znakem '\0'.
 * 
 * @param arg_input String, kde se ma znak '\n' nahradit '\0'.
 */
void delete_nl_char(char *arg_input) {
    int32_t i = 0;    
    while(arg_input[i] != '\n') {
        i++;
    }
    arg_input[i] = '\0';
}

/**
 * @brief Rozdeli vstup od uzivatele. Delici znak je mezera. Podle prikazu zavola danou
 *        funkci, ktera prikaz vykona.
 * 
 * @param arg_input Vstup od uzivatele.
 * @param fs_name Nazev souboru, kde je fs.
 */
void parse_input(char *arg_input, char *fs_name) {
    char **input;
    int num_of_splits;

    delete_nl_char(arg_input);    

    input = str_split(arg_input, ' ', &num_of_splits); // obsahuje \n
    
    //printf("Vstup: %s", input[0]);

    /* podminky pro prikazy */
    if (strcmp(input[0], "cp") == 0) {
        cp(input);
    }
    else if (strcmp(input[0], "mv") == 0) {
        mv(input);
    }
    else if (strcmp(input[0], "rm") == 0) {
        rm(input);
    }
    else if (strcmp(input[0], "mkdir") == 0) { // zde
        mkdir(input);
    }
    else if (strcmp(input[0], "rmdir") == 0) {
        rmdir(input);
    }
    else if (strcmp(input[0], "ls") == 0) {
        ls(input);
    }
    else if (strcmp(input[0], "cat") == 0) {
        cat(input);
    }
    else if (strcmp(input[0], "cd") == 0) { // zde
       cd(input);
    }
    else if (strcmp(input[0], "pwd") == 0) { // zde
        pwd();
    }
    else if (strcmp(input[0], "info") == 0) {
        info(input);
    }
    else if (strcmp(input[0], "incp") == 0) { // zde
        incp(input);
    }
    else if (strcmp(input[0], "outcp") == 0) { 
        outcp(input);
    }
    else if (strcmp(input[0], "load") == 0) { 
        load(input, fs_name);
    }
    else if (strcmp(input[0], "format") == 0) {
        format(input, fs_name);
    }
    else if (strcmp(input[0], "xcp") == 0) {
        xcp(input);
    }
    else if (strcmp(input[0], "short") == 0) {
        short_func(input);
    }
    else {
        printf("Command doesn't found.\n");
    }

    free(input);
}

/**
 * @brief Nacte vstup z konzole.
 * 
 * @return char* Text z konzole.
 */
char * get_line() {
    char *line = malloc(100), *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL) {
        return NULL;
    }

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF) {
            break;
        }

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if((*line++ = c) == '\n') {
            break;
        }
    }
    *line = '\0';
    return linep;
}

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
char** str_split(char *str, char delim, int *numSplits) {
    char** ret;
    int retLen = 0;
    char* c;    

    if ((str == NULL) || (delim == '\0')) {        
        ret = NULL;
        retLen = -1;
    }
    else {
        retLen = 0;
        c = str;

        do {
            if (*c == delim) {
                retLen++;
            }
            c++;
        } while (*c != '\0');

        ret = malloc((retLen + 1) * sizeof(*ret));
        ret[retLen] = NULL;

        c = str;
        retLen = 1;
        ret[0] = str;

        do {
            if (*c == delim) {
                ret[retLen++] = &c[1];
                *c = '\0';
            }
            c++;
        } while (*c != '\0');
    }

    if (numSplits != NULL) {
        *numSplits = retLen;
    }

    return ret;
}

/**
 * @brief Vrati delku souboru v bytech.
 * 
 * @param fptr Soubor, u ktereho chceme zjistit delku.
 * @return Delku souboru v bytech.
 */
int32_t get_file_lenght(FILE *fptr) {
    int32_t file_lenght = 0;

    fseek(fptr, 0, SEEK_END);
    file_lenght = ftell(fptr);
    rewind(fptr);

    return file_lenght;
}