#include"utils.h"


/**
* A utiliser pour afficher une ligne exemple "PID       16 :      10 -      4"
*/
void print_element(unsigned int pid, unsigned int temps, unsigned int duree)
{
    printf("PID %10u | %10u | %10u|\n", pid, temps, duree);
    puts("--------------------------------------------");
}


void print_erreur(int err)
{
  switch (err)
  {
    case ERR_OUVERTURE_FICHER :
      printf("Erreur : erreur ouverture fichier\n");
      break;
    case ERR_PARMETRE_INCORRECT :
      printf("Erreur : parametres incorrects\n");
      break;
    case ERR_FICHIER_CORROMPU :
      printf("Erreur : fichier corrompu\n");
      break;
    case ERR_AUTRE :
      printf("Erreur : autre\n");
      break;
    default:
      break;
  }
}
