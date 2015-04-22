#ifndef __CAN_H_
#define __CAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include <MPI.h>

#define COORD_MIN 0
#define COORD_MAX 1000

#define NORD  0
#define SUD   1
#define OUEST 2
#define EST   3

#define HORIZONTAL 0
#define VERTICAL   1

#define TRUE  1
#define FALSE 0

#define N         5
#define BOOTSTRAP 1

typedef struct _point {
  int x;
  int y;
} point;

typedef struct _espace {
  point a;
  point b;
  point ap;
  point bp;
} espace;

typedef struct _noeud {
  int id;
  point *p;
  espace *es;
  struct liste_noeud *lvoisins[4];
} noeud;

typedef struct _liste_noeud {
  noeud *n;
  struct liste_noeud *suivant;
} liste_noeud;

/* ************ */
/*  PRIMITIVES  */
/* ************ */

/* POINT */
point *alea(int a, int b);

/* NOEUD */
noeud *initNoeud(int id);

/* LISTE */
liste_noeud *nouvelleListe();
void ajouterNoeud(liste_noeud *liste, noeud *n);

/* INSERTION */
int maxEspace(espace *es);
int estDansEspace(espace *es, noeud *b);
espace *decoupe(noeud *a);

#endif //__CAN_H_
