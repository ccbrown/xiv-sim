#include "LexedFile.h"

#include <string>
#include <cstdlib>

LexedFile::LexedFile() {}
LexedFile LexedFile::operator=(const LexedFile& other) { return *this; }

LexedFile::LexedFile(const char* filename) {
	_filename = filename;
}

LexedFile::~LexedFile() {
	free(_contents);
}

bool LexedFile::lex(std::shared_ptr<LexedFile> self) {
	if (_is_lexed) {
		return false;
	}
	
	FILE* f = fopen(_filename.c_str(), "r");

	if (!f) {
		printf("Unable to open file %s\n", _filename.c_str());
		return false;
	}

	fseek(f, 0, SEEK_END);
	_size = ftell(f);
	rewind(f);
 
	_contents = (char*)malloc(_size);
	if (!_contents) {
		fclose(f);
		printf("Unable to allocate memory for file %s\n", _filename.c_str());
		return false;
	}
	
	if (fread(_contents, _size, 1, f) != 1) {
		fclose(f);
		printf("Unable to read file %s\n", _filename.c_str());
		return false;
	}

	fclose(f);
	
	Lexer l;

	_is_lexed = l.lex(_contents, _size, [&](const TokenRange& range) {
		_tokens.push_back(std::shared_ptr<LexedFileToken>(new LexedFileToken(range.type, range.flags, range.location, range.length, self)));
	});
	
	return _is_lexed;
}

const std::list<std::shared_ptr<LexedFileToken>>& LexedFile::tokens() {
	return _tokens;
}

const char* LexedFile::contents() {
	return _contents;
}

size_t LexedFile::size() {
	return _size;
}

const std::string LexedFile::filename() {
	return _filename;
}

LexedFileToken::LexedFileToken(TokenType type, size_t location, size_t length, std::shared_ptr<LexedFile> file) : 
	_type(type),
	_flags(0),
	_location(location),
	_length(length),
	_file(file) {
}

LexedFileToken::LexedFileToken(TokenType type, uint32_t flags, size_t location, size_t length, std::shared_ptr<LexedFile> file) : 
	_type(type),
	_flags(flags),
	_location(location),
	_length(length),
	_file(file) {
}

TokenType LexedFileToken::type() {
	return _type;
}

uint32_t LexedFileToken::flags() {
	return _flags;
}

const std::string LexedFileToken::value() {
	return std::string(_file->contents() + _location, _length);
}

void LexedFileToken::print_pointer() {
	const char* start = _file->contents() + _location;
	int offset = 0;

	while (start > _file->contents() && start[-1] != '\r' && start[-1] != '\n') {
		--start;
		++offset;
	}

	while (*start == ' ' || *start == '\t') {
		++start;
		--offset;
	}

	const char* end = _file->contents() + _location;

	while (end - _file->contents() + 1 < _file->size() && end[1] != '\r' && end[1] != '\n') {
		++end;
	}

	printf("%s: %.*s\n", _file->filename().c_str(), (int)(end - start + 1), start);
	printf("%*s  %*s^\n", (int)_file->filename().size(), "", offset, "");
}
