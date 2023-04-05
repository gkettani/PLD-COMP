grammar ifcc;

axiom : prog ;

prog : 'int' 'main' '(' ')' '{' instruction* '}' ;

type : INT | CHAR ;

instruction : declare ';'
            | affectation ';'
            | ret ';'            
            ;

declare: type listvar 
       ;

listvar: VAR (',' VAR)*
        ;

affectation: type VAR '=' expr
                | VAR '=' expr
        ;
        
usedvar: VAR                    
        ;                     

expr : CONST                      #constExpr
     | usedvar                    #varExpr
     | '(' expr ')'               #parExpr
     | unaryop expr               #unaryExpr
     | expr '*' expr              #multExpr 
     | expr '/' expr              #divExpr
     | expr '-' expr              #minusExpr
     | expr '+' expr              #plusExpr
     | expr COMPAREOP expr        #compareExpr
     | expr EQUALOP expr          #equalExpr
     | expr '&' expr              #andExpr
     | expr '^' expr              #xorExpr
     | expr '|' expr              #orExpr
     | ABCD                       #alphabets
     ;

ret : RET expr
    | RET
    ;

RET: 'return' ;

unaryop:  '-' | '!';

COMPAREOP: '>'
| '<'
;

EQUALOP: '=='
| '!='
;

INT : 'int' ;
CHAR : 'char' ;
VAR : [a-zA-Z][a-zA-Z0-9]* ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ABCD :  '\''[0-9a-zA-Z]'\'';