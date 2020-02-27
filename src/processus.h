/*
  Fichier processus.h: declaration de la structure processus_t et definition de certaine variable
  Auteur : Josselin ABEL et Jimmy WIMS
*/

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

#ifndef PROCESSUS_H
#define PROCESSUS_H

#define MAX_ARGS 128
#define MAX_PATH_SIZE 256
#define MAX_ENVS 256
#define MAX_CMD 2048
//#define MAX_CMD 256

typedef struct processus_t
{
	char path[MAX_PATH_SIZE];
	char *argv[MAX_ARGS];
	char *envp[MAX_ENVS];
	int stdin,stdout,stderr,status;
	pid_t pid;
	int background;
	struct processus_t * next_processus;
	struct processus_t * next_success;
	struct processus_t * next_failure;
}processus_t;
#endif

