####Documentação do 3º Trabalho

######Lista de tipos de no da AST. as sublistas são os possíveis filhos de cada nó.
######As Sub-listas são os possíveis filhos de cada nó.

* If  
	- Expression  
	- Block  
	- Elseif*  
	- Else?  
* Else   
	- Block
* While  
	- Expression  
	- Block
* Return  
	- Expression?  
* New  
	- Expression  
	- INT | STRING | CHAR | BOOL
* Int  
* Char  
* Bool  
* String  
* And  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Or  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Not  
	- ['Operador'|'Literal'|Var|Call|New
* BoolVal  
* IntVal  
* StringVal  
* Plus   
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Minus  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Mul  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Div  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Greater  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* GreaterEqual  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Less  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* LessEqual  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Equal  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* Different  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* DeclFunction  
	- ParamList ?
	- INT | STRING | CHAR | BOOL

* Param  
	- INT | STRING | CHAR | BOOL
* ParamList  
	- Param+
* Expression  
	- ['Operador'|'Literal'|Var|Call|New]
* Call  
	- Expressions*
* Attr  
	- Var
	- Expression
* Block  
	- While  
	- If  
	- Attr  
	- Call  
	- Return  
* Program  
	{
	- DeclVar
	- DeclFunction
	}
* DeclVar  
	- INT | STRING | CHAR | BOOL
* Var  
	- Expressions*
* UnaryMinus  
	- ['Operador'|'Literal'|Var|Call|New] (2)
* ElseIf  
	- Block  

####### ? Significam opcionais
####### + Significa pelo menos 1
####### * Significa pelo menos 0
####### (n) | n é o número de itens exigidos
