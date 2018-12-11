/*

PICARD, MATTHIEU
C.P. : PICM07129307

*/

#include "utils.h"
#include "string.h"
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h> 
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/sysmacros.h>

// Compilation : gcc -Wall -std=c99 exemple.c utils.c

// Prend un nom de fichier et retourne l'inode correspondante
struct stat getInode(const char* filename);

// Inspecte recursivement l'inode passe en param
void inspect(char * filename);

// Termine en imprimant l'erreur approprie (passe en param)
void terminer (int errnum);

// Dis si nous avons deja croise cette inode
int isNew(int inode);



// Variables globales
int size;
// Inode deja visite;
int inodeno[10000];

int main (int argc, char const *argv[])
{
    // Declarer les variables de main
    char nomRoot[100];

    //Verifier que le bon nombre d'arg est present
    if (argc != 2)
        terminer(3);

    // Assigner les variables
    strcpy(nomRoot, argv[1]);
    size = 0;

    inspect(nomRoot);

    print_element(size, nomRoot);

    return 0;
}

struct stat getInode(const char* filename)
{
    struct stat statbuf;
    if (lstat(filename,  &statbuf) == -1) {
        terminer(errno);
    }

    return statbuf;
}

void inspect(char * filename) 
{
    struct dirent *direntp;
    DIR *dirp;
    struct stat current;
    // Aller chercher l'inode courrante
    current = getInode(filename) ;

    if(errno != 0 )
        terminer(errno);


    if(S_ISLNK(current.st_mode))
        size += current.st_size;
    else if(S_ISREG(current.st_mode))
    {    
        if (isNew(current.st_ino))
            size += current.st_size;
    } else if(S_ISDIR(current.st_mode))
    {
        //Changer le repertoire courant
        if(chdir(filename))
            terminer(1);
        // Ouvrir le stream pour le fichier que je m<apprete a inspecter
        dirp = opendir(".");
        size += current.st_size;
        // Inspecter recursivement les fichier contenu par ce repertoire
        while ((direntp = readdir(dirp)) != NULL && errno ==0) {
            if(strcmp(".", direntp->d_name) && strcmp("..", direntp->d_name))
                inspect(direntp->d_name);
        }
        
        // Si il y a eu une erreur
        if(errno != 0 )
            terminer(errno);
        // Close everything
        while ((closedir(dirp) == -1) && (errno == EINTR));
        chdir("..");

    } else 
            terminer(3);

}

int isNew(int inode)
{
    int i = 0;
    int isnew = 1;

    for(i=0;i<10000; i++)
       if (inodeno[i] == inode)
          isnew = 0; 
        else if(inodeno[i] == 0)
        {
            inodeno[i] = inode;
            break;
        }

    return isnew;
}

void terminer (int errnum) 
{
    if(errnum == EACCES)
        errnum = 1;
    else if (errnum > 3)
        errnum = 3;

    print_erreur(errnum);
    exit(errnum);
}
