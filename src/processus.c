/*
  Fichier processus.c: gestion des l'éxecution des commandes et de la lecture desz chaine saisie
  Auteur : Josselin ABEL et Jimmy WIMS
  Dépendances : processus.h
*/

#include <string.h>
#include <stdlib.h>
#include "processus.h"
#include <stdio.h>

/*
Fonction exec_process
Paramètre p : pointeur vers le processus à executer
Retourne -1 si l'execution de la commande n'a pas fonctionné
*/
int exec_process(processus_t * p)
{
	p->pid=fork();
	if(p->pid==0)
	{
		dup2(p->stdin,0);
		dup2(p->stdout,1);
		dup2(p->stderr,2);
		if(p->stdin!=0) close(p->stdin);
		if(p->stdout!=1) close(p->stdout);
		if(p->stderr!=2) close(p->stderr);
		execv(p->path,p->argv,p->envp);
		exit(-1);
	}
}

/*
Fonction exec_cmdline
Paramètre process : pointeur vers le premier processus à executer
Aucun retour
*/
void exec_cmdline(processus_t * process)
{
	processus_t * p=process;
	while(p!=NULL)
	{
		exec_process(p);
		if(p->background==0)
		{
			waitpid(p->pid,&p->status,0);
			if(p->next_processus!=NULL)	//cmd1;cmd2
			{
				p=p->next_processus;
				continue;
			}
			if(p->next_success!=NULL) //cmd1 && cmd2
			{
				if(WEXITSTATUS(p->status)!=0) break;
				p=p->next_success;
				continue; 
			}
			if(p->next_failure!=NULL){ //cmd1 ||  cmd2
				if(WEXITSTATUS(p->status)==0)break;
				p=p->next_failure;
				continue;
			}
		}
		p=p->next_processus;
	}
}

/*
Fonction search_path
Paramètre p : pointeur vers le processus dont on doit chercher le path
Retourne -1 si l'execution de la commande n'a pas fonctionné
Aucun retour
*/
void search_path(processus_t * p){
	char * env_path = getenv("PATH");
	char *tmp;
	if((tmp=strchr(p->argv[0],'/'))!=NULL){		//exemple : /bin/ls
		if(tmp==p->argv[0]) strcpy(p->path,p->argv[0]);
		else{								//exemple : ./dir/executable
			char buffer[MAX_PATH_SIZE];
			strcpy(p->path,getcwd(buffer,MAX_PATH_SIZE));
			strcat(p->path,"/");
			strcat(p->path,p->argv[0]);
	}	
	}else{
		char *path[MAX_PATH_SIZE];
		strcpy(path,env_path);
		tmp=strtok(path,":");
		while(tmp!=NULL)
		{
			strcpy(p->path,tmp);
			strcat(p->path,"/");
			strcat(p->path,p->argv[0]);
			FILE *f=fopen(p->path,"r");
			if(f!=NULL)
			{
				fclose(f);
				return 0;
			}
			tmp=strtok(NULL,":");
		}
	}
}

/*
Fonction lire
Paramètre chaine : pointeur vers l'emplacement ou sera stocker la saisie
Paramètre longueur : entier représentant la longueur maximale de la saisie
Retourne 1 si la saisie est correcte et 0 sinon
*/
int lire(char *chaine, int longueur)
{

    char *positionEntree = NULL;

    // On lit le texte saisi au clavier

    if (fgets(chaine, longueur, stdin) != NULL)  // Pas d'erreur de saisie ?
    {
        positionEntree = strchr(chaine, '\n'); // On recherche l'"Entrée"
        if (positionEntree != NULL) // Si on a trouvé le retour à la ligne
        {
            *positionEntree = '\0'; // On remplace ce caractère par \0
        }
        return 1; // On renvoie 1 si la fonction s'est déroulée sans erreur
    }
    else
    {
        return 0; // On renvoie 0 s'il y a eu une erreur
    }
}