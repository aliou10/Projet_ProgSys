//
// Created by mamour on 24/03/17.
//

#ifndef PROJET_PROGSYS_PROJET_PROGSYS_H
#define PROJET_PROGSYS_PROJET_PROGSYS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>


/*****************************************************
 * Declarations des variables
 *****************************************************/
int nombreDeProcessus;          //Nombre de processus a creer
pid_t pidMainProc;           //pid du processus principal
int cpt = 0;





/*****************************************************
 * Declaration des fonctions utilisees
 *****************************************************/

/**
 * Cette fonction verifie que l'argument du nombre de processus est
 * bien passe en parametre sinon le @code<Programme> invite
 * l'utilisateur a saisir le nombre de processus qu'il souhaite creer
 *
 * @return le nombre de processus a
 */
int controleArguments(int, char *);

void handler_tubesDispo(int);


#endif //PROJET_PROGSYS_PROJET_PROGSYS_H
