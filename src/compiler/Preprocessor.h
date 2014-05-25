#pragma once

#include "Token.h"
#include "LexedFile.h"

class Preprocessor {
	public:

		Preprocessor();

		bool process_file(const char* filename);
		
		const std::list<TokenPtr>& tokens();

	private:
		template <class T>
		bool _process_tokens(T& tokens);
		
		char _escape_character(char c);
		void _read_string_value(TokenPtr token, std::string& value);
	
		std::list<TokenPtr> _tokens;
};

class PPModifiedToken : public Token {
	public:
		PPModifiedToken(TokenPtr base, const std::string& value) : _base(base), _value(value) {}
	
		virtual TokenType type() { return _base->type(); };
		virtual uint32_t flags() { return _base->flags(); };
		virtual const std::string value() { return _value; };
		virtual void print_pointer() { return _base->print_pointer(); }

		virtual ~PPModifiedToken() {}
	private:
		TokenPtr _base;
		std::string _value;
};
