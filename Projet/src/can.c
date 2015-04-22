#include "can.h"

/* ************ */
/*  PRIMITIVES  */
/* ************ */

/* POINT */
/* Tire aléatoirement un point entre a et b
*/
point *alea(int a, int b)
{
 int res1=0, res2=0, low_num=0, hi_num=0;
 point *p = (point *)malloc(sizeof(point));
 if(a<b)
    {
      low_num=a;
      hi_num=b+1;
    }
  else
    {
      low_num=b+1;
      hi_num=a;
    }
 struct timespec ts;
 clock_gettime(CLOCK_MONOTONIC, &ts);
 /* using nano-seconds instead of seconds */
 srand((time_t)ts.tv_nsec);
 res1 = (rand()%(hi_num-low_num))+low_num;
 res2 = (rand()%(hi_num-low_num))+low_num;
 p->x = res1;
 p->y = res2;

 return p;
}

/* NOEUD */

/*Initialise un noeud grâce à un identifiant*/
noeud *initNoeud(int id)
{
  noeud *n = (noeud *)malloc(sizeof(noeud));
  n->id = id;
  n->p  = alea(COORD_MIN, COORD_MAX);
  n->es = (espace *)malloc(sizeof(espace));

  n->lvoisins[NORD] = nouvelleListe();
  n->lvoisins[SUD] = nouvelleListe();
  n->lvoisins[OUEST] = nouvelleListe();
  n->lvoisins[EST] = nouvelleListe();

  return n;
}

/* LISTE */

/* Initialise une nouvelle liste */
liste_noeud *nouvelleListe()
{
  liste_noeud *liste = (liste_noeud *)malloc(sizeof(liste_noeud));
  liste->n = NULL;
  liste->suivant = NULL;
  return liste;
}

/* Ajoute un noeud dans la liste */
void ajouterNoeud(liste_noeud *liste, noeud *n)
{
  liste->n = n;
  liste->suivant = NULL;
}

/* INSERTION */

/* Retourne le sens de decoupe d'un espace */
int maxEspace(espace *es)
{  
  if ((es->ap.x - es->a.x) >= (es->a.y - es->b.y))
	return VERTICAL;
    else
	return HORIZONTAL;
}

/*
  Renvoie TRUE si b est dans l'espace de a
  Renvoie FALSE sinon 
*/
int estDansEspace(espace *es, noeud *b) {
   //Si le point correspondant à b est inclus dans l'espace
   //correspondant à a :
 
   if ((es->a.x < b->p->x) && (b->p->x < es->ap.x)
    && (es->b.x < b->p->x) && (b->p->x < es->bp.x)
    && (es->a.y > b->p->y) && (b->p->y < es->ap.y)
    && (es->b.y < b->p->y) && (b->p->y > es->bp.y))
	return TRUE;
   else
	return FALSE;
}

espace *decoupe(noeud *a)
{
  int l,h;
  if (maxEspace(a->es) == VERTICAL)
    {
      espace *droit  = malloc(sizeof(espace));
      espace *gauche = malloc(sizeof(espace));
      l = (a->es->ap.x - a->es->a.x);
            
      droit->ap = a->es->ap;
      droit->bp = a->es->bp;
      droit->a = a->es->a;
      droit->b = a->es->b;
      droit->a.x = a->es->a.x + (l/2);
      droit->b.x = a->es->b.x + (l/2);
      
      gauche->a = a->es->a;
      gauche->b = a->es->b;
      gauche->ap = a->es->ap;
      gauche->bp = a->es->bp;
      gauche->ap.x = a->es->ap.x - (l/2);
      gauche->bp.x = a->es->bp.x - (l/2);

      if (estDansEspace(droit, a) == TRUE)
	{
	  a->es->a.x = a->es->a.x + (l/2);
	  a->es->b.x = a->es->b.x + (l/2);
	  free(droit);
	  return gauche;
	}
      else
	{
	  a->es->ap.x = a->es->a.x + (l/2);
	  a->es->bp.x = a->es->b.x + (l/2);
	  free(gauche);
	  return droit;
	}
    }
  else
    {
      espace *haut  = malloc(sizeof(espace));
      espace *bas = malloc(sizeof(espace));
      h = (a->es->a.y - a->es->b.y);
      
      haut->ap = a->es->ap;
      haut->bp = a->es->bp;
      haut->bp.y = a->es->bp.y + (h/2);
      haut->a = a->es->a;
      haut->b = a->es->b;
      haut->b.y = a->es->b.y + (h/2);
      
      bas->a = a->es->a;
      bas->a.y = (h/2);
      bas->b = a->es->b;
      bas->ap = a->es->ap;
      bas->ap.y = (h/2);
      bas->bp = a->es->bp;

      if (estDansEspace(haut, a) == TRUE)
	{
	  a->es->b.y = a->es->b.y + (h/2);
	  a->es->bp.y = a->es->bp.y + (h/2);
	  free(haut);
	  return bas;
	}
      else
	{
	  a->es->a.y = (h/2);
	  a->es->ap.y = (h/2);
	  free(bas);
	  return haut;
	}
    }
}

void printEspace(noeud *noeud)
{
  printf("#%d   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",noeud->id,noeud->es->a.x,noeud->es->a.y,noeud->es->ap.x,noeud->es->ap.y,noeud->es->b.x,noeud->es->b.y,noeud->es->bp.x,noeud->es->bp.y);
}

noeud *attributionEspace(noeud *noeud, espace *espace)
{
  noeud->es->a = espace->a;
  noeud->es->ap = espace->ap;
  noeud->es->b = espace->b;
  noeud->es->bp = espace->bp;

  return noeud;
}

void printPlanGnuplotI(int max)
{
  printf("\n#!/bin/gnuplot\nset terminal png\nset output \"Plan.png\"\nset boxwidth 0.75 absolute\nset xtic auto # set xtics automatically\nset ytic auto # set ytics automatically\nset title \"Plan\"\nset xlabel \"X\"\nset ylabel \"Y\"\nset xr [0:%d]\nset yr [0:%d]\n",max);
}

void printRect(noeud *noeud)
{
  printf("set object %d rect from %d,%d to %d,%d fc rgb \"white\"\n",noeud->id, noeud->es->b.x, noeud->es->b.y, noeud->es->ap.x, noeud->es->ap.y);
}

void printPlanGnuplotII()
{
  printf("set offset 1,1,1,1\nplot 'dot.dat' using 1:2:(sprintf(\"(%d, %d)\", $1, $2)) with labels point  pt 7 offset char 1,1 notitle\n");
}

int main (int argc, char **argv) {

  point p1, p2, p3, p4;
  
  p1.x = 0; p1.y = 8;
  p2.x = 8; p2.y = 8;
  p3.x = 0; p3.y = 0;
  p4.x = 8; p4.y = 0;

  noeud *a = initNoeud(1);
  a->p->x = 2; a->p->y = 5;
  
  a->es->a  = p1;
  a->es->ap = p2;
  a->es->b  = p3;
  a->es->bp = p4;
  
  printf("Espace d'origine : [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",a->es->a.x,a->es->a.y,a->es->ap.x,a->es->ap.y,a->es->b.x,a->es->b.y,a->es->bp.x,a->es->bp.y);
    
  noeud *b = initNoeud(2);
  b->p->x = 5; b->p->y = 1;

  noeud *c = initNoeud(3);
  c->p->x = 7; c->p->y = 7;
  
  noeud *d = initNoeud(4);
  d->p->x = 3; d->p->y = 1;

  noeud *e = initNoeud(5);
  e->p->x = 7; e->p->y = 3;
  
  printf("\nA noeud %d de coordonnées (%d, %d)\n", a->id, a->p->x, a->p->y);
  printf("B noeud %d de coordonnées (%d, %d)\n", b->id, b->p->x, b->p->y);
  printf("C noeud %d de coordonnées (%d, %d)\n", c->id, c->p->x, c->p->y);
  printf("D noeud %d de coordonnées (%d, %d)\n", d->id, d->p->x, d->p->y);
  printf("E noeud %d de coordonnées (%d, %d)\n", e->id, e->p->x, e->p->y);
  
  if (estDansEspace(a->es, b) == TRUE)
    printf("\nB est dans l'espace de A\n");
  else
    printf("\nB n'est pas dans l'espace de A\n");
  
  if (maxEspace(a->es) == HORIZONTAL)
    printf("\nMAX ESPACE : HORIZONTAL\n");
  else
    printf("\nMAX ESPACE : VERTICAL\n");

  espace *espaceB = malloc(sizeof(espace));
  espace *espaceC = malloc(sizeof(espace));
  espace *espaceD = malloc(sizeof(espace));
  espace *espaceE = malloc(sizeof(espace));
  
  printEspace(a);
  
  espaceB = decoupe(a);
  b = attributionEspace(b,espaceB);
  printEspace(b);
  printEspace(a);

  espaceC = decoupe(b);
  c = attributionEspace(c,espaceC);
  printEspace(c);
  printEspace(a);
  
  espaceD = decoupe(a);
  d = attributionEspace(d,espaceD);
  printEspace(d);
  printEspace(a);

  espaceE = decoupe(b);
  e = attributionEspace(e,espaceE);
  printEspace(e);
  printEspace(b);

  if (estDansEspace(a->es, b) == TRUE)
    printf("\nB est dans l'espace de A\n");
  else
    printf("\nB n'est pas dans l'espace de A\n");

  if (estDansEspace(a->es, c) == TRUE)
    printf("C est dans l'espace de A\n");
  else
    printf("C n'est pas dans l'espace de A\n");

  if (estDansEspace(a->es, d) == TRUE)
    printf("D est dans l'espace de A\n");
  else
    printf("D n'est pas dans l'espace de A\n");

  /* GNUPLOT */
  /*
    printPlanGnuplotI(8);
    printRect(a);
    printRect(b);
    printRect(c);
    printRect(d);
    printRect(e);
    printPlanGnuplotII();
  */
  return EXIT_SUCCESS;
}

