#ifndef BUILD_IR_CODE_C
#define BUILD_IR_CODE_C
#include "build_ir_code.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

Function _func;
IRCode _code;
CTE _lastExpr;

static void _addBlock( AST block );
static void _addCommand ( AST cmd  );
static Endr _addExpression( AST expr );
static Endr _addExpressionToEndr( AST expr, Endr endr );
static Endr _addBinOp( AST expr );
static Endr _addExprNode( AST node );

static unsigned long _label = 0;
static Endr _newLabel( ) {
	Endr endr;
	endr = Endr_New( ENDR_LABEL, ++_label );
	return endr;
}

static unsigned long _temp = 0;
static unsigned long _newTemp( ) {
	return _temp++;
}

static unsigned long _str = 0;
static Endr _newStringID( ) {
	Endr endr;
	endr = Endr_New( ENDR_STR, ++_label );
	return endr;
}

Endr args[3]; //unitialized
char temp[20] = "t";

static void _addDeclVar ( AST node ) {
	CTE cte;
	args[0] = Endr_NewAsString( ENDR_VAR, AST_GetStringValue( node ) );
	args[1] = Endr_New( ENDR_WORD, 0 );
	cte = CTE_New( ATTR_SIMPLE, args );
	Function_AddCTE( _func, cte );
}

static void _addGlobal ( AST node ) {
	CTE cte;
	args[0] = Endr_NewAsString( ENDR_VAR, AST_GetStringValue( node ) );
	cte = CTE_New( ATTR_SIMPLE, args);
	IRCode_AddGlobal( _code, cte );
}

static void _addLabel( Endr label ) {
	CTE cte;
	args[0] = label;
	cte = CTE_New( ATTR_SIMPLE, args );
	Function_AddCTE( _func, cte );
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
	args[0] = endr;
	cte = CTE_New( instr, args );
	Function_AddCTE( _func, cte );
	return endr;
}

static Endr _addOr( AST or ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel =  _newLabel( ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( or ) );
	args[0] = endr;
	cte = CTE_New( GOTO_IF, args );
	Function_AddCTE( _func, cte );
	endr = _addExprNode( AST_GetLastChild( or ) ); //ATENÇÂO NO FILHO
	args[0] = endrLabel;
	cte = CTE_New( LABEL, args );
	Function_AddCTE( _func, cte );
	return endr;
}

static Endr _addAnd( AST and ) {
	CTE cte;
	Endr endr, endrLabel;
	endrLabel =  _newLabel( ); //RESULTADO do OR
	args[1] = endrLabel;
	endr = _addExprNode( AST_GetFirstChild( and ) );
	args[0] = endr;
	cte = CTE_New( GOTO_IF_FALSE,  args );
	Function_AddCTE( _func, cte );
	endr = _addExprNode( AST_GetLastChild( and ) ); //ATENÇÂO NO FILHO
	args[0] = endrLabel;
	cte = CTE_New( LABEL, args );
	Function_AddCTE( _func, cte );
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
		Function_AddCTE( _func, cte );
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
	Function_AddCTE( _func, headCte );
	args[0] = Endr_NewAsString( ENDR_FUNC, AST_GetStringValue( call ) );
	currCTE = CTE_New( CALL, args );
	Function_AddCTE( _func, currCTE );
	return Endr_New( ENDR_RETEXP, 0 );
}

static Endr _addNew( AST newNode ) {
	CTE cte;
	AST type, expr;
	Instr instr = ATTR_NEW;
	Endr len, endr;
	expr = AST_GetFirstChild( newNode );
	type = AST_GetLastChild( newNode );

	if( AST_GetIntValue( type ) == 0 && ( AST_GetType( type ) == AST_BOOL || AST_GetType( type ) == AST_CHAR ) ) {
		instr = ATTR_NEW_BYTE;
	}

	len = _addExpression( expr );
	endr = Endr_New( ENDR_TEMP, _newTemp( ) );
	args[0] = endr;
	args[1] = len;
	cte = CTE_New( instr, args );
	Function_AddCTE( _func, cte );
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
	Endr e;
	if ( ! IRCode_FindString( _code, AST_GetStringValue( strVal ), &e ) ) {
		e = _newStringID( );
		IRCode_AddString( _code, e, AST_GetStringValue( strVal ) );

	}	
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
	Endr e =  _addExprNode( child ); //ATENÇÂO
	CTE c = Function_GetLastCTE( _func );
	c->args[0] = endr;
}

static void _addBlock( AST block ) {
	AST child;
	for ( 
		child = AST_GetFirstChild( block );
		child;
		_addCommand( child ), child = AST_GetNextSibling( child )
		);
} 


static void _addReturn( AST ret ) {
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

static void _addIfFalse( Endr val , Endr destination ) {
	CTE cte;
	args[0] = val;
	args[1] = destination;
	cte = CTE_New( GOTO_IF_FALSE, args );
	Function_AddCTE( _func, cte );
}

static void _addIf( Endr val , Endr destination ) {
	CTE cte;
	args[0] = val;
	args[1] = destination;
	cte = CTE_New( GOTO_IF, args );
	Function_AddCTE( _func, cte );
}

static void _addGoto( Endr destination) {
	CTE cte;
	args[0] = destination;
	cte = CTE_New( GOTO, args );
	Function_AddCTE( _func, cte );
}

static void _addCommand ( AST cmd  ) {
	AST expNode, blockNode;
	Endr labelBegin, labelEnd, labelElIf;
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
			AST _if;
			expNode = AST_GetFirstChild( cmd );
			blockNode = AST_GetNextSibling( expNode );
			elIfNode = blockNode;
			labelEnd = _newLabel( );
			labelNext;
			_if = AST_GetNextSibling( blockNode);

			// IF "NORMAL"
			Endr result = _addExpression( exprNode );
			if( _if ) {
				_addIfFalse( result, labelEnd );
			}
			else {
				labelNext( );

			}
			_addBlock( blockNode );
			_addGoto( labelEnd );
			//FOI
			
			for( _if ; _if && AST_GetType( _if ) == AST_ELSE_IF; _if = AST_GetNextSibling( _if )) {
				Endr result = _addExpression( exprNode );
				_addIfFalse( result, labelEnd );
				_addBlock( blockNode );
				_addGoto( labelEnd );
			}
			/*do {
				if ( elIfNode != blockNode ) {
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
				
			} while( elIfNode && AST_GetType( elIfNode ) == AST_ELSE_IF );
			_addGoto( labelEnd );
			if ( elIfNode ) { //É um else
				_addLabel(  labelElIf );
				blockNode = AST_GetFirstChild( elIfNode );
				_addBlock( blockNode );
			}*/
			_addLabel( labelEnd );
		break;

		case AST_ATTR:			
			rval = AST_GetFirstChild( cmd );
			lval = AST_GetLastChild( cmd );
			rValID = _addVar( rval );
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

static void _addArgs( AST paramList ) {
	AST param = AST_GetFirstChild( paramList );
	while( param ) {
		Function_AddArg( _func, AST_GetStringValue( param ) );
		param = AST_GetNextSibling( param );
	}
}

Function _buildFunction( AST nodeFunc ) {
	AST child;
	Function function;
	_func = Function_New( AST_GetStringValue( nodeFunc ) );

	child = AST_GetFirstChild( nodeFunc ) ;

	if ( AST_GetType( child ) == AST_PARAM_LIST ) {
		_addArgs( child );
		child = AST_GetNextSibling( child );
	}
	
	if ( AST_GetType( child ) != AST_BLOCK ) {
		child = AST_GetNextSibling( child );	
	}


	_addBlock( child );

	return _func;

}

void _addFunction( AST nodeFunc ) {
	Function* pFunction;
	for( pFunction = &(_code->functions); *pFunction != NULL; pFunction = &((*pFunction)->next));
	*pFunction = _buildFunction( nodeFunc );
}




IRCode buildIRCode ( AST tree ) {
	AST child;
	int i,j;
	_code = IRCode_New( );
	child = AST_GetFirstChild( tree );

	while( child != NULL ) {
		if( AST_GetType( child ) == AST_DECL_VAR ) {
			_addGlobal( child );
		}
		else {
			_addFunction( child );
		}
		
		child = AST_GetNextSibling( child );
	}
	return _code;
}
#endif