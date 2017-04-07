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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <math.h>

#define CLE 248
#define CLE1 249
#define CLE2 250

/*****************************************************
 * Declarations des variables
 *****************************************************/
int nombreDeProcessus;          //Nombre de processus a creer
pid_t pidMainProc;           //pid du processus principal
int numeroProc;
void *adressePidFils;
void *adresseSegmentPublicKeys;
long int *publicKey;
pid_t *pid;





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

/**
 * Handler pour les signaux
 * sur disponibilité du segment
 * de memoire partagée pour les
 * clés publiques
 */
void handlerSegment(int);

/**
* Handler pour les signaux
* sur disponibilité des tubes
*/
void handlerPipe(int);

/**
* Handler pour terminaison
* des processus
*/
void handlerTerminaison(int);

int prime(long int);

void ce(long int, long int, long int, long int[], long int[]);

long int cd(long int, long int);

long int *encrypt(char *, long int, long int, long int[]);

void decrypt(char *, long int, long int, long int *);

long int generate(long int[], long int[]);

/**
 * Log des processus
 */
void logproc(int, int, char *);

/**
 * Suppression des segments
 */
void rmKEYS();


#endif //PROJET_PROGSYS_PROJET_PROGSYS_H
