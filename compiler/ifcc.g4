grammar ifcc;

axiom : prog ;

prog : 'int' 'main' '(' ')' '{' instruction* '}' ;

type : INT | CHAR | ;

instruction : declare ';'
            | affectation ';'
            | ret ';'            
            ;

declare: type listvar 
       ;

listvar: VAR (',' VAR)*
        ;

affectation: type VAR '=' expr
        ;


expr : CONST                      #constExpr
     | VAR                        #varExpr
     | expr '&' expr              #andExpr
     | expr '|' expr              #orExpr
     | expr '^' expr              #xorExpr
     | expr '>' expr              #sup
     | expr '<' expr              #min
     | expr '!=' expr             #diff
     | expr '==' expr             #equal
     | '-' expr                   #negExpr
     | '!' expr                   #notExpr
     | expr '+' expr              #plusExpr
     | expr '-' expr              #minusExpr
     | expr '*' expr              #multExpr
     ;

ret : RET expr
    | RET 
    ;

RET: 'return' ;

INT : 'int' ;
CHAR : 'char' ;
VAR : [a-zA-Z][a-zA-Z0-9]* ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
