#pragma once

#include "Token.h"
#include "Lexer.h"

#include <string>
#include <list>

class LexedFileToken;

class LexedFile {
	public:
		LexedFile(const char* filename);
		~LexedFile();
		
		bool lex(std::shared_ptr<LexedFile> self);

		const std::list<std::shared_ptr<LexedFileToken>>& tokens();

		const char* contents();
		size_t size();

		const std::string filename();

	private:
		LexedFile();
		LexedFile operator=(const LexedFile& other);

		std::string _filename;

		char* _contents = nullptr;
		size_t _size = 0;

		bool _is_lexed = false;

		std::list<std::shared_ptr<LexedFileToken>> _tokens;
};

class LexedFileToken : public Token {
	public:
		LexedFileToken(TokenType type, size_t location, size_t length, std::shared_ptr<LexedFile> file);
		LexedFileToken(TokenType type, uint32_t flags, size_t location, size_t length, std::shared_ptr<LexedFile> file);

		virtual uint32_t flags();
		virtual TokenType type();
		virtual const std::string value();
		virtual void print_pointer();

	private:
		TokenType _type;
		uint32_t _flags;
		size_t _location;
		size_t _length;
		std::shared_ptr<LexedFile> _file;
};
