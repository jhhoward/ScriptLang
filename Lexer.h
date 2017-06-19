#pragma once

#include "Token.h"

class Lexer
{
public:
	Lexer(const char* inputString);

	Token GetNext();
	Token PeekNext();

	bool IsEOF();

	void GetCurrentLineAndNumber(std::string& outLine, int& outLineNumber);

private:
	bool IsWhiteSpace(char input)
	{
		return input == ' ' || input == '\r' || input == '\n' || input == '\t';
	}

	bool IsNewLine(char input)
	{
		return input == '\n' || input == '\r';
	}

	const char* wholeInput;
	const char* ptr;
};
