/*
  Fichier builtin.c: gestion des commandes
  Auteur : Josselin ABEL et Jimmy WIMS
  Dépendances : builtin.h
*/

#include <signal.h>
#include <sys/types.h>
#include "builtin.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
Fonction builtin_exit
Paramètre pid : pid du programme principale
Aucun retour
*/
void builtin_exit(pid_t pid)
{
	kill(pid,2);
}

/*
Fonction builtin_cd
Paramètre path : pointeur vers le path du nouveau dossier
Retourne 0 si le changement de dossier a fonctionner et -1 sinon
*/
int builtin_cd(char *path)
{
	if(chdir(path)==0)
		return 0;
	else
	{
		printf("chemin %s : inexistant \n",path);
		return -1;
	}
}

/*
Fonction builtin_export
Paramètre name : nom de la variable à remplir
Paramètre value : valeur de la nouvelle variable
Retourne 0 si l'affectation a fonctionner et -1 sinon
*/
int builtin_export(char* name,char* value)
{
	if(setenv(name,value,1)==0)
		return 0;
	printf("probleme d'affectation");
	return -1;
}

/*
Fonction builtin_unset
Paramètre name : nom de la variable à vider
Retourne 0 si l'affectation a fonctionner et -1 sinon
*/
int builtin_unset(char* name)
{
	if(unsetenv(name)==0)
		return 0;
	printf("probleme de supression");
	return -1;
}