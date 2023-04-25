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
           | incrdecr ';'
           | affectation ';'
           | addAffect ';'
           | subAffect ';'
           | ret ';' 
           | ifStatement
           | standardFct ';'      
;

blocInstr: instruction *
;

declare: type listvar 
;

listvar: VAR (',' VAR)*
;

incrdecr: incrdecrop VAR
;

affectation: type VAR '=' expr
           | VAR '=' expr
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
    | incrdecr                   #incrdecrExpr
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
   | RET CONST                   #retConst
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

incrdecrop: '++' | '--'
;

COMPAREOP: '>'
| '<'
;

EQUALOP: '=='
| '!='
;

ifStatement: 
        'if' '(' expr ')' '{' blocInstr '}' (elseStatement)?     
;

elseStatement:
        'else' '{' blocInstr '}'                                
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