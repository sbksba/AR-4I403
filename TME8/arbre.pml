#define N 6           /* Number of processes */ 
#define NB_V_MAX 3    /* maximum degree of a node */ 

mtype = {calcul};

chan channel_IN[N] = [N] of {mtype, byte, byte}; 
   /*  All messages to process i are received on channel_IN[i] 
       a message contains information <type, sender, value>   */
   
/**************************************************************/

inline Simulateur() {
   if
      :: (id == 0) ->
	     minloc = 3; nb_voisins = 2;
	     voisins[0] = 1; voisins[1] = 2;
      :: (id == 1) ->
	     minloc = 11; nb_voisins = 3;
	     voisins[0] = 0; voisins[1] = 3; voisins[2] = 4;
      :: (id == 2) ->
	     minloc = 8; nb_voisins = 2;
	     voisins[0] = 0; voisins[1] = 5;
      :: (id == 3) ->
	     minloc = 14; nb_voisins = 1;
	     voisins[0] = 1;
      :: (id == 4) ->
	     minloc = 5; nb_voisins = 1;
	     voisins[0] = 1;
      :: (id == 5) ->
	     minloc = 17; nb_voisins = 1;
	     voisins[0] = 2;
   fi;
   mincal = minloc	  
}

/**************************************************************/
inline Test_Emission (q) {
   /* determine si l'on peut emettre et identifie le destinataire q */
   if
      ::(nb_voisins-1==nb_recus) -> emission = true;
		i=0;
		do
			::(i<nb_voisins)->
				if
					::recu[voisins[i]]==0 -> q=voisins[i];break
					::else ->i++
				fi
			::else->break
		od
      ::else -> emission=false
   fi
}

inline Test_Decision () {
   /* determine si l'on peut decider */
   if
    ::nb_recus<nb_voisins->decision=false
	::nb_recus>=nb_voisins -> decision=true;
		i=0;
		do
	  		::(i<nb_voisins && voisins[i]==sender)->i++
			::(i<nb_voisins && voisins[i]!=sender)->channel_IN[voisins[i]]!calcul(id,mincal);i++
	  		::(i==nb_voisins)->break
		od;
  fi;
  
}

/**************************************************************/

proctype node( byte id ) {
    
byte nb_voisins;
byte voisins[NB_V_MAX];

chan canal_IN = channel_IN[id];
xr canal_IN;            /* only id reads on this channel */

mtype type;
byte i, nb_recus;
byte sender, receiver, minloc, mincal;

/* tableau initialise a 0 ; recu[i] = 1 si on a recu de i */
byte recu[N];  
bool emission, wait = 0, decision, deja_emis = 0;

   Simulateur();
   nb_recus=0;
/* Each process executes a finite loop */
   do
      :: /* test emission et traitement correspondant 
              - on ne doit emettre qu'une fois
              - si on ne peut pas emettre, on attend (wait = 1) que la condition ait pu etre modifiee  */
		(wait == 0 && deja_emis == 0)->
		Test_Emission(receiver);
		if
	   		::emission == true -> deja_emis=1;channel_IN[receiver]!calcul(id,mincal)
	   		::emission == false -> wait = 1
		fi
      :: canal_IN?type(sender,i) -> recu[sender]=1;wait=0;nb_recus++;/* test reception et traitement correspondant */
		if
		   ::i<mincal -> mincal=i
		   ::else->skip
		fi

      :: /* test terminaison */
		(wait==0 && deja_emis==1)-> Test_Decision();
		if
			::(decision==true)->break
			::else->skip
		fi;

   od;
   printf("%d : le minimum est %d\n", _pid, mincal);
}

/**************************************************************/
/* Creates a network of 6 nodes 
   The structure of the network is given by array voisins */ 

init{
   byte proc;
   atomic {
      proc=0;
      do
         :: proc <N ->
            run node(proc); 
            proc++
         :: proc == N -> break 
      od
   }
}
