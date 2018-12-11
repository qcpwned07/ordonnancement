#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>


#define ERR_OUVERTURE_FICHER 1
#define ERR_PARMETRE_INCORRECT 2
#define ERR_FICHIER_CORROMPU 3
#define ERR_AUTRE 4


/**
* À utiliser pour afficher les erreurs
*/
void print_erreur(int err);

/**
* À utiliser pour afficher une ligne exemple : "PID       16 :      10 -      4"
*/
void print_element(unsigned int pid, unsigned int temps , unsigned int duree);

#endif
