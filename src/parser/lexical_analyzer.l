%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int files_count;
int lines;
int pos_start;
int pos_end;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}
%}

/***************
 TO STUDENTS: Copy your Lab1 here. Make adjustments if necessary.

 Note: don't modify the prologue unless you know what you are doing.
***************/

/* Example for you :-) */

%%
\n {lines+=1;pos_end=1;}
\t {pos_start=pos_end;pos_end=pos_start+1;}
" " {pos_start=pos_end;pos_end=pos_start+1;}
\r {pos_start=pos_end;pos_end=pos_start+1;}
\s {pos_start=pos_end;pos_end=pos_start+1;}
else {pos_start=pos_end;pos_end=pos_start+4;pass_node(yytext);return ELSE;}
if {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return IF;}
int {pos_start=pos_end;pos_end=pos_start+3;pass_node(yytext);return INT;}
return {pos_start=pos_end;pos_end=pos_start+6;pass_node(yytext);return RETURN;}
void {pos_start=pos_end;pos_end=pos_start+4;pass_node(yytext);return VOID;}
while {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return WHILE;}
float {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return FLOAT;}
"[]" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return ARRAY;}
\+ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return ADD;}
\- {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return SUB;}
\* {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return MUL;}
\/ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return DIV;}
\< {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LT;}
"<=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return LTE;}
\> {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return GT;}
">=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return GTE;}
"==" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return EQ;}
"!=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return NEQ;}
\= {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return ASSIN;}
\; {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return SEMICOLON;}
\, {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return COMMA;}
\( {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LPARENTHESE;}
\) {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RPARENTHESE;}
\[ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LBRACKET;}
\] {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RBRACKET;}
\{ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LBRACE;}
\} {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RBRACE;}
[a-zA-Z]+ {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return IDENTIFIER;}
[0-9]+ {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return INTEGER;}
[0-9]*\.[0-9]+ {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return FLOATPOINT;}
[0-9]+\. {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return FLOATPOINT;}
\/\*\/*([^\/]*[^\/\*]\/[\/]*)*[^\/]*\*\/ {
				pos_start=pos_end;
                pos_end=pos_start+2;
                int i=2;
      			while(yytext[i]!='*' || yytext[i+1]!='/')
      			{  			
      				if(yytext[i]=='\n')
      				{
						lines=lines+1;
						pos_end=1;
					}
					else
						pos_end=pos_end+1;
					i=i+1;
				}
				pos_end=pos_end+2;
				}
. {pos_start=pos_end;pos_end=pos_start+strlen(yytext);return ERROR;}

%%
