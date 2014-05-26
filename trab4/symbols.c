#ifndef SYMBOLS_C
#define SYMBOLS_C
#include "symboltype.h"
#include "symbols.h"
#include "symboltable.h"
#include <stdio.h>
#include "../trab3/ast.h"
#include <assert.h>
//#include "../trab3/ast.h"
static int dimension = 0;
static char *name;
static int type;
static SymbolType currReturnType;

bool Symbols_ExpressionDescendants ( SymbolTable* st, AST node );
bool Symbols_Expression(  SymbolTable* st, AST expr  );
bool Symbols_Block( SymbolTable* st, AST block );

static SymbolTable* globals = NULL;


static bool fail(const char* msg, const char* name, AST node) {
   fprintf(stderr, "%s - %s at line %d\n", msg, name, AST_GetLine(node));
   return false;
}

static bool op_fail(const char* msg, AST node) {
   fprintf(stderr, "%s - at line %d\n", msg,  AST_GetLine(node));
   return false;
}

   

bool Symbols_Param( SymbolTable* st, AST declVar ) {
   char* name;
   int dimension;
   AST type;
   Symbol* sym;
   name = AST_GetStringValue( declVar );
   sym = SymbolTable_getInScope( st, name );

   if( ! sym ) {
      
      type = AST_GetFirstChild( declVar );
      
      dimension = AST_GetIntValue( type );
      return SymbolTable_add( st, name, AST_GetSymType( declVar ) , declVar ) ;
   }
   else {
      if( SYM_IsFunctionType( sym->type ) ) {
          fprintf(stderr,  "Redeclared variable, previous declared at line %d! - error at line %d\n", sym->line, AST_GetLine(declVar) );
          return false;
      }
      fprintf(stderr,  "Redeclared variable, previous declared at line %d! - error at line %d\n", sym->line, AST_GetLine(declVar) );
      return false;
   }  
}


bool Symbols_DeclVar( SymbolTable* st, AST declVar ) {
   char* name;
   int dimension;
   AST type;
   Symbol* sym;
   name = AST_GetStringValue( declVar );
   sym = SymbolTable_getInScope( st, name );

   if( ! sym ) {
      
      type = AST_GetFirstChild( declVar );
      
      dimension = AST_GetIntValue( type );
      AST_SetSymType ( declVar,  SYM_NewSymbolType(AST_GetType( type ),dimension) );

      return SymbolTable_add( st, name, AST_GetSymType( declVar ) , declVar ) ;
   }
   else {
      if( SYM_IsFunctionType( sym->type ) ) {
          fprintf(stderr,  "Redeclared variable, previous declared at line %d! - error at line %d\n", sym->line, AST_GetLine(declVar) );
          return false;
      }
      fprintf(stderr,  "Redeclared variable, previous declared at line %d! - error at line %d\n", sym->line, AST_GetLine(declVar) );
      return false;
   }  
}

bool Symbols_If(  SymbolTable* st, AST cmdif  ) {
   bool ret;
   SymbolTable* lst;
   AST expr,block,elseifs;
   expr = AST_GetFirstChild( cmdif );

   ret = Symbols_Expression( st, expr );
   
   if ( ret ) {
      block = AST_GetNextSibling( expr );
      lst = SymbolTable_new( st );
      ret = Symbols_Block( lst,  block );
      SymbolTable_delete( lst );

      //Elseifs
      for ( elseifs = AST_GetNextSibling( block ); elseifs && ret; elseifs = AST_GetNextSibling( elseifs )) {
         if( AST_GetType ( elseifs ) == AST_ELSE_IF ) {
            expr = AST_GetFirstChild( elseifs );
            ret = Symbols_Expression( st, expr );
            if( ret ) {
               block = AST_GetNextSibling( expr );
               lst = SymbolTable_new( st );
               ret = Symbols_Block( lst,  block );
               SymbolTable_delete( lst );
            }
         } 
         else {
            block = AST_GetFirstChild( elseifs );
            lst = SymbolTable_new( st );
            ret = Symbols_Block( lst,  block );
            SymbolTable_delete( lst );
            break;
         }
      }


   }
   return ret;
}

bool Symbols_While ( SymbolTable* st, AST cmdWhile) {
   bool ret;
   SymbolTable* lst;
   AST expr,block;
   expr = AST_GetFirstChild( cmdWhile );
   ret = Symbols_Expression( st, expr );
   if ( ret ) {
      block = AST_GetNextSibling( expr );
      lst = SymbolTable_new( st );
      ret = Symbols_Block( lst,  block );
      SymbolTable_delete( lst );
   }
   return ret;
}

bool Symbols_Return ( SymbolTable* st, AST cmdReturn) {
   
   AST expr,block;
   expr = AST_GetFirstChild( cmdReturn );
   if( expr != NULL && !currReturnType.type  ){
      fprintf(stderr, "incompatible types when returning. Void Function can't return something - at line %d\n" , AST_GetLine(cmdReturn) );
      return false;
   }

   if( expr == NULL && currReturnType.type ) {
      fprintf(stderr, "incompatible types when returning. Function should return something - at line %d\n" , AST_GetLine(cmdReturn) );
      return false;
   }

   if( expr == NULL && !currReturnType.type ) {
      AST_SetSymType( cmdReturn, SYM_NewSymbolType( 0,  0 ));
      return true;
   }
   if (! Symbols_Expression( st, expr ) ) {
      return false;
   }
   if( ! SYM_CompatibleTypes( AST_GetSymType(expr) , currReturnType ) ) {
      fprintf(stderr, "incompatible types when returning - at line %d\n" , AST_GetLine(cmdReturn) );
      return false;
   }
   
   return true;
}



bool Symbols_Call ( SymbolTable* st, AST call) {
   Symbol* sym;
   SymbolType stp;
   char* name; 
   AST param,expr = NULL;
   int i = 1;
   
   name = AST_GetStringValue( call );
   sym = SymbolTable_get( st, name ); 
   if ( sym ) {
      if( SYM_IsFunctionType ) { //Se não é var é função
         stp = sym->type;
         stp.type = stp.type/10;

         param = AST_GetFirstChild( sym->node );
         if( AST_GetType(param) == AST_PARAM_LIST ) { // Se tem parametros; 
            expr = AST_GetFirstChild( call );
            for( param = AST_GetFirstChild(param); param; param = AST_GetNextSibling( param ), i++ ) {
               if( !expr ) {
                  fprintf(stderr, "At Call of '%s', the param %d is missing - at line %d\n" , name, i,  AST_GetLine(call) );
                  return false;
               }
               if( ! Symbols_Expression( st, expr ) ) {
                  return false; 
               }

               if( ! SYM_CompatibleTypes( AST_GetSymType(expr), AST_GetSymType(param) ) ) {
                  fprintf(stderr, "At Call of '%s', the argument %d has wrong type - at line %d\n" , name, i,  AST_GetLine(call) );
                  return false;
               }
               expr = AST_GetNextSibling(expr);
            }
         }else if( AST_GetFirstChild(call) != NULL ) { //Argumento inútil
            fprintf(stderr, "At Call of '%s', the function doesn't has any parameter- at line %d\n" , name,  AST_GetLine(call) );
            return false;
         }
         if( expr != NULL ) {
            fprintf(stderr, "At Call of '%s', extra arguments. The function require only %d - at line %d\n" , name, i,  AST_GetLine(call) );
         }
         AST_SetSymType( call , stp  );
         return true;
      } 
      else {
         return fail("is not a function!", name, call);
      }
   } 
   else {
      return fail("undeclared function!", name, call);
   }
   
}
bool Symbols_Var  ( SymbolTable* st, AST var ) {
   Symbol* sym;
   char* name; 
   name = AST_GetStringValue( var );
   sym = SymbolTable_get( st, name );
   if ( sym ) {
      if (!  SYM_IsFunctionType( sym->type )  ) { //É variavel;
         AST_SetSymType( var , sym->type );
         return true;
      }
      else {
       return fail("is not a variable!", name, var);
      }
   }
   else {
      return fail("undeclared variable!", name, var);
   }
}

bool Symbols_BoolBinOp( SymbolTable* st, AST op ) {
   bool ret;
   SymbolType tp;
   AST node;
   node = AST_GetFirstChild( op );
   //Só vai ter dois mesmo...
   while( node != NULL) {
      ret = Symbols_ExpressionDescendants( st, node );
      if( !ret ) {
         return false;
      } 
      else {
         tp = AST_GetSymType( node );
         if( !( tp.type == SYM_BOOL && tp.dimension == 0 ) ) {
            op_fail("Non-boolean expression used with bool operator", node);
            return false;
         }
      }
   node = AST_GetNextSibling( node );
   }
   tp = SYM_NewSymbolType( SYM_BOOL, 0 );
   AST_SetSymType(op, tp);

   return true;
}

bool Symbols_UnaryMinusOp( SymbolTable* st, AST op ) {
   bool ret;
   SymbolType tp;
   AST node;
   //Primeiro Filho
   node = AST_GetFirstChild( op );
   ret = Symbols_ExpressionDescendants( st, node );
   if( !ret ) {
      return false;
   } 
   else {
      tp = AST_GetSymType( node );
      if( (tp.type == SYM_INT || tp.type == SYM_CHAR )&& tp.dimension == 0 ) {
         op_fail("Non-numerical expression used with 'Minus' operator", node);
         return false;
      }
   }
   tp = SYM_NewSymbolType( SYM_INT, 0 );

   AST_SetSymType(op, tp);
   return true;
}

bool Symbols_NotOp( SymbolTable* st, AST op ) {
   bool ret;
   SymbolType tp;
   AST node;
   //Primeiro Filho
   node = AST_GetFirstChild( op );
   ret = Symbols_ExpressionDescendants( st,  node );
   if( !ret ) {
      return false;
   } 
   else {
      tp = AST_GetSymType( node );
      if( tp.type == SYM_INT && tp.dimension == 0 ) {
         op_fail("Non-boolean expression used with 'not' operator", node);
         return false;
      }
   }
   tp = SYM_NewSymbolType( SYM_BOOL, 0 );
   AST_SetSymType(op, tp);
   return true;
}

bool Symbols_IntBinOp( SymbolTable* st, AST op ) {
   bool ret;
   SymbolType tp;
   AST node;
   //Primeiro Filho
   node = AST_GetFirstChild( op );
   //Só vai ter dois mesmo...
   while( node != NULL) {
      ret = Symbols_ExpressionDescendants( st, node );
      if( !ret ) {
         return false;
      } 
      else {
         tp = AST_GetSymType( node );
         if( ! ((tp.type == SYM_INT || tp.type == SYM_CHAR )&& tp.dimension == 0 )) {
            op_fail("Non-numerical expression used with numerical operator", node);
            return false;
         }
      }      
   node = AST_GetNextSibling( node );
   }

   tp = SYM_NewSymbolType( SYM_INT, 0 );
   AST_SetSymType(op, tp);

   return true;
}



bool Symbols_Operators( SymbolTable* st, AST node ) {
    switch( AST_GetType( node ) ) {
      case AST_AND:
      case AST_OR:
         return Symbols_BoolBinOp( st, node );
      break;

      case AST_UNARYMINUS:
         return Symbols_UnaryMinusOp( st, node );
      break;

      case AST_NOT:
         return Symbols_NotOp( st, node );
      break;

      default: //Só pode ser IntBinOP
         return Symbols_IntBinOp( st, node );
      break;
   }
}


bool Symbols_New( SymbolTable* st, AST newArr ) {
   AST node;
   SymbolType tp;
   node = AST_GetFirstChild(newArr);

   if( ! Symbols_Expression( st, node ) ) {
      return false;
   }



   node = AST_GetLastChild( newArr );
   tp = SYM_NewSymbolType( AST_GetType(node), AST_GetIntValue( node ) + 1 );
   AST_SetSymType(newArr, tp);
   return true;
}

bool Symbols_Literal( SymbolTable* st, AST lit ) {

   SymbolBaseType bt = 0;
   unsigned int dimension = 0;
   switch( AST_GetType( lit )) {
      case AST_INT_VAL:
         bt = SYM_INT;
      break;
      case AST_BOOL_VAL:
         bt = SYM_BOOL;
      break;
      case AST_STRING_VAL:
         bt = SYM_CHAR;
         dimension = 1;
      break;
      default: 
         fprintf(stderr, "Oops;;; %d\n\n", AST_GetType(lit));
         return false; //Não vai acontecer;;;
         break;
   }
   
   AST_SetSymType( lit, SYM_NewSymbolType( bt, dimension ) );
   return true;
}
//I don't now if that is a great name :-(
bool Symbols_Constants( SymbolTable* st, AST node ) {
   switch( AST_GetType( node ) ) {
      case AST_CALL:
         return Symbols_Call( st, node );
      break;
      case AST_VAR:
         return Symbols_Var( st, node );
      break;
      case AST_NEW:
         return Symbols_New( st, node );
      break;
      default: //Só pode ser Literal
         return Symbols_Literal( st, node );
      break;
   }
}


bool Symbols_ExpressionDescendants ( SymbolTable* st, AST node ) {
   if( AST_IsOperatorNode( node )) {

      return Symbols_Operators( st, node );
   } 
   else {
      return Symbols_Constants( st, node );
   }
}

bool Symbols_Expression(  SymbolTable* st, AST expr  ) {
   AST node;
   node = AST_GetFirstChild( expr );

   if(! Symbols_ExpressionDescendants( st, node) ) {
      return false;
   }

   AST_SetSymType(expr, AST_GetSymType( node ));
   return true;
}

bool Symbols_Attr ( SymbolTable* st, AST attr ) {
   AST var,expr;

   var = AST_GetFirstChild( attr );
   
   if( ! Symbols_Var ( st, var ) ) { 
      return false;
   }

   expr = AST_GetLastChild ( attr );
   if ( ! Symbols_Expression ( st, expr ) ) { 
      return false;
   }


   if( ! SYM_CompatibleTypes( AST_GetSymType(var) , AST_GetSymType(expr)) ) {
      //printf("\n\n%d --- %d\n\n", AST_GetSymType(var) , AST_GetSymType(expr) );
      fprintf(stderr, "incompatible types when assigning - at line %d\n" , AST_GetLine(var) );
      return false;
   }


  return true;
}

bool Symbols_Command( SymbolTable* st, AST command) {
   switch ( AST_GetType( command ) ) {
      case AST_IF:
         return Symbols_If( st, command );
      break;
      case AST_WHILE:
         return Symbols_While( st, command );
      break;
      case AST_RETURN:
         return Symbols_Return( st, command );
      break;
      case AST_CALL:
         return Symbols_Call( st, command );
      break;
      case AST_ATTR:
         return Symbols_Attr( st, command );
      break;
      default: break;
   }
   return true;
}

bool Symbols_Block( SymbolTable* st, AST block ) { 
   bool ret = true;
   AST node;

   node = AST_GetFirstChild( block );
   while( ret && node && AST_GetType( node ) == AST_DECL_VAR ) {
      ret =  Symbols_DeclVar( st, node ) ;
      node = AST_GetNextSibling( node );
   }

   for( node; node && ret; node = AST_GetNextSibling(node) ) {
      ret = Symbols_Command( st , node );
   }

   return ret;
}

//Já anota nos nós "Param" o seu tipo, não cria escopo.
bool Symbols_DeclFunction ( SymbolTable* st, AST declFunc ) {
   bool ret = true;
   char* name;
   int dimension = 0;
   int tp = 1; // void val
   Symbol* sym;
   AST params, type = NULL,node, node_;

   name = AST_GetStringValue( declFunc );
   sym = SymbolTable_getInScope (st, name );

   if( ! sym ) {

      type = AST_GetPrevSibling( AST_GetLastChild( declFunc) );
      if ( type && ! AST_IsBaseNodeType( type ) ) {
         type = NULL;
      }
      params = AST_GetFirstChild( declFunc );
      if ( AST_GetType( params ) != AST_PARAM_LIST ) {
         params = NULL;
      } else {

         for( node = AST_GetFirstChild( params ) ; node; node = AST_GetNextSibling( node ) ) {
               node_ = AST_GetFirstChild( node );
               tp = AST_GetType( node_ );
               dimension = AST_GetIntValue( node_ );
               AST_SetSymType( node, SYM_NewSymbolType( tp ,dimension ) ); //Apenas anota na arvore.

               if( !ret ) {
                        return false;
               }
         }
      }

      if (  type !=  NULL ) {
         tp = AST_GetType( type )*10;
         dimension = AST_GetIntValue( type );
      }
      AST_SetSymType( declFunc, SYM_NewSymbolType( tp  ,dimension ) );
      ret = SymbolTable_add( st, name, SYM_NewSymbolType( tp  ,dimension ) , declFunc );

      return ret;
   }
   else {
      if( SYM_IsFunctionType( sym->type ) ) {
         return fail( "Redeclared function!", name, declFunc );
      }
      fprintf(stderr,  "Redeclared name, previous declared as variable at line %d! - error at line %d\n", sym->line, AST_GetLine(declFunc) );
      return false;
   }
}

//Não anota nos nos Param seu tipo, mas adiciona no escopo.
bool Symbols_Function( SymbolTable* st , AST declFunc ) {
   char* name;
   Symbol* sym;
   SymbolTable* lst;
   AST params, type, block,node;
   bool ret;

   
   name = AST_GetStringValue( declFunc );
   sym = SymbolTable_get ( st, name );

   if( !sym ) {
      "Something are Strange, hre";
      return false;
   }
   currReturnType = sym->type;
   currReturnType.type = currReturnType.type/10;


   
   block = AST_GetLastChild( declFunc );
   type = AST_GetPrevSibling( AST_GetLastChild( declFunc) );
   if ( type && ! AST_IsBaseNodeType( type ) ) {
      type = NULL;
   }
   params = AST_GetFirstChild( declFunc );
   if ( ! AST_GetType( params ) == AST_PARAM_LIST ) {
      params = NULL;
   }

   //Cria Escopo
   lst = SymbolTable_new( st );

   //Registra os parametros na tabela
   if( params != NULL && params != block ) {

      for( node = AST_GetFirstChild( params ) ; node; node = AST_GetNextSibling( node ) ) {
            if( ! Symbols_Param( lst, node ) ) { //Adiciona parametros ao escopo.
               
               break;
            }
      }
   }

   ret = Symbols_Block( lst, block );


   SymbolTable_delete( lst ); //Deleta Escopo;
   return ret;
}
         


bool Symbols_annotate(AST program) {

   SymbolTable* st; //Escopo Global
   AST child;
   bool ret = true;
   st = SymbolTable_new(NULL);
   globals = st;

   //Preencher Global primeiro.
   for ( child = AST_GetFirstChild(program);
         child && ret;
         child = AST_GetNextSibling(child) ) {

      if( AST_GetType(child) == AST_DECL_VAR ) {

         ret = Symbols_DeclVar( st, child );
      }
      else if( AST_GetType(child) == AST_DECL_FUNCTION ) {
         ret = Symbols_DeclFunction( st, child );
      }
      else {
         fprintf(stderr, "There're something wrong with your AST");
         return false;
      }
   }
   

   //Iniciar recursão entre nós.
   for( child = AST_GetFirstChild(program);
         child && ret;
         child = AST_GetNextSibling(child) ) {
         if( AST_GetType(child) == AST_DECL_FUNCTION ) {
           ret =  Symbols_Function( st, child );
         }
   }
   SymbolTable_delete( st );
   return ret;
}

#endif

