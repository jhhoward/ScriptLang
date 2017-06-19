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

/*void Parser::Term()
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

void Parser::Comparison()
{
	Expression();

	switch (token.type)
	{
	case TokenType::Equals:
		NextToken();
		Comparison();

		printf("CMP\n");
		break;
	}
}*/

void Parser::Expression(TokenType level)
{
	Factor();

	while (token.type <= level)
	{
		switch (token.type)
		{
		case TokenType::Add:
			NextToken();
			Expression(TokenType::Add);

			printf("ADD\n");
			// TODO emit +
			break;
		case TokenType::Multiply:
			NextToken();
			Expression(TokenType::Multiply);

			printf("MUL\n");
			// TODO emit
			break;
		case TokenType::Divide:
			NextToken();
			Expression(TokenType::Multiply);

			printf("DIV\n");
			// TODO emit
			break;
		case TokenType::EqualTo:
			NextToken();
			Expression(TokenType::EqualTo);

			printf("EQ\n");
			break;
		default:
			return;
		}
	}
}

/*
void Parser::Expression(int precedence)
{
	if (precedence > 1)
	{
		Expression(precedence - 1);
	}
	else
	{
		Factor();
	}

	switch (precedence)
	{
		case 3:
		{
			switch (token.type)
			{
			case TokenType::Multiply:
				NextToken();
				Expression(precedence);

				printf("MUL\n");
				// TODO emit
				break;
			}
		}
		break;

		case 4:
		{
			switch (token.type)
			{
			case TokenType::Add:
				NextToken();
				Expression(precedence);

				printf("ADD\n");
				// TODO emit +
				break;
			}
		}
		break;

		case 7:
		{
			switch (token.type)
			{
			case TokenType::EqualTo:
				NextToken();
				Expression(precedence);

				printf("CMP\n");
				break;
			}
		}
		break;
	}
}
*/


void Parser::Assignment()
{
	Variable* targetVariable = FindVariable(token.text);

	if (!targetVariable)
	{
		ThrowParseException(string("Undefined identifier : ") + token.text);
	}

	switch (NextToken().type)
	{
	case TokenType::AddAssign:
	{
		if (!targetVariable->isAssigned)
		{
			ThrowParseException(string("Use of unassigned variable ") + token.text);
		}
		printf("PUSH %s\n", targetVariable->name.c_str());
		NextToken();
		Expression();
		printf("ADD\n");
		printf("ST %s\n", targetVariable->name.c_str());
	}
	break;
	case TokenType::Assign:
		{
			NextToken();
			//Comparison();
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
			//Comparison();
			Expression();

			// TODO: emit assign
			printf("ST %s\n", newVar->name.c_str());
			newVar->isAssigned = true;

			declaringList = true;
			break;
		}
	}
}

void Parser::IfBlock()
{
	NextToken();
	Expect(TokenType::OpenBracket);
	Expression();
	// TODO: ensure boolean condition
	Expect(TokenType::CloseBracket);
}

// Function call
// Declaration
// Assignment
// { statements }
void Parser::Statement()
{
	{
		int lineNumber;
		string line;
		lexer.GetCurrentLineAndNumber(line, lineNumber);
		printf("# %s\n", line.c_str());
	}

	switch (token.type)
	{
	case TokenType::If:
		IfBlock();
		break;
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

	printf("DECL %s\n", name.c_str());

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
