#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
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

using namespace std;

int main(int argc, char* argv[])
{
	ifstream inputFileStream("test.script");
	stringstream inputBuffer;
	inputBuffer << inputFileStream.rdbuf();
	string inputData = inputBuffer.str();

	cout << inputData << endl;

	Lexer lex(inputData.c_str());

	while (!lex.IsEOF())
	{
		Token tok = lex.GetNext();
		cout << "Token: " << tok.GetTypeString() << " ('" << tok.text << "')" << endl;
	}

	Parser parser(inputData.c_str());

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

