#pragma once

#include "Token.h"

#include <string>
#include <list>
#include <functional>

struct TokenRange {
	TokenRange(TokenType type, size_t location, size_t length) : type(type), flags(0), location(location), length(length) {}

	TokenType type;
	uint32_t flags;
	size_t location;
	size_t length;
};

class Lexer {
	public:
	
		Lexer();
		
		bool lex(const char* data, size_t size, std::function<void(const TokenRange&)> lambda);
		
	private:
	
		TokenRange _read_token(const char* data, size_t size, size_t pos);
};
