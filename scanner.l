/******************************************************************************
 * Preston Fraser, pfraser
 * scanner.l
 *
 * This file is used to generate yylex.cpp.
 *****************************************************************************/

%{

#include "lyutils.h"

#define YY_USER_ACTION  { lexer::advance(); }

#define VAL_TOKEN(SYMBOL) { \
   yylval = new astree (SYMBOL, lexer::lloc, yytext); \
   fprintf(stdout, "before dump_node()\n"); \
   yylval->dump_node(out); \
   fprintf(stdout, "after dump_node()\n"); \
   return SYMBOL; \
}

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
CHARCON         ('([^\\'\n]|\\[\\'"0nt])')
STRINGCON       (\"([^\\"\n]|\\[\\"0nt])*\")
CHARUNCLOSED    ('[^' \t\n]*)
STRINGUNCLOSED  (\"[^\" \t\n]*)
CHARBADESC      ('([^\\'\n]|\\[^\\'"0nt])')
STRINGBADESC    (\"([^\\"\n]|\\[^\\"0nt])*\")

NOTNUMBER       ({MANTISSA}[Ee][+-]?)
NOTIDENT        ({DIGIT}({LETTER}|{DIGIT})*)
NOTCHAR         ({CHARUNCLOSED}|{CHARBADESC})
NOTSTRING       ({STRINGUNCLOSED}|{STRINGBADESC})

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"if"            { VAL_TOKEN (TOK_IF); }
"else"          { VAL_TOKEN (TOK_ELSE); }
"while"         { VAL_TOKEN (TOK_WHILE); }
"return"        { VAL_TOKEN (TOK_RETURN); }
"int"           { VAL_TOKEN (TOK_INT); }
"char"          { VAL_TOKEN (TOK_CHAR)}
"string"        { VAL_TOKEN (TOK_STRING); }
"struct"        { VAL_TOKEN (TOK_STRUCT); }
"void"          { VAL_TOKEN (TOK_VOID); }
"new"           { VAL_TOKEN (TOK_NEW); }
"null"          { VAL_TOKEN (TOK_NULL); }
"not"           { VAL_TOKEN (TOK_NOT); }

{NUMBER}        { VAL_TOKEN (TOK_INTCON); }
{IDENT}         { VAL_TOKEN (TOK_IDENT); }
{CHARCON}       { VAL_TOKEN (TOK_CHARCON); }
{STRINGCON}     { VAL_TOKEN (TOK_STRINGCON); }
"+"             { VAL_TOKEN ('+'); }
"-"             { VAL_TOKEN ('-'); }
"*"             { VAL_TOKEN ('*'); }
"/"             { VAL_TOKEN ('/'); }
"%"             { VAL_TOKEN ('%'); }
"!"             { VAL_TOKEN ('!'); }
"="             { VAL_TOKEN ('='); }
"=="			{ VAL_TOKEN (TOK_EQ); }
"!="            { VAL_TOKEN (TOK_NE); }
"<"             { VAL_TOKEN (TOK_LT); }
"<="            { VAL_TOKEN (TOK_LE); }
">"             { VAL_TOKEN (TOK_GT); }
">="            { VAL_TOKEN (TOK_GE); }
"->"            { VAL_TOKEN (TOK_ARROW); }
","             { VAL_TOKEN (','); }
"."             { VAL_TOKEN ('.'); }
";"             { VAL_TOKEN (';'); }
"("             { VAL_TOKEN ('('); }
")"             { VAL_TOKEN (')'); }
"["             { VAL_TOKEN ('['); }
"]"             { VAL_TOKEN (']'); }
"{"             { VAL_TOKEN ('{'); }
"}"             { VAL_TOKEN ('}'); }
"[]"            { VAL_TOKEN (TOK_ARRAY); }

{NOTNUMBER}     { lexer::badtoken (yytext);
                  VAL_TOKEN (TOK_INTCON); }
{NOTIDENT}      { lexer::badtoken (yytext);
                  VAL_TOKEN (TOK_IDENT); }
{NOTCHAR}       { lexer::badtoken (yytext);
                  VAL_TOKEN (TOK_CHARCON); }
{NOTSTRING}     { lexer::badtoken (yytext);
                  VAL_TOKEN (TOK_STRINGCON); }
.               { lexer::badchar (*yytext); }

%%