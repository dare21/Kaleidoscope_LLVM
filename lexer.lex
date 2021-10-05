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

if         return if_token;

then       return then_token;

else       return else_token;

for        return for_token;

in         return in_token;

var        return var_token;

"=="       return eq_token; 

[#].*      {   }

[a-zA-Z_][a-zA-Z_0-9]* {
  yylval.s = new string(yytext);
  return id_token;
}

[0-9]+(\.[0-9]*)? {
  yylval.d = atof(yytext);
  return num_token;
}

[-+*/;(),<>=!|:]         return *yytext;

[ \n\t]            {  }

.   {
  cerr << "Lexical error: unknown character '" << *yytext << "'" << endl;
  exit(EXIT_FAILURE);
}
%%
