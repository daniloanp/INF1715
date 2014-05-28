#ifndef BUILD_IR_CODE_C
#define BUILD_IR_CODE_C
#include "build_ir_code.h"
#include <malloc.h>

Function _func;
IRCode _code;

static void _addBlock( AST block );
static void _addCommand ( AST cmd  );
static Endr _addExpression( AST expr );
static Endr _addExpressionToEndr( AST expr, Endr endr );
static Endr _addBinOp( AST expr );
static Endr _addExprNode( AST node );

static unsigned long _label = 0;
static unsigned long _newLabel( ) {
	return _label++;
}

static unsigned long _temp = 0;
static unsigned long _newTemp( ) {
	return _temp++;
}

Endr args[3]; //unitialized
char temp[20] = "t";

static void _addDeclVar ( AST node ) {
	CTE cte;
	args[0] = Endr_NewAsString( ENDR_VAR, node );
	args[1] = Endr_New( ENDR_WORD, 0 );
	cte = CTE_New( ATTR_SIMPLE, args );
	//ADD_FUNCTION
}

static void _addLabel( unsigned long label ) {
	CTE cte;
	args[0] = Endr_New( ENDR_LABEL, label );
	cte = CTE_New( ATTR_SIMPLE, args );
	//ADD_FUNCTION
}

static Endr _addBinOp( AST expr ) {
	Endr endr;
	CTE cte;
	Instr instr;

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
		case AST_EQUAL:
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
			return endr;
		break;
	}
	endr = Endr_New( ENDR_TEMP, _newTemp( ) );
	cte = CTE_New( instr, args );
	//ADD_FUNCTION
	return endr;
}

static Endr _addOr( AST or ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel = Endr_New( ENDR_LABEL, _newLabel() ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( or ) );
	args[0] = endr;
	cte = CTE_New( GOTO_IF, args );
	//ADD CTE Function
	endr = _addExprNode( AST_GetLastChild( or ) ); //ATENÇÂO NO FILHO
	args[0] = endrLabel;
	cte = CTE_New( LABEL, args );
	//ADDLabelCTE
	return endr;
}

static Endr _addAnd( AST and ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel = Endr_New( ENDR_LABEL, _newLabel() ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( and ) );
	args[0] = endr;
	cte = CTE_New( GOTO_IF_FALSE,  args );
	//ADD CTE Function
	endr = _addExprNode( AST_GetLastChild( and ) ); //ATENÇÂO NO FILHO
	args[0] = endrLabel;
	cte = CTE_New( LABEL, args );
	//ADDLabelCTE
	return endr;
}


static Endr _addVar( AST var ) {
	AST expr;
	Endr endrID, endr, endrI;
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
static Endr _addCall( AST call ) {
	AST expr;
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

static Endr _addNew( AST newNode ) {
	CTE cte;
	AST type, expr;
	Instr instr = ATTR_NEW;
	Endr len, endr;
	expr = AST_GetFirstChild( newNode );
	type = AST_GetLastChild( newNode );

	if( AST_GetIntValue( type ) == 0 && ( AST_GetType( type ) == AST_BOOL || AST_GetType == AST_CHAR ) ) {
		instr = ATTR_NEW_BYTE;
	}

	len = _addExpression( expr );
	endr = Endr_New( ENDR_TEMP, _newTemp());
	args[0] = endr;
	args[1] = len;
	cte = CTE_New( instr, args );
	//ADD_IT
	return endr;
}

static Endr _addIntVal ( AST intVal ) {
	Endr e = Endr_New( ENDR_WORD, AST_GetIntValue( intVal ));
	return e;
}

static Endr _addBoolVal ( AST intVal ) {
	Endr e = Endr_New( ENDR_BYTE, AST_GetBoolValue( intVal ) );
	return e;
}

static Endr _addStringVal ( AST strVal ) {

	//Find at list
	Endr e = Endr_NewAsString( ENDR_STR, AST_GetStringValue( strVal ) ); //ERRADO


	return e;
}


static Endr _addExprNode( AST node ) {
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
			endr = _addBinOp( node );
		break;

		case AST_AND:
			endr = _addAnd( node );
		break;

		case AST_OR:
			endr = _addOr( node );
		break;

		case AST_VAR: //with array acess;
			endr = _addVar( node );
		break;

		case AST_CALL:
			endr = _addCall( node );
		break; 

		case AST_NEW:
			endr = _addNew( node );
		break;

		case AST_BOOL_VAL:
			endr = _addBoolVal( node );
		break;

		case AST_INT_VAL:
			endr = _addIntVal( node );
		break;

		case AST_STRING_VAL:
			endr = _addStringVal( node );
		break;

		default: break;
	}
	return endr;
}

static Endr _addExpression( AST expr ) {
	AST child = AST_GetFirstChild( expr );
	return _addExprNode( child );
}
static Endr _addExpressionToEndr( AST expr, Endr endr ) {
	AST child = AST_GetFirstChild( expr );
	return _addExprNode( child ); //ATENÇÂO
}

static void _addBlock( AST block ) {
	AST child;
	for ( 
		child = AST_GetFirstChild( block );
		child;
		_addCommand( child ), child = AST_GetNextSibling( child )
		);
} 


static _addReturn( AST ret ) {
	CTE cte;
	Endr endr;
	if( AST_GetFirstChild( ret ) ) {
		endr = _addExpressionToEndr( AST_GetFirstChild( ret ), endr );
		args[0] = endr;
		cte = CTE_New( RET_VAL, args );
	} 
	else {
		cte = CTE_New( RET, args );
	}

}
/*if( expNode ) {
		valID = _addExpression( expNode ); //WARNING
	_addReturnVal( valID );
} else {
	_addReturn( );
}*/

static endr _addIfFalse( ){

}

static void _addGoto( Endr Label ) {

}

static void _addCommand ( AST cmd  ) {
	AST expNode, blockNode;
	unsigned long labelBegin, labelEnd, labelElIf;
	Endr valID, varID;
	Endr rValID;
	AST rval, lval;
	AST elIfNode;

	switch ( AST_GetType( cmd ) ) {
		case AST_DECL_VAR:
			_addDeclVar(  cmd  );
		break;

		case AST_WHILE:
			labelBegin = _newLabel( );
			labelEnd = _newLabel( );
			_addLabel(  labelBegin );
			valID = _addExpression( AST_GetFirstChild( cmd ) );
			_addIfFalse( varID, labelEnd );
			_addBlock( AST_GetLastChild( cmd ) );
			_addGoto(  labelBegin );
			_addLabel(  labelEnd );
		break;

		case AST_IF:			
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
					labelElIf = _newLabel( );
				} else {
					labelElIf = labelEnd;
				}

				valID = _addExpression( expNode );
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
			rval = AST_GetFirstChild( cmd );
			lval = AST_GetLastChild( cmd );
			rValID = _addExpression( rval );
			_addExpressionToEndr( lval, rValID );

		break;

		case AST_CALL:
			_addCall( cmd );
		break;

		case AST_RETURN:
			_addReturn( cmd );
		break;

		default: break;
	}
}



Function _buildFunction( AST nodeFunc ) {
	AST child;
	Function function;
	function = (Function)malloc( sizeof( struct _function ) );
	_func = function; //Seta global por praticidade

	child = AST_GetFirstChild( nodeFunc ) ;

	function->id = AST_GetStringValue( nodeFunc );
	if ( AST_GetType( child ) == AST_PARAM_LIST ) {
		addArgs( child );
		child = AST_GetNextSibling( child );
	}
	
	if ( AST_GetType( child ) != AST_BLOCK ) {
		child = AST_GetNextSibling( child );	
	}

	//BLOCK

	_addBlock( child );

	return function;

}

void _addFunction( AST nodeFunc ) {
	Function* pFunction;
	for( pFunction = &(_code->functions); *pFunction != NULL; pFunction = &((*pFunction)->next));
	*pFunction = _buildFunction( nodeFunc );
}




void build_ir ( AST tree ) {
	AST child; // a DeclVar OR a DeclFunction
	int i,j;
	_code = IRCode_New( );
	if (tree == NULL) {
		return;
	}
	child = AST_GetFirstChild( tree );

	while( child != NULL ) {
		if( AST_GetType( child ) == AST_DECL_VAR ) {
			_addDeclVar( child );
		}
		else {
			_addFunction( child );
		}
		
		child = AST_GetNextSibling( child );
	}
}
#endif