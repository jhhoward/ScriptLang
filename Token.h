#pragma once

#include <string>

enum class TokenType
{
	Invalid = -1,
	Terminator = 0,
	Identifier,
	IntegerConstant,
	FloatConstant,
	OpenBracket,
	CloseBracket,
	Multiply,
	Divide,
	Modulo,
	Add,
	Subtract,
	LeftShift,
	RightShift,
	LessThan,
	LessThanOrEqualTo,
	GreaterThan,
	GreaterThanOrEqualTo,
	EqualTo,
	NotEqual,
	BitwiseAnd,
	BitwiseXor,
	BitwiseOr,
	LogicalAnd,
	LogicalOr,
	Assign,
	AddAssign,
	SubtractAssign,
	MultiplyAssign,
	DivideAssign,
	ModuloAssign,
	LeftShiftAssign,
	RightShiftAssign,
	AndAssign,
	XorAssign,
	OrAssign,
	Comma,

	If,
	Else,
	While,
	Class,
	Int,
	Float,
	Void,
	For,
	Return,

	Error,
	EndOfFile,

	FirstKeyword = If,
	LastKeyword = Return,
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
