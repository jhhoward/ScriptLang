#pragma once

#include <string>
#include "Scope.h"
#include "Lexer.h"
#include "Symbol.h"

class Parser
{
public:
	Parser(const char* input);

	void Parse();

private:
	void Statements();
	void Statement();

//	void Expression(int precedence = 12);
	void Expression(TokenType level = TokenType::Comma);

	void Factor();
	//void Term();
	void Assignment();

	void IfBlock();

	//void Comparison();

	void Expect(TokenType tokenType);
	void UnexpectedTokenError();
	void ThrowParseException(std::string reason);

	void Declare(VariableType variableType);
	void DeclarePrime(VariableType variableType);

	Variable* DeclareVariable(VariableType variableType, std::string name);
	Variable* FindVariable(std::string name);

	void PushScope();
	void PopScope();

	Token NextToken();

	Lexer lexer;
	Token token;

	Scope globalScope;
	Scope* currentScope;
};

class ParseException
{
public:
	ParseException(std::string inReason, std::string inLine, int inLineNumber) :
		reason(inReason), line(inLine), lineNumber(inLineNumber)
	{
	}

	std::string reason;
	std::string line;
	int lineNumber;
};


