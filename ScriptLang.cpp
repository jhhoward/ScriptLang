#include <stdio.h>
#include "Lexer.h"
#include "Parser.h"

char* testInput =
"int x = 3, y, z = 100; \n"
"y = x * 2 + z; \n";

char* testInput3 =
	"int y;"
	"int x = (1 + 2) * 3;"
	"y = x * 2;";

char* testInput2 =
	"int /* a comment */testFunc(int a, int b){\n"
	"\treturn (a + b) / 2;\n"
	"}\n";

int main(int argc, char* argv[])
{
	Lexer lex(testInput);

	printf("Input:\n%s\n", testInput);

	while (!lex.IsEOF())
	{
		Token tok = lex.GetNext();
	//	printf("Token: %s ('%s')\n", tok.GetTypeString().c_str(), tok.text.c_str());
	}

	Parser parser(testInput);

	try
	{
		parser.Parse();
	}
	catch (ParseException ex)
	{
		printf("Error at line %d : %s\n%s\n", ex.lineNumber, ex.reason.c_str(), ex.line.c_str());
	}

    return 0;
}

