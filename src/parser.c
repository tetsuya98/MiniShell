#include "parser.h"
#include "processus.h"

#include <string.h>


#include <stdio.h>

// Supprimer les espaces en début et fin de ligne
int trim(char* str) {
    size_t length = strlen(str);
    char* c = str;
    
    // Suppression des espaces au début
    while (*c != '\0' && (*c == ' ' || *c == '\t')) ++c;
    memmove(str, c, length - (c - str) + 1);
    
    // Suppression des espaces à la fin
    for (c = str + strlen(str)-1;
         c > str && (*c == ' ' || *c == '\t');
         --c);
    *(c+1)='\0';
    
    return 0;
}

// Suppression des doublons d'espaces
int clean(char* str) {
    char* sp, *tab, *c;
    
    while (str != NULL) {
        sp = strchr(str, ' ');
        tab = strchr(str, '\t');
        if (sp != NULL && tab != NULL)
            str = (sp < tab ? sp:tab);
        else {
            if (sp != NULL) str = sp;
            else str = tab;
        }
        if (str == NULL) break;
        // Suppression des espaces multiples
        c = str;
        while (*c != '\0' && (*c == ' ' || *c == '\t')) ++c;
        memmove(str+1, c, strlen(str) - (c - str) + 1);
        *str=' ';
        str++;
    }
    return 0;
}

// Découpe la chaîne str et enregistre les pointeurs dans tab
size_t split_str(char* str, char* tab[], size_t max_size) {
    char* prev = str, *c;
    size_t nb=0;
    
   // printf("split2");
    while ((c=strchr(str, ' ')) != NULL && nb<max_size) {
     //   printf("ok");
        tab[nb++]=prev;
        str = c+1;
        *c='\0';
        prev=str;
    }
    tab[nb++] = prev;
    return nb;
}

// Découpe le tableau suivant les séparateurs de commandes
// retourne la position dans le tableau de la prochaine
// commande et place le type de séparateur dans *sep
int split_command(char* tab[], separator_t* sep) {
    int i = 0;
    
    while ((tab[i] != NULL) &&
           (strcmp(tab[i], "&") != 0) &&
           (strcmp(tab[i], "&&") != 0) &&
           (strcmp(tab[i], "||") != 0) &&
           (strcmp(tab[i], ";") != 0) &&
           (strcmp(tab[i], "|") != 0) &&
            (strcmp(tab[i], "=") != 0)) 
        ++i;
    if (tab[i]==NULL) { *sep = END; return -1; }
    if (strcmp(tab[i], "&") == 0) *sep = PARAL;
    if (strcmp(tab[i], "&&") == 0) *sep = AND;
    if (strcmp(tab[i], "||") == 0) *sep = OR;
    if (strcmp(tab[i], ";") == 0) *sep = SEMICOL;
    if (strcmp(tab[i], "|") == 0) *sep = PIPE;
    if (strcmp(tab[i], "=") == 0) *sep = EGAL;
    tab[i] = NULL;
    
    return i+1;
}

// Analyse des redirections :
//  - Remplace la chaine de redirection par NULL
//  - Retourne l'indice du chemin du fichier de
//    redirection dans tab
//  - enregistre le type de redirection dans *redir
int redirection(char* tab[], redirection_t* redir) {
    char** str;
    int i = 0;
    
    for (str=tab; *str!=NULL; ++str) {
        if (strcmp(*str, "<") == 0) {
            *redir = INPUT;
            *str = NULL;
            return i + 1;
        }
        if (strcmp(*str, ">") == 0) {
            *redir = OUTPUT;
            *str = NULL;
            return i + 1;
        }
        if (strcmp(*str, "2>") == 0) {
            *redir = ERROR;
            *str = NULL;
            return i + 1;
        }
        if (strcmp(*str, ">>") == 0) {
            *redir = OUTPUT_APPEND;
            *str = NULL;
            return i + 1;
        }
        if (strcmp(*str, "2>>") == 0) {
            *redir = ERROR_APPEND;
            *str = NULL;
            return i + 1;
        }
        ++i;
    }
    return -1;
}
