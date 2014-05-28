#ifndef LIST_C
#define LIST_C
#include "list.h"
#include "malloc.h"
// Lista com id numérico, automático e incremental.(limitado);
// E com valor void*

static unsigned long _id = 0;

struct _list {
	unsigned long id;
	void* val;
	List next;
};


List List_New( void* val ) {
	List l = (List) malloc( sizeof(struct _list ) );
	l->id = ++_id;
	l->val = val;
	l->next = NULL;
	return l;
}

List List_Insert( List l, List lNew) {
	if( !l ) {
		return lNew;
	}
	for(l; l->next; l = l->next);
	l->next = lNew;

	return l;
}

List List_Delete( List l) {
	List prevList;
	while( l ) {
		prevList = l;
		l = l->next;
		free( prevList );
	}
}

#endif





