#include "Lexer.h"

#include <string>

Lexer::Lexer() {}

bool Lexer::lex(const char* data, size_t size, std::function<void(const TokenRange&)> lambda) {
	bool line_first = true;
	bool skip_to_new_line = false;
	bool skip_to_end_of_comment_block = false;

	char c;
	char prevc = '\0';
	
	for (size_t i = 0; i < size; ++i, prevc = c) {
		size_t rem = size - i;
		c = data[i];
		
		if (skip_to_end_of_comment_block) {
			if (c != '*') {
				continue;
			}
			if (rem >= 1 && data[i + 1] == '/') {
				++i;
				skip_to_end_of_comment_block = false;
			}
			continue;
		}

		if (c == ' ' || c == '\t' || c == '\0') {
			// whitespace
			continue;
		}
		
		if (c == '\r' || c == '\n') {
			// newline
			if (prevc != '\\') {
				skip_to_new_line = false;
				line_first = true;
			}
			if (c == '\r' && rem >= 2 && data[i + 1] == '\n') {
				// cr+lf, skip an extra character
				++i;
			}
			continue;
		}
		
		if (skip_to_new_line) {
			continue;
		}

		if (c == '/' && rem >= 2) {
			if (data[i + 1] == '/') {
				// single line comment, advance to next line
				skip_to_new_line = true;
				continue;
			} else if (data[i + 1] == '*') {
				// multi line comment, advance to next "*/", skipping the next asterisk
				++i;
				skip_to_end_of_comment_block = true;
				continue;
			}
		}

		// read token
		TokenRange token = _read_token(data, size, i);
		token.flags |= (line_first ? TokenFlagLineFirst : 0);
		line_first = false;

		lambda(token);
		
		i += (token.length - 1);
	}
	
	return true;
}

TokenRange Lexer::_read_token(const char* data, size_t size, size_t pos) {
	char c = data[pos];
	size_t rem = size - pos;

	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
		// identifier
		size_t len = 1;
		for (; rem >= len; ++len) {
			char c2 = data[pos + len];
			if (!(c2 >= 'A' && c2 <= 'Z') && !(c2 >= 'a' && c2 <= 'z') && !(c2 >= '0' && c2 <= '9') && c2 != '_') {
				break;
			}
		}
		return TokenRange(TokenTypeIdentifier, pos, len);
	}
	
	if ((c >= '0' && c <= '9') || (c == '.' && rem >= 1 && data[pos + 1] >= '0' && data[pos + 1] <= '9')) {
		// number
		size_t len = 1;
		for (char c2, prevc2 = c; rem >= len; ++len, prevc2 = c2) {
			c2 = data[pos + len];
			if ((c2 >= 'a' && c2 <= 'z') || (c2 >= 'A' && c2 <= 'Z') || (c2 >= '0' && c2 <= '9') || c2 == '_' || c2 == '.') {
				// letter, digit, underscore, or period
				continue;
			}
			if ((c2 == '+' || c2 == '-') && (prevc2 == 'e' || prevc2 == 'E' || prevc2 == 'p' || prevc2 == 'P')) {
				// exponent
				continue;
			}
			break;
		}
		return TokenRange(TokenTypeNumber, pos, len);
	}
	
	if (c == '\'' || c == '"') {
		// string literal or character constant
		size_t len = 1;
		bool escape = false;
		for (; ; ++len) {
			if (len > rem) {
				// unterminated literal!
				break;
			}
			
			char c2 = data[pos + len];
			
			if (escape) {
				escape = false;
				continue;
			}

			if (c2 == '\\') {
				escape = true;
			}

			if (c2 == c) {
				++len;
				break;
			}
		}
		return TokenRange(c == '\'' ? TokenTypeCharacterConstant : TokenTypeStringLiteral, pos, len);
	}

	if ((unsigned char)c >= 0x7F || c < 0x20 || c == '$' || c == '@' || c == '`') {
		// other
		return TokenRange(TokenTypeOther, pos, 1);
	}			

	// punctuator
	
	// multicharacter punctuators
	const static char* punctuators[] = { 
		"%:%:", 
		"...", "<<=", ">>=", "->*",
		"->", "++", "--", "##", "<<", ">>", "!=", "<=", 
		">=", "==", "&&", "||", "*=", "/=", "%=", "+=", 
		"-=", "&=", "^=", "|=", ".*", "<%", "%>", "<:",
		":>", "%:", "::"
	};

	size_t punctuator_length = 1;
	
	for (size_t i = 0; i < sizeof(punctuators) / sizeof(char*); ++i) {
		size_t len = strlen(punctuators[i]);
		if (rem >= len && !memcmp(data + pos, punctuators[i], len)) {
			punctuator_length = len;
			break;
		}
	}

	return TokenRange(TokenTypePunctuator, pos, punctuator_length);
}
