#include "Preprocessor.h"

#include <string>
#include <vector>

Preprocessor::Preprocessor() {
}

bool Preprocessor::process_file(const char* filename) {
	auto file = std::make_shared<LexedFile>(filename);
	
	return file->lex(file) && _process_tokens(file->tokens());
}

const std::list<TokenPtr>& Preprocessor::tokens() {
	return _tokens;
}

template <class T>
bool Preprocessor::_process_tokens(T& tokens) {
	for (auto it = tokens.begin(); it != tokens.end();) {
		TokenPtr tok = *it;
		
		if ((tok->flags() & TokenFlagLineFirst) && tok->type() == TokenTypePunctuator && tok->value() == "#") {
			// directive
			std::vector<TokenPtr> directive;

			++it;
			while (it != tokens.end() && !((*it)->flags() & TokenFlagLineFirst)) {
				directive.push_back(*it);
				++it;
			}
		
			if (!directive.size()) {
				printf("Preprocessing error: expected directive\n");
				tok->print_pointer();
				return false;
			}
		
			if (directive[0]->value() == "include") {
				// include directive
				if (directive.size() < 2 || directive[1]->type() != TokenTypeStringLiteral) {
					printf("Preprocessing error: expected string literal after include\n");
					directive[0]->print_pointer();
					return false;
				}
				
				std::string filename = directive[1]->value().substr(1, directive[1]->value().size() - 2);
				std::shared_ptr<LexedFile> file(new LexedFile(filename.c_str()));
				
				if (!file->lex(file) || !_process_tokens(file->tokens())) {
					return false;
				}
			} else {
				printf("Preprocessing error: unknown directive\n");
				directive[0]->print_pointer();
				return false;
			}
		} else if (tok->type() == TokenTypeStringLiteral) {
			// transform / merge string literals
			std::string value = "";
			while (it != tokens.end()) {
				TokenPtr str_tok = *it;
				if (str_tok->type() != TokenTypeStringLiteral) {
					break;
				}
				_read_string_value(str_tok, value);
				++it;
			}
			TokenPtr new_tok(new PPModifiedToken(tok, value));
			_tokens.push_back(new_tok);
		} else if (tok->type() == TokenTypeCharacterConstant) {
			// transform character constants
			std::string value = "";
			_read_string_value(tok, value);
			TokenPtr new_tok(new PPModifiedToken(tok, value));
			_tokens.push_back(new_tok);
			++it;
		} else {			
			_tokens.push_back(tok);
			++it;
		}
	}

	return true;
}

char Preprocessor::_escape_character(char c) {
	switch (c) {
		case 'r':
			return '\r';
		case 'n':
			return '\n';
		case 'v':
			return '\v';
		case 'b':
			return '\b';
		case 'f':
			return '\f';
		case 'a':
			return '\a';
		case 't':
			return '\t';
		case '0':
			return '\0';
		default:
			return c;
	}	
}

void Preprocessor::_read_string_value(TokenPtr token, std::string& value) {
	bool escape = false;
	for (size_t i = 1; i < token->value().size() - 1; ++i) { // skip beginning and end characters
		// TODO: support octal / hex numbers
		char c = token->value()[i];
		if (escape) {
			value += _escape_character(c);
			escape = false;
		} else if (c == '\\') {
			escape = true;
		} else {
			value += c;
		}
	}
}