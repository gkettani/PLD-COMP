grammar ifcc;

axiom: prog ;

prog: 'int' 'main' '(' ')' startMainBlock blocInstr endMainBlock 
;

startMainBlock: '{'
;

endMainBlock: '}' 
;

type: INT | CHAR 
;

instruction: declare ';'
           | incdec ';'
           | affectation ';'
           | addAffect ';'
           | subAffect ';'
           | ret ';' 
           | ifStatement
           | whileStatement
           | standardFct ';'      
;

blocInstr: instruction *
;

declare: type listvar               #listVarDec
        | type VAR '[' CONST ']'    #arrayDec
;

listvar: VAR (',' VAR)*
;

incdec: incdecop VAR
;

affectation: type VAR '=' expr           #affDecVar
           | VAR '=' expr                #affVar
           | VAR '[' CONST ']' '=' CONST #affArrayConst
           | VAR '[' CONST ']' '=' VAR   #affArrayVar
           | VAR '[' CONST ']' '=' expr   #affArrayExpr
;

addAffect: VAR '+=' expr
;

subAffect: VAR '-=' expr
;


usedvar: VAR                    
;                     

expr: CONST                      #constExpr
    | usedvar                    #varExpr
    | '(' expr ')'               #parExpr
    | unaryop expr               #unaryExpr
    | incdec                     #incdecExpr
    | expr multdivop expr        #multDivExpr
    | expr addsubop expr         #addSubExpr   
    | expr COMPAREOP expr        #compareExpr
    | expr EQUALOP expr          #equalExpr
    | expr '&' expr              #andExpr
    | expr '^' expr              #xorExpr
    | expr '|' expr              #orExpr
    | ABCD                       #alphabets
;

ret: RET VAR                     #retVar
   | RET expr                    #retExpr
   | RET                         #retNothing
;

RET: 'return' 
;

unaryop:  '-' | '!'
;

addsubop: '-' | '+'
;

multdivop: '*' | '/' | '%'
;

incdecop: '++' | '--'
;

COMPAREOP: '>'
| '<'
;

EQUALOP: '=='
| '!='
;

ifStatement: 
        'if' '(' expr ')' '{' blocInstr '}' (elseStatement)?
        | 'if' '(' expr ')' instruction (elseStatement)? 
;

elseStatement:
        'else' '{' blocInstr '}'  
        |'else'  instruction                             
;

whileStatement :
	'while' '(' expr ')' '{' blocInstr '}'
        | 'while' '(' expr ')' instruction
;

standardFct:
        'putchar' '(' ABCD ')'                          #putcharChar
        |'putchar' '(' VAR ')'                          #putcharVar
        |VAR '=' 'getchar' '(' ')'                      #getchar
;

INT : 'int' ;
CHAR : 'char' ;
VAR : [a-zA-Z][a-zA-Z0-9]* ;
CONST : [0-9]+ ;
CH : [0-9a-zA-Z] ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ABCD :  '\''[0-9a-zA-Z]'\'';