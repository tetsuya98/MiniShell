/*
  Fichier main.c: gestion des redirections, des operateur et de l'éxecution de la commande
  Auteur : Josselin ABEL et Jimmy WIMS
  Dépendances : processus.h parser.h builtin.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "processus.h"
#include "parser.h"
#include "builtin.h"

int main(int argc, char *argv[]){
	char str[MAX_STR_SIZE];
	char* tokens[MAX_TOKENS];
	processus_t cmds[MAX_CMD];
	int nb_cmd = 0, verif_cmds = 0,verif_redir=0,boucle=1;
	
	while(1)
	{
		nb_cmd = 0;
		//for(int i=0;i<MAX_STR_SIZE;i++){
		for(int i=0;i<MAX_CMD;i++){
			cmds[i].argv[0]=NULL;
			cmds[i].stdin=0;
			cmds[i].stdout=1;
			cmds[i].stderr=2;
			cmds[i].background=0;
			cmds[i].next_processus=NULL;
			cmds[i].next_success=NULL;
			cmds[i].next_failure=NULL;
		}
		
		printf("$ ");
		lire(str, MAX_STR_SIZE);
		trim(str);  //on enléve les espaces en début et fin de ligne
		clean(str);	// Suppression des doublons d'espaces
		split_str(str,tokens,MAX_TOKENS);	// Découpe la chaîne str et enregistre les pointeurs dans tokens
		separator_t sep;
		cmds[0].argv[0]=tokens[0];
		char** iterator = tokens; 
		int next_processus;
		while((next_processus = split_command(iterator, &sep))!=-1)
		{
			cmds[nb_cmd+1].argv[0]=iterator[next_processus];
			verif_cmds = 1;
			switch(sep){	//on cherche le séparateur
				case AND:
					printf("separateur : &&, prochaine commande :%d \n",next_processus);
					cmds[nb_cmd].next_processus = NULL;
					cmds[nb_cmd].next_success = &cmds[nb_cmd+1];
					cmds[nb_cmd].next_failure = NULL;
					break;
				case OR:
					cmds[nb_cmd].next_processus = NULL;
					cmds[nb_cmd].next_success = NULL;
					cmds[nb_cmd].next_failure = &cmds[nb_cmd+1];
					break;
				case SEMICOL:
					cmds[nb_cmd].next_processus = &cmds[nb_cmd+1];
					cmds[nb_cmd].next_success = NULL;
					cmds[nb_cmd].next_failure = NULL;
					break;
				case EGAL:
					builtin_export(cmds[nb_cmd].argv[0],cmds[nb_cmd+1].argv[0]);
					break;
			}
			redirection_t redir_type;
			int path_idx = redirection(iterator,&redir_type);
			switch(redir_type){	//on regarde si il ya une redirection
			case OUTPUT_APPEND:
				cmds[nb_cmd].stdout=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
				break;
			case OUTPUT:
				cmds[nb_cmd].stdout=open(iterator[path_idx], O_WRONLY | O_CREAT, 0660);
				break;	
			case ERROR:
				cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_TRUNC, 0660);
				break;	
			case ERROR_APPEND:
					cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
					break;
			case INPUT:
					cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
					break;				
			}
			int i=0;
			for(char** arg=iterator; *arg!=NULL; ++arg) //on affecte les arguments
			{ 
				if(*arg[0]=='$')
				{
					char env[strlen(*arg)];
					strcpy(env,*arg);
					for(int n=0;n<strlen(env);n++)
					{
						env[n]=env[n+1];
					}
					cmds[nb_cmd].argv[i++] = getenv(env);
				}
				else
				{
					cmds[nb_cmd].argv[i++] = *arg;
				}
			}
			
			if(strcmp(cmds[nb_cmd+1].argv[0],"exit")==0)
				builtin_exit(getpid());
			if(strcmp(cmds[nb_cmd+1].argv[0],"cd")==0)
				builtin_cd(cmds[nb_cmd].argv[1]);
			if(strcmp(cmds[nb_cmd+1].argv[0],"unset")==0)
				builtin_unset(cmds[nb_cmd].argv[1]);

			search_path(&cmds[nb_cmd]);
			if(cmds[nb_cmd+1].argv[0] != NULL) 
				search_path(&cmds[nb_cmd+1]);
	
			iterator+=next_processus;
			nb_cmd++;
		}
		int i=0;
		for(char** arg=iterator; *arg!=NULL; ++arg) //on affecte les arguments
		{
			if(strcmp(*arg,">>")==0 || strcmp(*arg,">")==0 || strcmp(*arg,"2>")==0 || strcmp(*arg,"2>>")==0)
			{
				verif_redir=1;
				break;
			}
			if(*arg[0]=='$')
			{
				char env[strlen(*arg)];
				strcpy(env,*arg);
				for(int n=0;n<strlen(env);n++)
				{
					env[n]=env[n+1];
				}
				cmds[nb_cmd].argv[i++] = getenv(env);
				i++;
			}
			else
			{
				cmds[nb_cmd].argv[i++] = *arg;
			}
		}

		if(strcmp(cmds[nb_cmd].argv[0],"exit")==0)
			builtin_exit(getpid());
		if(strcmp(cmds[nb_cmd].argv[0],"cd")==0)
			builtin_cd(cmds[nb_cmd].argv[1]);
		if(strcmp(cmds[nb_cmd].argv[0],"unset")==0)
			builtin_unset(cmds[nb_cmd].argv[1]);

		if(verif_redir==1)
		{
			redirection_t redir_type;
			int path_idx = redirection(iterator,&redir_type);
			switch(redir_type){	//on regarde les redirections
				case OUTPUT_APPEND:
					cmds[nb_cmd].stdout=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
				break;
				case OUTPUT:
					cmds[nb_cmd].stdout=open(iterator[path_idx], O_WRONLY | O_CREAT | O_TRUNC, 0660);
					break;		
				case ERROR:
					cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_TRUNC, 0660);
					break;		
				case ERROR_APPEND:
					cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
					break;
				case INPUT:
					cmds[nb_cmd].stderr=open(iterator[path_idx], O_WRONLY | O_CREAT | O_APPEND, 0660);
					break;
				}
		}
			
		if (verif_cmds == 0) {
			search_path(&cmds[nb_cmd]);
		}
		exec_cmdline(cmds); //on execute les commandes
		cmds[nb_cmd].stdout=1;
		for(int i=1; i<strlen(tokens); i++){
			tokens[i] = NULL;
			cmds[nb_cmd].argv[i] = NULL;
		}
		iterator=NULL; //on remet tout a NULL ou 0
		cmds[nb_cmd].argv[0] = NULL;
		verif_cmds = 0;
		verif_redir=0;
	}
}
