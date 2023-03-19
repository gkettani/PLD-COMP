grammar ifcc;

axiom : prog ;

prog : 'int' 'main' '(' ')' '{' instruction* '}' ;

type : INT | CHAR | ;

instruction : declare ';'
            | affectation ';'
            | comparaison ';'
            | ret ';'
            
            ;

declare: type VAR '=' CONST
       | type VAR
       ;

affectation: type VAR '=' VAR
            | type VAR '=' comparaison
            ;


expr : CONST 
| VAR
;

comparaison: expr '>' expr  #sup
| expr '<' expr             #min
| expr '!=' expr            #diff
| expr '==' expr            #equal
| '(' comparaison ')'       #parComparisons
;

ret : RET VAR
    | RET CONST
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
