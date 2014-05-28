#ifndef LIST_C
#define LIST_C

// Lista com id numérico, automático e incremental.(limitado);
// E com valor void*

static unsigned long _id = 0;

struct _list {
	unsigned long id;
	void* val;
	list* next;
};


List List_New( void* val ) {
	List l = (List) malloc( sizeof( _list ) );
	l->id = ++_id;
	l->val = val;
	l->next = NULL;
	return l;
}

void List_Insert( List l, List lNew) {
	if( !l ) {
		return;
	}
	for(l; l->next; l = l->next);
	l->next = lNew;
}

List List_Delete( ) {
	List prevList;
	while( l ) {
		prevList = l;
		l = l->next;
		//free( l->val );
		free( prevList );
	}
}

#endif





