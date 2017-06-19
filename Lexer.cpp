#include <sstream>
#include "Lexer.h"

using namespace std;

static const char* baseTokens = "(){}+-/*;=,";

static const char* keywords[] =
{
	"if",
	"else",
	"while",
	"class",
	"int",
	"float",
	"void",
	"for",
	"return",
};


Lexer::Lexer(const char* inputString) :
	wholeInput(inputString),
	ptr(inputString)
{
	 
}

void Lexer::GetCurrentLineAndNumber(string& outLine, int& outLineNumber)
{
	outLineNumber = 1;
	const char* lineStart = wholeInput;
	const char* p;

	for (p = wholeInput; p < ptr; p++)
	{
		if ((*p) == '\n')
		{
			outLineNumber++;
			lineStart = p + 1;
		}
	}

	while (*p && *p != '\n')
	{
		p++;
	}

	outLine = string(lineStart, (p - lineStart));
}

Token Lexer::GetNext()
{
	Token token;

	char readChar;

	if (*ptr == '\0')
	{
		token.type = TokenType::EndOfFile;
		return token;
	}
	
	readChar = *ptr++;

	while (IsWhiteSpace(readChar))
	{
		readChar = *ptr++;
	}

	if (readChar == '=')
	{
		if (ptr[1] == '=')
		{
			token.type = TokenType::Equals;
			token.text = "==";
			return token;
		}
	}

	if (readChar == '/')
	{
		if (*ptr == '/')
		{
			while (!IsEOF() && !IsNewLine(*ptr))
			{
				ptr++;
			}
			while (IsWhiteSpace(readChar))
			{
				readChar = *ptr++;
			}
		}
		else if (*ptr == '*')
		{
			while(!IsEOF())
			{
				ptr++;
				if (*ptr == '*' && ptr[1] == '/')
				{
					ptr = ptr + 2;
					break;
				}
			}
			do 
			{
				readChar = *ptr++;
			} while (IsWhiteSpace(readChar));
		}
	}

	for (const char* baseTokenPtr = baseTokens; *baseTokenPtr; baseTokenPtr++)
	{
		if (readChar == *baseTokenPtr)
		{
			token.type = (TokenType) readChar;
			token.text = string(1, readChar);
			return token;
		}
	}

	if (readChar == '\0')
	{
		token.type = TokenType::EndOfFile;
		return token;
	}

	if (readChar >= '0' && readChar <= '9')
	{
		int value = readChar - '0';
		while (*ptr >= '0' && *ptr <= '9')
		{
			value = value * 10 + (*ptr++) - '0';
		}

		// TODO: Error if not followed by operator or whitespace

		token.type = TokenType::IntegerConstant;
		token.intValue = value;
		token.text = to_string(value);
		return token;
	}

	if ((readChar >= 'a' && readChar <= 'z') || (readChar >= 'A' && readChar <= 'Z') || readChar == '_')
	{
		ostringstream oss;
		oss << readChar;

		while ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || *ptr == '_' || (*ptr >= '0' && *ptr <= '9'))
		{
			oss << *ptr++;
		}

		token.type = TokenType::Identifier;
		token.text = oss.str();

		for (int k = (int) TokenType::FirstKeyword; k <= (int) TokenType::LastKeyword; k++)
		{
			if (token.text == string(keywords[k - (int) TokenType::FirstKeyword]))
			{
				token.type = (TokenType) k;
				break;
			}
		}

		return token;
	}

	return token;
}

Token Lexer::PeekNext()
{
	const char* oldPtr = ptr;

	Token token = GetNext();

	ptr = oldPtr;

	return token;
}

bool Lexer::IsEOF()
{
	return *ptr == '\0';
}
