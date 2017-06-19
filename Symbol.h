#pragma once

#include <string>

enum class SymbolType
{
	Variable,
	Function,
	Struct
};

class Symbol
{
public:
	Symbol(std::string inName) : name(inName)
	{
	}

	virtual SymbolType GetType() = 0;

	std::string name;
};

enum class VariableType
{
	Integer,
	Float,
	String,
	Object
};

class Variable : public Symbol
{
public:
	Variable(std::string inName, VariableType inType) :
		Symbol(inName), variableType(inType), isAssigned(false)
	{}

	virtual SymbolType GetType() override { return SymbolType::Variable; }

	VariableType variableType;
	bool isAssigned;
};
