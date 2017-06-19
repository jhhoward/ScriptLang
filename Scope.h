#pragma once

#include <string>
#include <map>
#include "Symbol.h"

class Scope
{
public:
	std::map<std::string, Symbol*> symbolTable;

	Scope* parentScope;
};
