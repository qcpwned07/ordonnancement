/*

ARNAUD VILLEMAIRE
C.P. : VILA12049608

PICARD, MATTHIEU
C.P. : PICM07129307


gcc -Wall -std=c99 dispatch.c utils.c -o dispatch

*/

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define NEW 0
#define READY 1
#define BLOQUE 2
#define TERMINE 3
// ------------------------- //
// -       STRUCTURE       - //
// ------------------------- //

typedef struct
{
    int pid;
    int tarrive;
    int tsize;
    int tn;
    int * ts;
    int * tblocages;
    int nblocage;
    int etat;
    int nextQ;
    int quantumLeft;
    int timeInCpu;
    
} Process;

typedef struct 
{
    int size;
    int current;
    Process * ps;
} List;

typedef struct 
{
    int size;
    int current;
    Process ** ps;
} Queue;

// ------------------------- //
// ------------------------- //
// -    DECLARATIONS Fx    - //
// ------------------------- //

void validerParams(int argc, char const *argv[] );

List * parseFile (FILE *file);

void erreur (int err);

// F(x) des process
Process * newProcess ();

void printProcess (Process *p);
    
void addT (Process *p, int t);

int getPid (List l, int pid);

// F(x) des listes
int getBiggestArrival (List l);

List * newList ();

void addProcess (List *l, Process *p);

void printList (List l) ;

void swap (Process *x, Process *y);

void sortList (List *l);

void ordonnancer (List l, int quantum0, int quantum1);

// Queue

Queue newQueue ();

Process * next(Queue *q);

void enqueue (Queue *q, Process *p);

void printQueue(Queue * qs);
// Ordonnancement

int setReady(List *l, int t);

int nready (List *l, int t);

Process * mettreDansCpu (Process **cpu, Process *p, int t, int quantum);

void bloquer (Process * p, int t);

int allFinished(List  l) ;

Process * nextReady (Queue* q);

void dequeue(Process * p, Queue *qs);
void setTermine(List *l);
void hasString(char *str, int err);
//
// ------------------------- //
// -       FUNCTIONS       - //
// ------------------------- //


int main (int argc, char const *argv[])
{
    // Debuter par la validation des args
    validerParams(argc,  argv);

    // Assigner les variables
    hasString((char*)argv[1],2);
    int quantumF0 = atoi(argv[1]);
    hasString((char*)argv[2],2);
    int quantumF1 = atoi(argv[2]);
    
    if(quantumF0 <= 0 || quantumF1 <= 0){
      erreur(2);
    }

    // Ouvrir le fichier, erreur si probleme
    FILE *file = fopen( argv[3], "r");

    if (file == NULL)
    {
      erreur(1);        
    }


    // Creation des files
    // Tableau contenant les donnees
    List *l = newList();

    l = parseFile(file);

    ordonnancer(*l, quantumF0, quantumF1);

    // Free les malloc
    free(l);          // NEED ALSO TO FREE SECOND LEVEL POINTERS ??
    return 0;
}


void ordonnancer (List l, int quantum0, int quantum1)
{

    int cycle       = 0;
    int i           = 0;


    Queue qs[3];
    qs[0]           = newQueue();
    qs[1]           = newQueue();
    qs[2]           = newQueue();
    int quantums[3] = {quantum0 , quantum1, -1};

    Process * idle  = newProcess();
    Process * cpu = idle; 
    sortList ( &l );
    printList(l);

    while (!allFinished(l) && cycle < 85)
    {
        //printf("\n---- CYCLE  #%d ----\n", cycle);
        
        //Mettre les procvessus a ready
        setReady(&l, cycle);
        setTermine(&l);

        // Les inserer dans la bonne file
        for(i=0; i<=l.current; i++)
            if(l.ps[i].tarrive == cycle)
                enqueue(&qs[l.ps[i].nextQ], &l.ps[i]);

        // Impression des files
        //printQueue(qs);

        //Verifier si le processus a termine son execution
       // if (cpu != idle)
       //     if (cpu->quantumLeft == 0 || cpu->ts[0] == 0) {
       //         dequeue(cpu, &qs[cpu->nextQ]);
       //         bloquer(cpu,cycle);
       //         mettreDansCpu(&cpu, idle, cycle, -1);
       //     }

        if (cpu != idle)
        {
            if (cpu->ts[0] == 0) {
                dequeue(cpu, &qs[cpu->nextQ]);
                bloquer(cpu,cycle);
                mettreDansCpu(&cpu, idle, cycle, -1);
            }
            else if (cpu->quantumLeft == 0)
            {
                dequeue(cpu, &qs[cpu->nextQ]);
                enqueue(&qs[cpu->nextQ], cpu);
                cpu->quantumLeft = quantums[cpu->nextQ];
            }
        }

        //printf("NEXT READY:");
        //if (nextReady(qs) != NULL)
        //    printProcess(nextReady(qs));
        //else 
        //    printf("NULL");
        //printf("\n");

        //// Si arrive un nouveau processus prioritaire, le mettre dans le CPU
        //// TODO chg quantum 0
        if(nextReady(qs) != NULL)
            if(cpu != nextReady(qs))
            {
                printf("\n---- CYCLE  #%d ----\n", cycle);
                mettreDansCpu(&cpu, nextReady(qs), cycle, quantums[nextReady(qs)->nextQ] );
                printQueue(qs);
            }

        //printProcess(cpu);
        //Execute the cycle
        cpu->quantumLeft--;
        cpu->ts[0]--;
        cpu->timeInCpu++;
        cycle++;
    }
        printList(l);
}

Process * nextReady (Queue qs[]){
    int i, j;
    Process * p = NULL;
    Queue  q;

    for (i=0; i<3; i++)
    {
        q = qs[i];
        for(j=0; j<=qs[i].current; j++)
        if (qs[i].current >= 0) {
            p = qs[i].ps[0] ;
            i = 4;
            j = qs[i].current +2;
        } 
    }
    //if(p == NULL)
    //    printf("P == NULL #14\n");
    //else
    //    printf("BreakPoint #A : nextReady = PID : %d\n",p->pid );
    return p;
}

Process * mettreDansCpu (Process **cpu, Process *p, int t, int quantum)
{
    //printf("BreakPoint #B : mettreDansCPU \n");
    //printf("Putting PID #%d in CPU\n", p->pid);
    bloquer(*cpu, t);
    *cpu = p;
    if(p->quantumLeft == 0)
        p->quantumLeft = quantum;
    p->tarrive = t;
    return p;
}

int allFinished(List  l) 
{
    int i;
    int allFinished = 1;
    for (i=0; i<=l.current; i++)
        if( l.ps[i].nextQ < TERMINE)
            allFinished = 0;
    return allFinished;
}


void bloquer (Process * p, int t)
{
    int i; 
    // Afficher ses infos
    //printf("Blocking %d \n", p->pid);
    if (p->timeInCpu > 0)
        print_element(p->pid, p->tarrive, p->timeInCpu);

    // Si il doit changer de file 
    
    //Si son temps demander est termine
    if(p->ts[0] == 0)
    {
        p->tn--;
        // Changer son prochain temps darriver
        for (i=0; i<=p->tn; i++)
            p->ts[i] = p->ts[i+1];
        p->tarrive = t - p->ts[0] + 1;
        // Changer son prochain temps dexec
        p->etat = BLOQUE;
        p->tn--;
        for (i=0; i<=p->tn; i++)
            p->ts[i] = p->ts[i+1];

    } else if (p->quantumLeft == 0) {
        p->etat = READY;
        p->tarrive = t+1;
    }


    if(p->tn < 0)
    {
        p->tarrive = 0;
        p->etat = TERMINE;
        p->nextQ = 3;
    }


        
    p->timeInCpu = 0;
}

List * parseFile (FILE *file)
{
    // Declarer/initialiser les variables
    const int START_SIZE = 50;
    int last        = 0;
    //int current     = 0;
    int compteur    = 0;
    int arrayIndex  = 0;
    char temp;
    char temp1;
    char current[100];
    List *l         = newList();
    Process *p ;

    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
    if(len == 0)
      erreur(ERR_FICHIER_CORROMPU);
    rewind(file);
    // Parser le file 
    while (!feof (file)) {  

        p = newProcess();

        // --PID--
        fscanf (file, "%s%c", current, &temp);
        //printf("%s\n",current);
        //printf("%cval\n",temp);
        hasString(current,3);
        p->pid = atoi(current);                           
        if( feof(file) )
            break;
        if (atoi(current) <= 0 || temp == '\n') 
            erreur(ERR_FICHIER_CORROMPU);
        

        // --TARRIVE--
        fscanf (file, "%s%c", current, &temp);
        printf("%cval\n",temp);
        hasString(current,3);     
        p->tarrive = atoi(current);
        if (atoi(current) < 0 || temp == '\n' || feof(file)) 
            erreur(ERR_FICHIER_CORROMPU);

        // -- TABLEAU
        
        while(temp != '\n' && temp != EOF)
        {
            fscanf (file, "%s%c", current, &temp);
            if(temp != '\n' && temp != EOF){
              fscanf(file, "%c", &temp1);
              printf("v%c\n",temp1);
              if(temp1 == ' ' || temp1 == '\n' || temp1 == EOF){
                fscanf(file, " ");
                temp = '\n';
              }
              fseek(file,-1, SEEK_CUR);
            }
            //fscanf(file, " ");
            //fscanf(file, "%c", &temp);
            printf("c%s\n",current);
            hasString(current,3);    
            if (last < 0 && atoi(current) < 0) 
                erreur(ERR_FICHIER_CORROMPU);
            else if (compteur == 0 && atoi(current) < 0) 
                erreur(ERR_FICHIER_CORROMPU);
            // Ajouter au compteur l'array
            if (atoi(current) >= 0 && last >= 0)
                compteur += atoi(current);
            else if (current >= 0 && last < 0)
                compteur = atoi(current);

            //printf("compt. :%d, last: %d, current: %d, \n", compteur, last, atoi(current));

            
            if (temp == '\n' || atoi(current) < 0) {
                addT(p,compteur);
                if (atoi(current) < 0)
                    addT(p,atoi(current));
            }
            
            // Changer les valeurs
            last = atoi(current);
        }
        
        printf("%d\n",last);
        
        if(last <= 0)
          erreur(ERR_FICHIER_CORROMPU);
          
        //  --NEW LINE-- 
        if (temp == '\n' || temp == EOF) {
            //reinitialiser les valeur de la ligne
            printf("\n");
            arrayIndex ++;
            compteur    = 0;
            last        = 0;
            addProcess(l, p);
        }
    }

    fclose (file);       

    //printList(*l);
    return l;
}


void validerParams(int argc, char const *argv[] )
{
    // Checking if we have the right amount of param
    if (argc != 4 )
        exit(1);
}

// ---------- PROCESSES FUNCTIONS ----------

Process* newProcess ( )
{
    Process *p;
    p           = malloc(sizeof *p);

    p->ts       = calloc(10, sizeof(int));
    p->pid      = 0;
    p->tarrive  = 0; 
    p->tsize    = 10;
    p->tn       = -1;
    p->etat     = 0;
    p->nextQ    = 0;
    return p;
}

void sortList (List *l)
{
    int i, j;
    for (i=0; i < l->current ; i++)
        for (j=0; j < l->current - i; j++)
            if (l->ps[j].tarrive >= l->ps[j+1].tarrive)
            {
                if (l->ps[j].tarrive == l->ps[j+1].tarrive)   {
                    if (l->ps[j].pid > l->ps[j+1].pid)    {
                        swap(&l->ps[j], &l->ps[j+1]);
                    }
                } else
                        swap(&l->ps[j], &l->ps[j+1]);
            }
}

void swap (Process *x, Process *y)
{
    Process temp = *x;
    *x = *y;
    *y = temp;
}

void freeProcess (Process *p)
{
    //TODO
    free(p);
}

void addT (Process *p, int t)
{
    p->tn++;
    if (p->tn >= p->tsize) {
        p->tsize += 10;
        p->ts = realloc(p->ts, p->tsize * sizeof(int) );
    }

    p->ts[p->tn] = t;
}


void printProcess (Process *p)
{
    printf("    PID %d:   Etat: %d | qLeft: %d | nextQ: %d | tarrive: %d   [", p->pid, p->etat, p->quantumLeft, p->nextQ, p->tarrive);
    for(int i=0; i <= p->tn ;i++)
        printf(" %2d ", p->ts[i]);
    printf ("]\n");
    //printf("PID: %d, tn: %d, tsizeExec: %d, tarrive: %d \n", p->pid,p->tn, p->tsize, p->tarrive);
}

// ------- LIST FUNCTIONS ---------
List * newList ()
{
    List * l        = malloc(sizeof(List));
    l->ps    = malloc(sizeof(Process) *10);
    l->current      = -1;
    l->size         = 10;
    return l;
}

void addProcess (List *l, Process *p)
{
    l->current++;
    if (l->current >= l->size-1) {
        l->size += 10;
        l->ps = realloc(l->ps, l->size * sizeof(Process) );
    }

    l->ps[l->current] = *p;
    //WATCH OUT FOR SEGFAULT
    //freeProcess(p);
}
    


void printList (List l) 
{
    printf("  Liste :     Size = %d, current = %d \n", l.size, l.current);
    for (int i=0; i<=l.current; i++)
    {
        Process p = l.ps[i];
        printProcess(&p);
    }
    
}

// ------ QUEUE -----------
Queue newQueue ()
{
    Queue q;
    q.size = 9;
    q.current = -1;
    q.ps = malloc(sizeof(Process*) * 10);
    return q;
}

void printQueue(Queue *qs) {
    printf("\n QUEUES \n");
    for(int j=0; j<3; j++)
    {
        Queue q = qs[j];
        printf("  Queue %d :  Current = %d \n",j, q.current);
        for (int i=0; i<q.current+1; i++)
        {
            Process* p = q.ps[i];
            printProcess(p);
        }
    }
}

int setReady (List *l, int t)
{
    int i;
    int n = 0;
    for (i=0; i<=l->current; i++)
         if(l->ps[i].tarrive == t ) {
             //Si il etait bloque, remmettre dans la file F0
             if(l->ps[i].etat == BLOQUE)
                 l->ps[i].nextQ = 0;
             l->ps[i].etat = READY;
            n++;
        }
    return n;
}

void setTermine(List *l)
{
    int i;
    for (i=0; i<=l->current; i++)
        if(l->ps[i].tn == 0 ) {
            l->ps[i].etat = TERMINE;
        }
        //} else if (l->ps[i].ts[ l->ps[i].tn ] ==
}

void enqueue (Queue *q, Process *p)
{
    q->current++;
    if (q->current >= q->size -2)  {
        q->size += 10;
        q->ps = realloc(q->ps, q->size * sizeof(Process*));
    }
    q->ps[q->current] = p;
}

void dequeue(Process * p, Queue *q)
{
    //printf("Dequieing %d \n Before: \n",  p->pid);
    //printQueue(*q);
    int i;
    q->current--;
    for (i=0; i<=q->current + 1; i++)
        q->ps[i] = q->ps[i+1];
    if (p->nextQ <2)
        p->nextQ++;
}

void hasString(char *str, int err){
  char *endptr;
  strtol(str, &endptr, 10);
  if (*endptr != '\0' || endptr == str) {
    erreur(err);
  }
}

void erreur (int err)
{
    print_erreur(err);
    exit(err);
}

// TODO 
//
// Try with letters in file
// Verifier PID = 0 et tarrive =0 valide
// ||||||||
//\\\\\o
//jkh
