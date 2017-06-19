#include <sstream>
#include "Lexer.h"

using namespace std;

struct TokenMapping 
{
	const char* pattern;
	const char* displayName;
};

static TokenMapping tokenPatterns[] =
{
	{ ";", "Terminator" },
	{ "", "Identifier" },
	{ "", "IntegerConstant" },
	{ "", "FloatConstant" },
	{ "(", "OpenBracket" },
	{ ")", "CloseBracket" },
	{ "*", "Multiply" },
	{ "/", "Divide" },
	{ "%", "Modulo" },
	{ "+", "Add" },
	{ "-", "Subtract" },
	{ "<<", "LeftShift" },
	{ ">>", "RightShift" },
	{ "<", "LessThan" },
	{ "<=", "LessThanOrEqualTo" },
	{ ">", "GreaterThan" },
	{ ">=", "GreaterThanOrEqualTo" },
	{ "==", "EqualTo" },
	{ "!=", "NotEqual" },
	{ "&", "BitwiseAnd" },
	{ "^", "BitwiseXor" },
	{ "|", "BitwiseOr" },
	{ "&&", "LogicalAnd" },
	{ "||", "LogicalOr" },
	{ "=", "Assign" },
	{ "+=", "AddAssign" },
	{ "-=", "SubtractAssign" },
	{ "*=", "MultiplyAssign" },
	{ "/=", "DivideAssign" },
	{ "%=", "ModuloAssign" },
	{ "<<=", "LeftShiftAssign" },
	{ ">>=", "RightShiftAssign" },
	{ "&=", "AndAssign" },
	{ "^=", "XorAssign" },
	{ "|=", "OrAssign" },
	{ ",", "Comma" },
	{ "if", "If" },
	{ "else", "Else" },
	{ "while", "While" },
	{ "class", "Class" },
	{ "int", "Int" },
	{ "float", "Float" },
	{ "void", "Void" },
	{ "for", "For" },
	{ "return", "Return" },
	{ "error", "Error" },
	{ "EOF", "EndOfFile" },
};

const std::string Token::GetTypeString()
{
	return tokenPatterns[(int) type].displayName;
}

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

	if (readChar == '/')
	{
		if (*ptr == '/')
		{
			while (!IsEOF() && !IsNewLine(readChar))
			{
				readChar = *ptr++;
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
					readChar = *ptr;
					break;
				}
			}
			do 
			{
				readChar = *ptr++;
			} while (IsWhiteSpace(readChar));
		}
	}

	{
		int longestMatch = 0;

		for (int n = 0; n < (int)TokenType::FirstKeyword; n++)
		{
			if (tokenPatterns[n].pattern[0] != '\0')
			{
				const char* patternPtr = tokenPatterns[n].pattern;
				const char* comparePtr = ptr - 1;
				bool match = true;
				int matchLength = 0;

				while (match && *patternPtr && *comparePtr)
				{
					if (*patternPtr != *comparePtr)
					{
						match = false;
						break;
					}
					patternPtr++;
					comparePtr++;
					matchLength++;
				}
				match &= (*patternPtr == '\0');

				if (match && matchLength > longestMatch)
				{
					longestMatch = matchLength;
					token.type = (TokenType)(n);
					token.text = tokenPatterns[n].pattern;
				}
			}
		}

		if (longestMatch > 0)
		{
			ptr += longestMatch - 1;
			return token;
		}
	}

	if (readChar == '\0')
	{
		--ptr;
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
			if (token.text == string(tokenPatterns[k].pattern))
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
