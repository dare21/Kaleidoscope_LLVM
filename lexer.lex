%option noyywrap
%option noinput
%option nounput


%{

  #include <iostream>
  #include <cstdlib>
  using namespace std;
  #include <string>
  #include <vector>

  #include "ast.hpp"
  #include "parser.tab.hpp"
  
%}

%%
def        return def_token;
extern     return extern_token;
[a-zA-Z_][a-zA-Z_0-9]* {
  yylval.s = new string(yytext);
  return id_token;
}
[0-9]+(\.[0-9]*)? {
  yylval.d = atof(yytext);
  return num_token;
}
[-+*/;(),]         return *yytext;
[ \n\t]            {  }
.   {
  cerr << "Lexic error: unknown character '" << *yytext << "'" << endl;
  exit(EXIT_FAILURE);
}
%%
