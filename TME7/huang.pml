#define N      5    /* Number of processes */ 
#define L     10    /* Buffer size */
#define NIL (N+1)   /* for an undefined value */
#define PASSIF true
#define ACTIF false

mtype = {mes, ack, TERM, agr,decterm};

/* le processus i recoit ses messages dans canal[i] */ 
chan canal[N] = [L] of {mtype, int, byte};

inline initialisation(){
	horloge=0;
	unack=0;
	etat=ACTIF;
	i=0;
	idfin=0;
	fin=false;
}

inline broadcast_term(){
	i=0;
	idfin++;
	do
		::(i<N && i!=id)->canal[i]!TERM(horloge,id);i++
		::(i == id)->i++
		::(i==N)->break;
	od
}

inline broadcast_decterm(){
	i=0;
	do
		::(i<N && i!=id)->canal[i]!decterm(horloge,id);i++
		::(i == id)->i++
		::(i==N)->break;
	od
}

/*envoie d'un message applicatif*/
inline envoyer(){
	unack++;
	canal[(id+1)%N]!mes(NIL,id);
}

/*reception d'un message applicatif*/
inline reception_mes(){
	if
		::(etat==PASSIF)->etat=ACTIF
		::else->skip;
	fi;
	canal[idemet]!ack(horloge,id)
}

/*reception d'un ack*/
inline reception_ack(){
	unack--;
	if
		::(horemet > horloge) -> horloge = horemet;
		::else -> skip;
	fi;
	if
		::(unack == 0 && etat == PASSIF) -> horloge++; nbagr=0; broadcast_term()
		::else->skip
	fi
}

/*reception d'un message terminaison*/
inline reception_term(){
	if
		::(horloge<horemet)->horloge=horemet;
			if
				::(etat == PASSIF && unack == 0) -> canal[idemet]!agr(horloge,id)
				::else->skip
			fi
		::else->skip
	fi
}

inline reception_agr(){
	nbagr++;
	if
		::(nbagr==N-1)->broadcast_decterm();fin=true;
		::else->skip
	fi
}

inline dect_changement_etat(){
	etat = PASSIF;
	if
		::(unack == 0) ->broadcast_term();
		::else->skip;
	fi
}
	
proctype node(byte id){
	chan mycanal = canal[id]; /*canal personnel*/
	byte idemet; /*id de l'emeteur du message*/
	int horloge; /*horloge locale*/
	int horemet; /*horloge message */
	int unack; /*nombre de ack*/
	mtype messtype; /*type du message recu */
	int nbagr; /*nombre d'agr recu*/
	bool etat;	/*etat du processus*/
	int i;
	int idfin; /*nombre de processus fini*/
	bool fin; /* si en etat de terminaison*/
	
	initialisation();
	do
		::(fin!=true && etat==ACTIF)->envoyer()
		::(etat!=PASSIF)->dect_changement_etat()
		::(idfin==N)->break
		::mycanal?messtype(horemet,idemet)->
			if
				::(messtype == mes)->reception_mes()
				::(messtype == TERM)->reception_term()
				::(messtype == ack)->reception_ack()
				::(messtype == agr)->reception_agr()
				::(messtype == decterm)->idfin++;
			fi
	od
}

init {
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
