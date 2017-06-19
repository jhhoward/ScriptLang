#pragma once

#include <string>

enum class TokenType : unsigned char
{
	Invalid = 0,
	Assign = '=',
	Multiply = '*',
	Divide = '/',
	Add = '+',
	Subtract = '-',
	OpenBracket = '(',
	CloseBracket = ')',
	Comma = ',',
	Terminator = ';',
	Identifier = 128,
	Equals,
	IntegerConstant,
	FloatConstant,
	Error = 254,
	EndOfFile = 255,

	If = 192,
	Else,
	While,
	Class,
	Int,
	Float,
	Void,
	For,
	Return,
	FirstKeyword = If,
	LastKeyword = Return
};

class Token
{
public:
	Token()
	{
		type = TokenType::Invalid;
		intValue = 0;
	}

	TokenType type;

	union
	{
		int intValue;
		float floatValue;
	};

	std::string text;

	const std::string GetTypeString();

} ;
