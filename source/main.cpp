#include "parser.tab.h"
#include "scanner.h"
#include "ast.h"

extern int ch;
extern int parserror;

void yyerror(char *errmsg)
{
    parserror += 1;
    fprintf(stderr, "%s (%d, %d): %s\n", errmsg, yylineno, ch, yytext);
}

int main(int argc, char **argv)
{
    extern int ch;
    extern int parserror;
    extern Node *root;
    extern const char* token_name(int t);
    int l, flag, res;
    if(argc < 3)
    {
        printf("\nNot enough arguments. Please specify filename and mode\n");
        return -1;
    }
    if((yyin = fopen(argv[1], "r")) == NULL)
    {
        printf("\nCannot open file %s.\n", argv[1]);
        return -1;
    }
    sscanf(argv[2], "%d", &flag);
    ch = 1;
    yylineno = 1;
    if(flag == 1)
        while(l = yylex())
        {
            printf("found token %s (%d, %d): %s\n", token_name(l), yylineno, ch, yytext);
        }
    else{
        yyparse();
        if(parserror == 0)
        {
            tree_print(root, 0);
            rapidjson::Document doc;
            toJSON(doc, root);
            rapidjson::StringBuffer buffer;
            // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);
            const std::string& str = buffer.GetString();
           // std::cout << "Serialized:" << std::endl;
           // std::cout << str << std::endl;
            std::ofstream out;
            out.open("./ast.json");
            if (out.is_open())
            {
                out << str << std::endl;
            }
        }    
    }
    fclose(yyin);
    return 0;
}