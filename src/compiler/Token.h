#pragma once

#include <memory>
#include <string>

enum TokenType {
	TokenTypeIdentifier,
	TokenTypeNumber,
	TokenTypeCharacterConstant,
	TokenTypeStringLiteral,
	TokenTypePunctuator,
	TokenTypeOther,
	TokenTypeCount,
};

enum TokenFlag {
	TokenFlagLineFirst = 1,
};

class Token {
	public:
		virtual TokenType type() = 0;
		virtual uint32_t flags() = 0;
		virtual const std::string value() = 0;
		virtual void print_pointer() {}

		virtual ~Token() {}
};

typedef std::shared_ptr<Token> TokenPtr;
