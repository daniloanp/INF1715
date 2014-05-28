#ifndef BUILD_IR_CODE_C
#define BUILD_IR_CODE_C

Function _func;

static unsigned long _label = 0;
static unsigned long _newLabel( ) {
	return _label++;
}

static unsigned long _temp = 0;
static unsigned long newTemp( ) {
	return _temp++;
}



Endr args[3]; //unitialized
char temp[20] = "t";

void _addDecl ( char* s ) {
	CTE cte;
	args[0] = Endr_NewAsString( ENDR_VAR, s );
	args[1] = Endr_New( ENDR_VAL, 0 );
	cte = CTE_New( ATTR_SIMPLE, args );
	//ADD_FUNCTION
}

void _addLabel( unsigned long label ) {
	CTE cte;
	args[0] = Endr_New( ENDR_LABEL, label );
	cte = CTE_New( ATTR_SIMPLE, args );
	//ADD_FUNCTION
}



void _addIfFalse( Endr condition, Endr destination ) {

}

Endr _addBinOp( AST_BinOp expr ) {
	Endr endr;
	CTE cte;
	Instr inst;

	args[1] = _addExprNode( AST_GetFirstChild( expr ) );
	args[2] = _addExprNode( AST_GetLastChild( expr ) );

	switch ( AST_GetType( expr ) ) {
		case AST_LESS:
			instr = ATTR_LE;
		break;
		case AST_LESS_EQUAL:
			instr = ATTR_LTE;
		break;
		case AST_GREATER:
			instr = ATTR_GT;
		break;
		case AST_GREATER_EQUAL:
			instr = ATTR_GTE;
		break;
		case ASFT_EQUAL:
			instr = ATTR_EQ;
		break;
		case AST_DIFFERENT:
			instr = ATTR_NE;
		break;
		case AST_PLUS:
			instr = ATTR_ADD;
		break;
		case AST_MINUS:
			instr = ATTR_MIN;
		break;
		case AST_MUL:
			instr = ATTR_MUL;
		break;
		case AST_DIV:
			instr = ATTR_DIV;
		break;
		default:
			assert(0);
		break;
	}
	endr = Endr_New( ENDR_TEMP, _newTemp( ) );
	cte = CTE_New( instr, args );
	//ADD_FUNCTION
	return args[0];
}

Endr _addOr( AST or ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel = Endr_New( ENDR_Label, _newLabel() ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( or ) );
	arg[0] = endr;
	cte = CTE_New( GOTO_IF, args );
	//ADD CTE Function
	endr = _addExprNode( AST_GetLastChild( or ) ); //ATENÇÂO NO FILHO
	cte = CTE_New( LABEL, endrLabel );
	//ADDLabelCTE
	return endr;
}

Endr _addAnd( AST and ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel = Endr_New( ENDR_Label, _newLabel() ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( or ) );
	cte = CTE_New( GOTO_IF_FALSE, endr, endrLabel );
	//ADD CTE Function
	endr = _addExprNode( AST_GetLastChild( or ) ); //ATENÇÂO NO FILHO

	cte = CTE_New( LABEL, endrLabel );
	//ADDLabelCTE
	return endr;
}


Endr _addVar( AST var ) {
	AST expr;
	ENDR endrID, endr, endrI;
	CTE cte;
	endrID = Endr_NewAsString( ENDR_VAR, AST_GetStringValue( var ) );
	endr = endrID;
	for ( expr = AST_GetFirstChild( var ); expr; expr = AST_GetNextSibling( expr ) ) {
		endrI = _addExpression( expr );
		endr = Endr_New( ENDR_TEMP, _newTemp( ) );
		args[0] = endr;
		args[1] = endrID;
		args[2] = endrI;
		cte = CTE_New( ATTR_FROM_ARR, args );
		//ADDFUNCTION
		endrID = endr;
	}

	return endrID;
}

//Call into expressions
Endr _addCall( AST call ) {
	Endr endr;
	CTE headCte = NULL, currCTE;
	for( expr = AST_GetLastChild( call ); expr; AST_GetPrevSibling( expr ) ) {
		endr = _addExpression( expr );
		args[0] = endr;
		if( !headCte ) {
			headCte = CTE_New( PARAM,  args );
			currCTE = headCte;
		}
		else {
			currCTE->next = CTE_New( PARAM,  args );
			currCTE = currCTE->next;
		}
	}
	//ADD headCTE;
	args[0] = Endr_NewAsString( ENDR_FUNC, AST_GetStringValue( call ) );
	currCTE = CTE_New( CALL, args );
	//ADD CALL;
	return Endr_New( ENDR_RETEXP, 0 );
}

Endr _addNew( AST newNode ) {
	AST type, expr;
	Instr instr = ATTR_NEW;
	Endr len, endr;
	expr = AST_GetFirstChild( newNode );
	type = AST_GetLastChild( newNode );

	if( AST_GetIntValue( type ) == 0 && ( AST_GetType( type ) == AST_BOOL || AST_GetType == AST_CHAR ) ) {
		instr = ATTR_NEW_BYTE;
	}

	len = _addExpression( expr );
	endr = new ENDR( ENDR_TEMP, _newTemp());
	args[0] = endr;
	args[1] = len;
	cte = CTE_New( instr, args );
	//ADD_IT
	return endr;
}

Endr _addIntVal ( AST intVal ) {
	Endr e = Endr_New( ENDR_WORD, AST_GetIntValue( intVal ));
	return e;
}

Endr _addBoolVal ( AST intVal ) {
	Endr e = Endr_New( ENDR_WORD, AST_GetIntValue( intVal ));
	return e;
}

Endr _addStringVal ( AST intVal ) {

	//Find at list
	Endr e = Endr_New( ENDR_STR, );


	return e;
}


Endr _addExprNode( AST node ) {
	Endr endr;
	switch ( AST_GetType( node ) ) {
		case AST_LESS:
		case AST_LESS_EQUAL:
		case AST_GREATER:
		case AST_GREATER_EQUAL:
		case AST_EQUAL:
		case AST_DIFFERENT:
		case AST_PLUS:
		case AST_MINUS:
		case AST_MUL:
		case AST_DIV:
			endr = _addBinOp( child );
		break;

		case AST_AND:
			endr = _addAnd( child );
		break;

		case AST_OR:
			endr = _addOr( child );
		break:

		case AST_VAR: //with array acess;
			endr = _addVar( child );
		break;

		case AST_CALL:
			endr = _addCall( child );
		break; 

		case AST_NEW:
			endr = _addNew( child );
		break;

		case AST_BOOL_VAL:
			endr = _addBoolVal( child );
		break;

		case AST_INT_VAL:
			endr = _addIntVal( child );
		break;

		case AST_STRING_VAL:
			endr = _addStringVal( child );
		break;

		default break;
	}
	return endr;
}

Endr _addExpression( AST expr, char *s ) {
	AST child = AST_GetFirstChild( expr );
	_addExprNode( child );
}


static void _addCommand ( AST cmd  ) {
	AST expNode, blockNode;
	unsigned long labelBegin, labelEnd;
	Endr valID = NULL;

	switch ( AST_GetType( cmd ) ) {
		case AST_DECL_VAR:
			_addDecl(  AST_GetStringValue( cmd ) );
		break;

		case AST_WHILE:
			labelBegin = newLabel( );
			labelEnd = newLabel( );
			_addLabel(  labelBegin );
			valID = _addExpression( AST_GetFirstChild( cmd ), NULL );
			_addIfFalse( varID, labelEnd );
			_addBlock( AST_GetLastChild );
			_addGoto(  labelBegin );
			_addLabel(  labelEnd );
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
					_addLabel(  labelElIf );
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
				_addIfFalse(  valID, labelElIf );
				_addBlock( blockNode );
				_addGoto( labelEnd );


			} while( elIfNode && AST_GetType( elIfNode ) == AST_ELSE_IF );

			if ( elIfNode ) { //É um else
				_addLabel(  labelElIf );
				blockNode = AST_GetFirstChild( elIfNode );
				_addBlock( blockNode );
			}
			_addLabel( labelEnd);
		break;

		case AST_ATTR:
			AST rval, lrval;
			char* rvalID = NULL;
			rval = AST_GetFirstChild( cmd );
			lval = AST_GetLastChild( cmd );
			rvalID = _addExpression( rval, NULL );
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

Function _buildfunction( AST nodeFunc ) {
	
	function = (Function)malloc( sizeof( struct _function ) );
	_func = function; //Seta global por praticidade

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

	_addBlock( functionChild );

	return function;

}

void _addFunction( AST nodeFunc ) {
	Function* pFunction;
	for( pFunction = &(code->function); *pFunction != NULL; pFunction = &(*pFunction->next) );
	*pFunction = _buildFunction( func );
}



void build_ir ( AST tree ) {
	AST child; // a DeclVar OR a DeclFunction
	int i,j;

	if (tree == NULL) {
		return;
	}

	child = AST_GetFirstChild( tree );

	while( child != NULL ) {
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