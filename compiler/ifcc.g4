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
     | '-' expr                   #negExpr
     | '!' expr                   #notExpr
     | expr '*' expr              #multExpr 
     | expr '/' expr              #divExpr
     | expr '-' expr              #minusExpr
     | expr '+' expr              #plusExpr
     | expr COMPAREOP expr        #compareExpr
     | expr EQUALOP expr          #equalExpr
     | expr '&' expr              #andExpr
     | expr '^' expr              #xorExpr
     | expr '|' expr              #orExpr
     ;

ret : RET expr
    | RET
    ;

RET: 'return' ;

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
