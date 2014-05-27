IRCode _code;
Function _func;


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
			add_global( c );
		}
		else {
			add_function( c );
		}
		
		c = AST_GetNextSibling(c);
	}
}
static unsigned long _label = 0;
static unsigned long newLabel( ) {
	return _label++;
}

static unsigned long _temp = 0;
static unsigned long newTemp( ) {
	return _temp++;
}
//void args( )




void addCommand ( AST cmd  ) {
	AST expNode, blockNode;
	unsigned long labelBegin, labelEnd;
	char* valID = NULL;
	switch ( AST_GetType( cmd ) ) {
		case AST_DECL_VAR:

		break;

		case AST_WHILE:
			labelBegin = newLabel( );
			labelEnd = newLabel( );
			addLabel( labelBegin );
			valID = addExpression( AST_GetFirstChild( cmd ) );
			addWhileCondition( labelBegin, labelEnd );
			addBlock( AST_GetLastChild );
			addLabel( labelEnd );
		break;

		case AST_IF:
			char *labelElIf = NULL;
			AST elIfNode;

			expNode = AST_GetFirstChild( cmd );
			blockNode = AST_GetNextSibling( expNode );
			elIfNode = blockNode;
			labelEnd = newLabel( );

			do {
				if ( labelElIf ) {
					addLabel( labelElIf );
					expNode = AST_GetFirstChild( elIfNode );
					blockNode = AST_GetNextSibling( expNode );
				}

				elIfNode = AST_GetNextSibling( elIfNode ); //Proximo Else ou If
				if ( elIfNode ) { //Se tem proximo, gera label, se não, termina.
					labelElIf = newLabel( );
				} else {
					elIfNode = labelEnd;
				}

				valID = addExpression( expNode );
				addIfCondition( labelElIf );
				addBlock( blockNode );
				addGoto( labelEnd );


			} while( elIfNode && AST_GetType( elIfNode ) == AST_ELSE_IF );

			if ( elIfNode ) { //É um else
				addLabel( labelElIf );
				blockNode = AST_GetFirstChild( elIfNode );
				addBlock( blockNode );
			}
			addLabel( labelEnd);
		break;

		case AST_ATTR:
			char* endrID;
			endrID = addVar( AST_GetFirstChild( cmd ) );
			addExpression( AST_GetLastChild( cmd ), endrID );
		break;

		case AST_CALL:
			AST arg;
			for( arg = AST_GetFirstChild( cmd ); arg; arg = AST_GetNextSibling( arg ) ) {
				addParam( arg );
			}
			addCall( cmd );
		break;
		case AST_RETURN:
			expNode = AST_GetFirstChild( cmd );
			if( expNode ) {
				valID = addExpression( expNode );
				addReturnVal( valID );
			} else {
				addReturn( );
			}
			
		break;

		default: break;
	}
}

void addBlock( AST block ) {
	AST child;

	for ( 
		child = AST_GetFirstChild( block );
		child;
		command( block ), child = AST_GetNextSibling( child ); 
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
	//HEAD
	//function = build_function( func );
	for( pFunction = &(code->function); *pFunction != NULL; pFunction = &(*pFunction)->next );
	*pFunction = build_function( func );

	//END
}









