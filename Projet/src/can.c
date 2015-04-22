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

int main (int argc, char **argv) {

  point p1;
  p1.x = 0;
  p1.y = 8;
  
  point p2;
  p2.x = 8;
  p2.y = 8;
  
  point p3;
  p3.x = 0;
  p3.y = 0;

  point p4;
  p4.x = 8;
  p4.y = 0;

  noeud *a = initNoeud(1);
  a->es->a = p1;
  a->es->ap = p2;
  a->es->b = p3;
  a->es->bp = p4;
  printf("Création d'un noeud : [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",a->es->a.x,a->es->a.y,a->es->ap.x,a->es->ap.y,a->es->b.x,a->es->b.y,a->es->bp.x,a->es->bp.y);
    
  noeud *b = initNoeud(2);
  b->p->x = 5;
  b->p->y = 1;

  noeud *c = initNoeud(3);
  c->p->x = 7;
  c->p->y = 7;
  
  noeud *d = initNoeud(4);
  d->p->x = 3;
  d->p->y = 1;

  a->p->x = 2;
  a->p->y = 5;
  printf("A noeud %d de coordonnées (%d, %d)\n", a->id, a->p->x, a->p->y);
  printf("B noeud %d de coordonnées (%d, %d)\n", b->id, b->p->x, b->p->y);
  printf("C noeud %d de coordonnées (%d, %d)\n", c->id, c->p->x, c->p->y);
  printf("D noeud %d de coordonnées (%d, %d)\n", d->id, d->p->x, d->p->y);
  
  if (estDansEspace(a->es, b) == TRUE)
    printf("EST DANS\n");
  
  if (maxEspace(a->es) == HORIZONTAL)
    printf("MAX ESPACE : HORIZONTAL\n");
  else
    printf("MAX ESPACE : VERTICAL\n");

  espace *espaceB = malloc(sizeof(espace));
  espace *espaceC = malloc(sizeof(espace));
  espace *espaceD = malloc(sizeof(espace));
    
  printf("ORI [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",a->es->a.x,a->es->a.y,a->es->ap.x,a->es->ap.y,a->es->b.x,a->es->b.y,a->es->bp.x,a->es->bp.y);
  
  espaceB = decoupe(a);
  printf("B   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",espaceB->a.x,espaceB->a.y,espaceB->ap.x,espaceB->ap.y,espaceB->b.x,espaceB->b.y,espaceB->bp.x,espaceB->bp.y);

  b->es->a = espaceB->a;
  b->es->ap = espaceB->ap;
  b->es->b = espaceB->b;
  b->es->bp = espaceB->bp;

  espaceC = decoupe(b);
  printf("C   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",espaceC->a.x,espaceC->a.y,espaceC->ap.x,espaceC->ap.y,espaceC->b.x,espaceC->b.y,espaceC->bp.x,espaceC->bp.y);

  printf("A   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",a->es->a.x,a->es->a.y,a->es->ap.x,a->es->ap.y,a->es->b.x,a->es->b.y,a->es->bp.x,a->es->bp.y);

  espaceD = decoupe(a);
  printf("D   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",espaceD->a.x,espaceD->a.y,espaceD->ap.x,espaceD->ap.y,espaceD->b.x,espaceD->b.y,espaceD->bp.x,espaceD->bp.y);

  printf("A   [ (%d,%d) (%d,%d) (%d,%d) (%d,%d) ]\n",a->es->a.x,a->es->a.y,a->es->ap.x,a->es->ap.y,a->es->b.x,a->es->b.y,a->es->bp.x,a->es->bp.y);

  return EXIT_SUCCESS;
}

