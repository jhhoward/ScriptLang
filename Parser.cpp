#include <sstream>
#include "Parser.h"

using namespace std;

Parser::Parser(const char* input) :
	lexer(input)
{
	currentScope = &globalScope;
}

void Parser::UnexpectedTokenError()
{
	ostringstream oss;

	oss << "Unexpected token : " << token.GetTypeString();

	ThrowParseException(oss.str());
}

void Parser::Expect(TokenType tokenType)
{
	if (token.type != tokenType)
	{
		UnexpectedTokenError();
	}
	NextToken();
}

void Parser::Factor()
{
	switch (token.type)
	{
	case TokenType::OpenBracket:
		NextToken();
		Expression();
		Expect(TokenType::CloseBracket);
		break;
	case TokenType::Identifier:
		// TODO: check valid identifier
		// TODO: Emit code
		{
			Variable* srcVar = FindVariable(token.text);

			if(!srcVar)
			{ 
				ThrowParseException(string("Undeclared identifier ") + token.text);
			}

			if (!srcVar->isAssigned)
			{
				ThrowParseException(string("Use of unassigned variable ") + token.text);
			}
			printf("PUSH %s\n", token.text.c_str());
			NextToken();
		}
		break;
	case TokenType::IntegerConstant:
		// TODO: emit code
		printf("PUSH %d\n", token.intValue);
		NextToken();
		break;
	default:
		break;
	}
}

void Parser::Term()
{
	Factor();

	switch (token.type)
	{
	case TokenType::Multiply:
		NextToken();
		Term();

		printf("MUL\n");
		// TODO emit
		break;
	default:
		break;
	}
}

void Parser::Expression()
{
	Term();

	switch (token.type)
	{
	case TokenType::Add:
		NextToken();
		Expression();

		printf("ADD\n");
		// TODO emit +
		break;
	default:
		break;
	}
}

void Parser::Assignment()
{
	Variable* targetVariable = FindVariable(token.text);

	if (!targetVariable)
	{
		ThrowParseException(string("Undefined identifier : ") + token.text);
	}

	switch (NextToken().type)
	{
	case TokenType::Assign:
		{
			NextToken();
			Expression();

			printf("ST %s\n", targetVariable->name.c_str());
			targetVariable->isAssigned = true;
		}
		break;
	default:
		UnexpectedTokenError();
		break;
	}

}

void Parser::DeclarePrime(VariableType variableType)
{
	
}

void Parser::Declare(VariableType variableType)
{
	if (NextToken().type != TokenType::Identifier)
	{
		ThrowParseException("Expected [identifier]");
	}

	string declarationName = token.text;
	Variable* newVar = DeclareVariable(variableType, declarationName);
	NextToken();

	bool declaringList = true;

	while (declaringList)
	{
		declaringList = false;

		switch (token.type)
		{
		case TokenType::Comma:
			Declare(variableType);
			break;
		case TokenType::Assign:
			NextToken();
			Expression();

			// TODO: emit assign
			printf("ST %s\n", newVar->name.c_str());
			newVar->isAssigned = true;

			declaringList = true;
			break;
		}
	}
}

// Function call
// Declaration
// Assignment
// { statements }
void Parser::Statement()
{
	switch (token.type)
	{
	case TokenType::Int:
		Declare(VariableType::Integer);
		break;
	case TokenType::Identifier:
		Assignment();
		break;
	default:
		UnexpectedTokenError();
	}

	Expect(TokenType::Terminator);
}

void Parser::Statements()
{
	while (token.type != TokenType::EndOfFile)
	{
		Statement();

		if (token.type != TokenType::Terminator)
		{
			break;
		}
		NextToken();
	}
}

void Parser::Parse()
{
	NextToken();

	while (!lexer.IsEOF())
	{
		Statements();
	}
}

Token Parser::NextToken()
{
	return (token = lexer.GetNext());
}

Variable* Parser::DeclareVariable(VariableType variableType, std::string name)
{
	if (currentScope->symbolTable.find(name) != currentScope->symbolTable.end())
	{
		ThrowParseException(name + string(" previously declared"));
	}

	Variable* newVar = new Variable(name, variableType);
	currentScope->symbolTable[name] = newVar;
	return newVar;
}

void Parser::PushScope()
{
	Scope* newScope = new Scope();
	newScope->parentScope = currentScope;
	currentScope = newScope;
}

void Parser::ThrowParseException(std::string reason)
{
	string line;
	int lineNumber;
	lexer.GetCurrentLineAndNumber(line, lineNumber);

	throw ParseException(reason, line, lineNumber);
}

void Parser::PopScope()
{
	if (currentScope->parentScope)
	{
		Scope* newScope = currentScope->parentScope;
		delete currentScope;
		currentScope = newScope;
	}
	else
	{
		ThrowParseException("Could not pop scope");
	}
}

Variable* Parser::FindVariable(std::string name)
{
	Scope* scope = currentScope;

	while (scope)
	{
		auto it = scope->symbolTable.find(name);

		if (it != scope->symbolTable.end())
		{
			if (it->second->GetType() == SymbolType::Variable)
			{
				return (Variable*)(it->second);
			}
		}

		scope = scope->parentScope;
	}

	return nullptr;
}
