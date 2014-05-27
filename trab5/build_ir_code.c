#ifndef BUILD_IR_CODE_C
#define BUILD_IR_CODE_C


static void _addCommand ( AST cmd  ) {
	AST expNode, blockNode;
	unsigned long labelBegin, labelEnd;
	char* valID = NULL;

	switch ( AST_GetType( cmd ) ) {
		case AST_DECL_VAR:
			Function_addDecl( _func, AST_GetStringValue( cmd ) );
		break;

		case AST_WHILE:
			labelBegin = newLabel( );
			labelEnd = newLabel( );
			Function_AddLabel( _func, labelBegin );
			valID = _addExpression( AST_GetFirstChild( cmd ) );
			Funtion_AddIfFalse( _func, labelBegin, labelEnd );
			_addBlock( AST_GetLastChild );
			Function_AddGoto( _func, labelBegin );
			Function_AddLabel( _func, labelEnd );
		break;

		case AST_IF:
			char *labelElIf = NULL;
			AST elIfNode;

			expNode = AST_GetFirstChild( cmd );
			blockNode = AST_GetNextSibling( expNode );
			elIfNode = blockNode;
			labelEnd = _newLabel( );

			do {
				if ( labelElIf ) {
					Function_AddLabel( _func, labelElIf );
					expNode = AST_GetFirstChild( elIfNode );
					blockNode = AST_GetNextSibling( expNode );
				}

				elIfNode = AST_GetNextSibling( elIfNode ); //Proximo Else ou If
				if ( elIfNode ) { //Se tem proximo, gera label, se não, termina.
					labelElIf = newLabel( );
				} else {
					labelElIf = labelEnd;
				}

				valID = _addExpression( expNode, NULL );
				Function_AddIfFalse( _func, valID, labelElIf );
				_addBlock( blockNode );
				Function_AddGoto( _func, labelEnd );


			} while( elIfNode && AST_GetType( elIfNode ) == AST_ELSE_IF );

			if ( elIfNode ) { //É um else
				Function_AddLabel( _func,  labelElIf );
				blockNode = AST_GetFirstChild( elIfNode );
				_addBlock( blockNode );
			}
			addLabel( labelEnd);
		break;

		case AST_ATTR:
			AST rval, lrval;
			char* rvalID = NULL;
			rval = AST_GetFirstChild( cmd );
			lval = AST_GetLastChild( cmd );
			rvalID = _addExpression( rval );
			_addExpression( lval, rvalID );

		break;

		case AST_CALL:
			AST arg;
			for( arg = AST_GetFirstChild( cmd ); arg; arg = AST_GetNextSibling( arg ) ) {
				_addParam( arg );
			}
			_addCall( cmd );
		break;

		case AST_RETURN:
			expNode = AST_GetFirstChild( cmd );
			if( expNode ) {
				valID = addExpression( expNode );
				_addReturnVal( valID );
			} else {
				_addReturn( );
			}
			
		break;

		default: break;
	}
}

static void _addBlock( AST block ) {
	AST child;
	for ( 
		child = AST_GetFirstChild( block );
		child;
		command( child ), child = AST_GetNextSibling( child ); 
		);
}

Function build_function( AST nodeFunc ) {
	AST child;
	Function function;
	function = (Function)malloc( sizeof( struct _function ) );
	_func = function;

	child = AST_GetFirstChild( nodeFunc ) ;

	function->id = AST_GetStringValue( nodeFunc );
	if ( AST_GetType( child ) == AST_PARAM_LIST ) {
		args( child );
		child = AST_GetNextSibling( child );
	}
	
	if ( AST_GetType( child ) != AST_BLOCK ) {
		child = AST_GetNextSibling( child );	
	}

	//BLOCK

	block( functionChild );

	return function;

}

void add_function( AST nodeFunc ) {
	Function* pFunction;
	for( pFunction = &(code->function); *pFunction != NULL; pFunction = &(*pFunction)->next );
	*pFunction = build_function( func );
}



void build_ir ( AST tree ) {
	AST c; 
	int i,j;

	if (t == NULL) {
		return;
	}	
	printf( "\n" );
	c = AST_GetFirstChild( tree );

	while( c != NULL ) {
		if( AST_GetType( c ) == AST_DECL_VAR ) {
			_addGlobal( c );
		}
		else {
			_addFunction( c );
		}
		
		c = AST_GetNextSibling(c);
	}
}

#endif