#include <stdbool.h>
#include <stdlib.h>

#define NREG 3

typedef struct AddrDescr_  AddrDescr;
struct AddrDescr_ {
	Addr id;
	Regs* rs[NREG];
	bool inMem;
	AddrDescr* next;
};

typedef struct AddrNode_ AddrNode;
struct AddrNode_ {
	Addr addr;
	AddrNode* next;
};

typedef struct RegDescr_ RegDescr; //Is a list
struct RegDescr_ {//Cabeça de uma lista de addr
	AddrNode* locations;
};

RegDescr regs[NREG];
AddrDescr* descriptors;

bool eqAddr( Addr a1, Addr a2 ) {
	if( a1.type != a2.type ) {
		return false;
	}
	switch( a1.type ) {
		case AD_GLOBAL:
		case AD_LOCAL:
		case AD_TEMP:
			return !( strcmp( a.str, b.str ) );
		break;
		default:
			return false;
		break;
	}
}


void AddrNode_Delete( AddrNode* l ) {
	AddrNode* node;
	for( node = l; node;  ) {
		l = node;
		node = node->next
		free( l );
	}
}

void AddrNode_link( AddrNode* l1, AddrNode* l2 ) {
	AddrNode* node;
	if( !l1 ) {
		return l2;
	}
	if( !l2 ) {
		return l1;
	}
	for( node = l1; node->next; node = node->next );
	node->next = l2;
}

AddrNode* AddrNode_New( Addr a ) {
	AddrNode* node = (AddrNode*)calloc( 1, sizeof( AddrNode ) );
	node->next = NULL;
	node->addr = a;
	return node;
}

AddrDescr* AddrDescr_New( Addr a, bool inMem ) {//Não seta registradores
	AddrDescr* node = (AddrDescr*)calloc( 1, sizeof( AddrDescr ) );
	node->next = NULL;
	node->addr = a;
	node->inMem = inMem;
	for( int i = 0; i < NREG; i++ ) {
		node->rs[i] = NULL;
	}
	return node;	
}

void AddrDescr_link( AddrDescr* l1, AddrDescr* l2 ) {
	AddrDescr* node;
	if( !l1 ) {
		return l2;
	}
	if( !l2 ){
		return l1;
	}
	for( node = l1; node->next; node = node->next );
	node->next = l2;
}

void AddrDescr_Delete( AddrDescr* l ) {
	AddrDescr* node;
	for( node = l; node; ) {
		l = node;
		node = node->next;
		free(l);
	}
}

echoLocation( Function* fun, Addr a );
int echoLocation( Function* fun, Addr a ) {
	long loc = 0;
	if( a.type == AD_LOCAL ) {
		loc = (-8)*( fun->nArgs - a.num );
	}
	else if( a.type == AD_GLOBAL ) {

	}
	else 
}

void echoReg(long regId, FILE* out) {
	switch( regId ) {
		case 0:
			fprintf( out, "%%eax" );
		break;
		case 1:
			fprintf( out, "%%edx" );
		break;
		case 2:
			fprintf( out, "%%edi" );
		break;
	}
}

void getReg(Function* fun, Instr* ins, long* ry, long* rz) {
	Reg* ri;
	Addr x, Addr y, Addr z;
	AddrNode it;
	AddrDescr ad;
	x = ins->x;
	y = ins->y;
	z = ins->z;


	*ry = -1, *rz = -1;
	//Y 
		//se y já tá em algum registrador
		for( int i = 0; i < NREG; i++ ) {
			ri = regs + i;
			for( it = ri->locations; it; it= it->next  ) {
				if( eqAddr(fun, y, it->addr) ) {
					ry = i;
					break;
				}
			}
		}

		//SE há registrador vazio
		if( (*ry) < 0 ) {
			for( int i = 0; i < NREG; i++ ) {
				ri = regs + i;
				if( ! ri->locations ) {
					*ry = i;
				}
			}
		}

		//Caso CHATO
			//(a)
			long nstores = 0;
			for( int i = 0; i < NREG; i++ ) { //cada reg
				bool regOk = true;
				for( it = ri->locations; it; it= it->next  ) { //cada v
					bool varOk = false;
					for( ad = descriptors; ad; ad = ad->next ) { //find v
						if( eqAddr( ad->id, it->addr ) ) {
							for( int j = 0; j < NREG; j++ ) {
								varOk = varOk || ( j!=i && ad->rs[j] );
							}
							varOk = varOk || ad->inMem;
						}
					}
					regOk = regOk && varOk;
 				}
 				if( regOk ) {
 					*ry = i;
 				}
			}
			//(b) 
			for( int i = 0; i < NREG; i++ ) { //cada reg
				for( it = ri->locations; it; it= it->next  ) { //cada v
					if( eqAddr( it->addr, x ) && !eqAddr( z, x ) ) {
						*ry = i;
					}
				}
			}

			if( eqAddr( x, y) && !eqAddr( x, z ) ) {
				if( )
			}

		//
		if( !(*ry) ) {
			for( ri = regs; ri; ri->next ) {
				if(ri->stored) {
					*ry = ri;
				}
			}
		}	
	// if is x
	if( !(*ry) ) {
		for( ri = regs; ri; ri->next ) {
			if( eqAddr(fun, x, ri->addr) ** !eqAddr(fun, x, z ) ) {
				*ry = ri;
			}
		}
	}
}


void addStore( Function* fun, int reg, Addr a ) {
	AddrDescr* it;
	AddrDescr* ad = NULL;
	for( it = descriptors; it; it = it->next ) {
		if( eqAddr( a, it->id) ) {
			ad  = it;
			ad->inMem = true;
		}
	}
	if( !ad ) {
		ad = AddrDescr_New( a, true );
		AddrDescr_link( descriptors, ad );
	}
}

void addLoad( Function* fun, int reg, Addr a) { // TO register
	AddrDescr* it;
	AddrDescr* ad = NULL;
	Reg* r = regs + reg;

	//ADICIONA INSTRUÇÂO


	AddrNode_Delete( r->locations ); // Limpa
	r->locations = AddrNode_New( a ); // seta como somente o atual;

	//add R as a aditional location to a;
	for( it = descriptors; it; it = it->next ) {
		if( eqAddr( a, it->id) ) {
			ad  = it;
			ad->rs[reg] = r;
		}
	}
	if( !ad ) {
		ad = AddrDescr_New( a, true );
		ad->rs[reg] = r;
		AddrDescr_link( descriptors, ad );
	}
}

void addOp( Funtion* fun, Instr* ins int ry, int rz, FILE* out ) {


	switch( ins->op ) {

	}

	if( ry > -1 ) {
		echoReg( ry, out);
	}

	if( rz > -1 ) {
		fprintf( out, ", ");
		echoReg( rz, out);
	}

	fprintf( out, "\n");


}

void genCode(Function* fun, Instr* ins, FILE* out) {
		getRegs( fun, ins ); //fills rx,ry,rz -- FUN é necessária???
		if ( isNotIn( ry ) ) {
			addLD( fun ry );
		}
		if ( isNotIn( rz ) ) {
			addLD( fun, rz );
		}
		addIns( fun, ins );
}


void processBlock( Function*fun, BasicBlock* block, FILE* out ) {
	Instr* ins;

	for( ins = block->leader; ins; ins = ins->next ) {
		genCode( fun, ins, out );
	}

}