#include "Parser.h"
#include "Preprocessor.h"

#include <sstream>
#include <algorithm>

Parser::Parser() {
	Scope global("^");

	global.types["void"]   = C3Type::VoidType();
	global.types["auto"]   = C3Type::AutoType();
	global.types["bool"]   = C3Type::BoolType();
	global.types["int8"]   = C3Type::Int8Type();
	global.types["uint8"]  = C3Type::ModifiedType(C3Type::Int8Type(), C3TypeModifierUnsigned);
	global.types["int32"]  = C3Type::Int32Type();
	global.types["uint32"] = C3Type::ModifiedType(C3Type::Int32Type(), C3TypeModifierUnsigned);
	global.types["int64"]  = C3Type::Int64Type();
	global.types["uint64"] = C3Type::ModifiedType(C3Type::Int64Type(), C3TypeModifierUnsigned);
	global.types["double"] = C3Type::DoubleType();

	_scopes.push_back(global);
	
	for (auto& kv : global.types) {
		_keywords.insert(kv.first);
	}
	
	_keywords.insert("class");
	_keywords.insert("const");
	_keywords.insert("extern");
	_keywords.insert("return");
	_keywords.insert("import");
	_keywords.insert("if");
	_keywords.insert("else");
	_keywords.insert("static");
	_keywords.insert("static_cast");
	_keywords.insert("namespace");
	_keywords.insert("nullptr");
	_keywords.insert("typename");

	// TODO: respect unary precedence
	_binary_ops["."]  = { 110, false };
	_binary_ops["->"] = { 110, false };
	 _unary_ops["+"]  = { 100, true };
	 _unary_ops["-"]  = { 100, true };
	 _unary_ops["*"]  = { 100, true };
	 _unary_ops["&"]  = { 100, true };
	 _unary_ops["!"]  = { 100, true };
	_binary_ops["*"]  = {  80, false };
	_binary_ops["/"]  = {  80, false };
	_binary_ops["%"]  = {  80, false };
	_binary_ops["+"]  = {  60, false };
	_binary_ops["-"]  = {  60, false };
	_binary_ops["<"]  = {  50, false };
	_binary_ops["<="] = {  50, false };
	_binary_ops[">"]  = {  50, false };
	_binary_ops[">="] = {  50, false };
	_binary_ops["=="] = {  40, false };
	_binary_ops["!="] = {  40, false };
	_binary_ops["&&"] = {  26, false };
	_binary_ops["||"] = {  25, false };
	_binary_ops["="]  = {  20, true };
}

ASTSequence* Parser::generate_ast(const std::list<TokenPtr>& tokens) {
	// may be called recursively when importing modules

	auto prev_cur_tok = _cur_tok;
	auto prev_end_tok = _end_tok;
	
	_cur_tok = tokens.begin();
	_end_tok = tokens.end();

	auto block = _parse_block();
	
	if (block && !_peek(ptt_end_token)) {
		_errors.push_back(ParseError("expected end of file", _token()));
		delete block;
		block = nullptr;
	}
	
	_cur_tok = prev_cur_tok;
	_end_tok = prev_end_tok;

	return block;
}

const std::list<ParseError>& Parser::errors() {
	return _errors;
}

TokenPtr Parser::_token() {
	if (_cur_tok != _end_tok) {
		return *_cur_tok;
	}

	class DummyToken : public Token {
		virtual TokenType type() { return TokenTypeOther; }
		virtual uint32_t flags() { return 0; }
		virtual const std::string value() { return std::string(); }
	};
	
	static TokenPtr dummy(new DummyToken());
	return dummy;
}

void Parser::_consume(size_t tokens) {
	for (size_t i = 0; i < tokens && _cur_tok != _end_tok; ++i) {
		++_cur_tok;
	}
}

TokenPtr Parser::_consume_token() {
	TokenPtr ret = _token();
	_consume(1);
	return ret;
}

bool Parser::_peek(ParserTokenType type, TokenIterator* next) {
	if (next) {
		*next = _cur_tok;
		++*next;
	}

	TokenPtr tok = _token();

	switch (type) {
		case ptt_open_angle:
			return tok->type() == TokenTypePunctuator && tok->value() == "<";
		case ptt_close_angle:
			return tok->type() == TokenTypePunctuator && tok->value() == ">";
		case ptt_semicolon:
			return tok->type() == TokenTypePunctuator && tok->value() == ";";
		case ptt_colon:
			return tok->type() == TokenTypePunctuator && tok->value() == ":";
		case ptt_open_brace:
			return tok->type() == TokenTypePunctuator && tok->value() == "{";
		case ptt_close_brace:
			return tok->type() == TokenTypePunctuator && tok->value() == "}";
		case ptt_open_paren:
			return tok->type() == TokenTypePunctuator && tok->value() == "(";
		case ptt_close_paren:
			return tok->type() == TokenTypePunctuator && tok->value() == ")";
		case ptt_comma:
			return tok->type() == TokenTypePunctuator && tok->value() == ",";
		case ptt_asterisk:
			return tok->type() == TokenTypePunctuator && tok->value() == "*";
		case ptt_ampersand:
			return tok->type() == TokenTypePunctuator && tok->value() == "&";
		case ptt_assignment:
			return tok->type() == TokenTypePunctuator && tok->value() == "=";
		case ptt_equality:
			return tok->type() == TokenTypePunctuator && tok->value() == "==";
		case ptt_inequality:
			return tok->type() == TokenTypePunctuator && tok->value() == "!=";
		case ptt_namespace_delimiter:
			return tok->type() == TokenTypePunctuator && tok->value() == "::";
		case ptt_keyword:
			return tok->type() == TokenTypeIdentifier && _keywords.count(tok->value()) > 0;
		case ptt_keyword_auto:
			return _peek(ptt_keyword) && tok->value() == "auto";
		case ptt_keyword_class:
			return _peek(ptt_keyword) && tok->value() == "class";
		case ptt_keyword_const:
			return _peek(ptt_keyword) && tok->value() == "const";
		case ptt_keyword_extern:
			return _peek(ptt_keyword) && tok->value() == "extern";
		case ptt_keyword_return:
			return _peek(ptt_keyword) && tok->value() == "return";
		case ptt_keyword_import:
			return _peek(ptt_keyword) && tok->value() == "import";
		case ptt_keyword_if:
			return _peek(ptt_keyword) && tok->value() == "if";
		case ptt_keyword_else:
			return _peek(ptt_keyword) && tok->value() == "else";
		case ptt_keyword_static:
			return _peek(ptt_keyword) && tok->value() == "static";
		case ptt_keyword_static_cast:
			return _peek(ptt_keyword) && tok->value() == "static_cast";
		case ptt_keyword_namespace:
			return _peek(ptt_keyword) && tok->value() == "namespace";
		case ptt_keyword_nullptr:
			return _peek(ptt_keyword) && tok->value() == "nullptr";
		case ptt_keyword_typename:
			return _peek(ptt_keyword) && tok->value() == "typename";
		case ptt_number:
			return tok->type() == TokenTypeNumber;
		case ptt_end_token:
			return _cur_tok == _end_tok;
		case ptt_unary_op:
			return tok->type() == TokenTypePunctuator && _unary_ops.find(tok->value()) != _unary_ops.end();
		case ptt_binary_op:
			return tok->type() == TokenTypePunctuator && _binary_ops.find(tok->value()) != _binary_ops.end();
		case ptt_string_literal:
			return tok->type() == TokenTypeStringLiteral;
		case ptt_char_constant:
			return tok->type() == TokenTypeCharacterConstant;
		case ptt_identifier: {
			return tok->type() == TokenTypeIdentifier;
		}
		case ptt_undefd_func_name: {
			if (!_peek(ptt_identifier)) {
				return false;
			}

			Scope& s = _scopes.back();

			auto fit = s.functions.find(s.local_prefix() + tok->value());
			if (fit != s.functions.end() && !fit->second->definition()) {
				return true;
			}

			return _peek(ptt_new_variable_name);
		}
		case ptt_nontype_name: {
			return _peek(ptt_identifier) && !_peek(ptt_keyword) && !_peek(ptt_type_name);
		}
		case ptt_new_type_name:
		case ptt_new_variable_name: {
			if (!_peek(ptt_identifier) || _peek(ptt_keyword) || _peek(ptt_local_type_name)) {
				return false;
			}

			Scope& s = _scopes.back();

			if (s.variables.count(s.local_prefix() + tok->value())) {
				return false;
			}

			if (s.functions.count(s.local_prefix() + tok->value())) {
				return false;
			}

			return true;
		}
		case ptt_local_type_name: {
			Scope& s = _scopes.back();
			return s.types.count(s.local_prefix() + tok->value());
		}
		case ptt_type: {
			auto tok = _cur_tok;
			auto ret = (bool)_try_parse_type();
			if (next) { *next = _cur_tok; }
			_cur_tok = tok;
			return ret;
		}
		case ptt_type_name: {
			return (bool)_resolve_type(tok->value());
		}
	}
	
	return false;
}

bool Parser::_peek(std::initializer_list<ParserTokenType> types) {
	TokenIterator prev_it = _cur_tok;
	TokenIterator next = _cur_tok;
	for (ParserTokenType type : types) {
		if (!_peek(type, &next)) {
			_cur_tok = prev_it;
			return false;
		}
		_cur_tok = next;
	}
	_cur_tok = prev_it;
	return true;
}

Parser::Scope& Parser::_push_scope(const std::string& name) {
	Scope s = Scope(_scopes.back().global_prefix() + name + ".");
	s.return_type = _scopes.back().return_type;
	_scopes.push_back(s);
	return _scopes.back();
}

Parser::Scope& Parser::_push_scope(C3FunctionPtr function) {
	Scope& s = _push_scope(function->name());
	s.return_type = function->return_type();
	return s;
}

void Parser::_pop_scope() {
	if (_scopes.size() == 1) {
		// don't pop the global scope
		return;
	}

	_scopes.pop_back();
}

std::string Parser::_try_parse_full_name() {
	std::string ret = "";

	while (true) {
		if (!_peek(ptt_identifier)) {
			return ret;
		}
		ret += _token()->value();
		_consume(1);
		if (!_peek(ptt_namespace_delimiter)) {
			return ret;
		}
		ret += _token()->value();
		_consume(1);
	}

	return ret;
}

C3TypePtr Parser::_resolve_type(const std::string& name) {
	for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
		auto& scope = *it;
		auto namespace_copy = scope.current_namespace;
		while (true) {
			std::string prefix = "";
			for (auto& str : namespace_copy) {
				prefix += str + "::";
			}
			auto it2 = scope.types.find(prefix + name);
			if (it2 != scope.types.end()) {
				return it2->second;
			}
			if (namespace_copy.empty()) {
				break;
			}
			namespace_copy.pop_back();
		};
	}
	
	return nullptr;
}

C3TypePtr Parser::_try_parse_type() {
	auto start = _cur_tok;

	bool is_constant = false;
	
	if (_peek(ptt_keyword_const)) {
		is_constant = true;
		_consume(1);
	}

	auto name = _try_parse_full_name();

	if (name.empty()) { return nullptr; }

	C3TypePtr type = _resolve_type(name);

	if (!type) {
		_cur_tok = start;
		return nullptr;
	}
	
	if (is_constant) {
		type = C3Type::ModifiedType(type, C3TypeModifierConstant);
	}
	
	while (true) {
		if (_peek(ptt_asterisk)) {
			type = C3Type::PointerType(type);
			_consume(1);
		} else if (_peek(ptt_ampersand)) {
			type = C3Type::ReferenceType(type);
			_consume(1);
		} else {
			break;
		}
	}

	return type;
}

C3VariablePtr Parser::_resolve_variable(const std::string& name) {
	for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
		auto& scope = *it;
		auto namespace_copy = scope.current_namespace;
		while (true) {
			std::string prefix = "";
			for (auto& str : namespace_copy) {
				prefix += str + "::";
			}
			auto it2 = scope.variables.find(prefix + name);
			if (it2 != scope.variables.end()) {
				return it2->second;
			}
			if (namespace_copy.empty()) {
				break;
			}
			namespace_copy.pop_back();
		};
	}
	
	return nullptr;
}

C3VariablePtr Parser::_try_parse_variable() {
	auto start = _cur_tok;

	auto name = _try_parse_full_name();

	if (name.empty()) { return nullptr; }

	if (auto variable = _resolve_variable(name)) {
		return variable;
	}

	_cur_tok = start;
	return nullptr;
}

std::vector<C3FunctionPtr> Parser::_function_candidates(const std::string& name) {
	std::vector<C3FunctionPtr> ret;
	
	for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
		auto& scope = *it;
		auto namespace_copy = scope.current_namespace;
		while (true) {
			std::string prefix = "";
			for (auto& str : namespace_copy) {
				prefix += str + "::";
			}
			// TODO: support overloading / multiple functions with same name
			auto it2 = scope.functions.find(prefix + name);
			if (it2 != scope.functions.end()) {
				ret.push_back(it2->second);
				return ret;
			}
			if (namespace_copy.empty()) {
				break;
			}
			namespace_copy.pop_back();
		};
	}

	return ret;
}

C3FunctionPtr Parser::_try_parse_function() {
	// only generate errors if the thing we're at is a function but there's something wrong with it
	
	auto start = _cur_tok;

	auto name = _try_parse_full_name();

	if (name.empty()) {
		// not even a name
		return nullptr;
	}
	
	auto function_candidates = _function_candidates(name);

	if (function_candidates.empty()) {
		// not a function
		_cur_tok = start;
		return nullptr;
	}
	
	// TODO: overload resolution
	return *function_candidates.begin();
}

ASTExpression* Parser::_implicit_conversion(ASTExpression* expression, C3TypePtr type) {
	auto rr_exp_type = C3Type::RemoveReference(expression->type);
	
	if (*rr_exp_type == *type) {
		return expression;
	}
	
	// TODO: this should really be rewritten

	if (expression->type->type() == C3TypeTypePointer && type->type() == C3TypeTypePointer && *C3Type::PointerType(C3Type::RemoveReference(expression->type->pointed_to_type())) == *type) {
		return expression;
	}

	if (true
		&& rr_exp_type->type() == C3TypeTypePointer && type->type() == C3TypeTypePointer
		&&  (   *C3Type::ModifiedType(rr_exp_type->pointed_to_type(), C3TypeModifierConstant) == *C3Type::ModifiedType(type->pointed_to_type(), C3TypeModifierConstant)
			|| (rr_exp_type->pointed_to_type()->type() != C3TypeTypePointer && type->pointed_to_type()->type() == C3TypeTypeVoid)
		)
		&& (!rr_exp_type->pointed_to_type()->is_constant() || type->pointed_to_type()->is_constant())
	) {
		return new ASTCast(expression, type);
	}

	if (expression->type->type() == C3TypeTypeNullPointer && type->type() == C3TypeTypePointer) {
		return new ASTCast(expression, type);
	}
	
	if (rr_exp_type->is_integer() && type->is_integer()) {
		return new ASTCast(expression, type);
	}

	return nullptr;
}

ASTExpression* Parser::_explicit_conversion(ASTExpression* expression, C3TypePtr type) {
	if (auto converted = _implicit_conversion(expression, type)) {
		return converted;
	}
	
	auto rr_exp_type = C3Type::RemoveReference(expression->type);
	
	if ((rr_exp_type->is_integer() || rr_exp_type->is_floating_point() || rr_exp_type->pointed_to_type()) && type->type() == C3TypeTypeBool) {
		return new ASTCast(expression, type);
	}

	return nullptr;
}

C3TypePtr Parser::_resolve_auto_type(C3TypePtr auto_type, C3TypePtr target) {
	if (!auto_type->is_auto()) { return auto_type; }

	if (auto_type->type() == C3TypeTypeReference) {
		if (target->type() == C3TypeTypeReference) {
			auto inner = _resolve_auto_type(auto_type->referenced_type(), target->referenced_type());
			return inner ? C3Type::ModifiedType(C3Type::ReferenceType(inner), auto_type->modifiers()) : nullptr;
		}
		return nullptr;
	}

	if (auto_type->type() == C3TypeTypePointer) {
		if (target->type() == C3TypeTypePointer) {
			auto inner = _resolve_auto_type(auto_type->pointed_to_type(), target->pointed_to_type());
			return inner ? C3Type::ModifiedType(C3Type::PointerType(inner), auto_type->modifiers()) : nullptr;
		}
		return nullptr;
	}
	
	if (auto_type->type() == C3TypeTypeAuto) {
		return C3Type::ModifiedType(C3Type::RemoveReference(target), auto_type->modifiers());
	}

	return nullptr;
}

ASTVariableDec* Parser::_parse_variable_dec() {
	bool is_static = _scopes.size() == 1;

	if (_peek(ptt_keyword_static)) {
		is_static = true;
		_consume(1); // static
	}

	auto type = _try_parse_type();

	if (!type) {
		_errors.push_back(ParseError("expected type", _token()));
		return nullptr;
	}

	if (!_peek(ptt_new_variable_name)) {
		_errors.push_back(ParseError("expected new variable name", _token()));
		return nullptr;
	}

	TokenPtr name_tok = _consume_token();

	Scope& scope = _scopes.back();
	
	ASTExpression* init = nullptr;
	
	if (_peek(ptt_assignment)) {
		// initial value
		_consume(1); // consume '='
		init = _parse_expression();
		if (!init) {
			return nullptr;
		}
		if (type->is_auto()) {
			auto resolved = _resolve_auto_type(type, init->type);
			if (!resolved) {
				_errors.push_back(ParseError("unable to resolve auto type", name_tok));
				delete init;
				return nullptr;
			}
			type = resolved;
		}
		auto converted = _implicit_conversion(init, type);
		if (!converted) {
			std::string msg("unable to initialize variable of type '");
			msg += type->name() + "' with expression of type '" + init->type->name() + "'";
			_errors.push_back(ParseError(msg.c_str(), name_tok));
			delete init;
			return nullptr;
		}
		init = converted;
		if (is_static && !init->is_constant) {
			_errors.push_back(ParseError("unable to initialize static variable with non-constant expression", name_tok));
			delete init;
			return nullptr;
		}
	} else if (type->is_auto()) {
		_errors.push_back(ParseError("variables with auto types must have an initialization", name_tok));
	}

	C3VariablePtr var = C3VariablePtr(new C3Variable(type, name_tok->value(), scope.global_prefix() + name_tok->value(), name_tok, is_static));
	scope.variables[scope.local_prefix() + var->name()] = var;

	return new ASTVariableDec(var, init);
}

ASTNode* Parser::_parse_function_proto_or_def(bool* was_just_proto) {
	// function prototype	
	bool args_are_named = false;
	TokenPtr protoTok = _token();
	ASTFunctionProto* proto = _parse_function_proto(&args_are_named);
	if (proto && _peek(ptt_open_brace)) {
		// function body
		ASTNode* node = nullptr;
		if (!args_are_named) {
			// unnamed arguments
			_errors.push_back(ParseError("function definition has unnamed arguments", _token()));
			// try to recover
			_consume(1);
			_push_scope(proto->func);
			delete _parse_block();
			_pop_scope();
		} else {
			// set up / parse the function body
			proto->func->set_definition(_token());
			_consume(1);
			_push_scope(proto->func);
			// add the arguments to the scope
			Scope& scope = _scopes.back();
			for (size_t i = 0; i < proto->arg_names.size(); ++i) {
				scope.variables[proto->arg_names[i]] = C3VariablePtr(new C3Variable(proto->func->arg_types()[i], proto->arg_names[i], scope.global_prefix() + proto->arg_names[i], protoTok));
			}
			// parse the body
			ASTSequence* body = _parse_block();
			if (body) {
				if (!_peek(ptt_close_brace)) {
					_errors.push_back(ParseError("expected closing brace", _token()));
					delete body;
				} else {
					_consume(1); // }
					node = new ASTFunctionDef(proto, body, scope.global_prefix());
				}
			}
			_pop_scope();
			// TODO: check for return statement
		}
		if (was_just_proto) {
			*was_just_proto = false;
		}
		return node;
	}
	
	// just the proto
	if (was_just_proto) {
		*was_just_proto = true;
	}
	return proto;
}

ASTFunctionProto* Parser::_parse_function_proto(bool* args_are_named) {
	auto return_type = _try_parse_type();
	
	if (!return_type) {
		_errors.push_back(ParseError("expected type", _token()));
		return nullptr;
	}
	
	if (return_type->is_auto()) {
		_errors.push_back(ParseError("cannot declare an auto return type", _token()));
		return nullptr;
	}
	
	if (!_peek(ptt_undefd_func_name)) {
		_errors.push_back(ParseError("expected undefined function name", _token()));
		return nullptr;
	}

	TokenPtr tok = _consume_token();
	
	if (!_peek(ptt_open_paren)) {
		_errors.push_back(ParseError("expected open parenthesis", _token()));
		return nullptr;
	}

	_consume(1); // consume open parenthesis

	std::vector<C3TypePtr>   args;
	std::vector<std::string> names;
	
	while (!_peek(ptt_close_paren)) {
		C3TypePtr arg_type = _try_parse_type();

		if (!arg_type) {
			_errors.push_back(ParseError("expected argument type", _token()));
			return nullptr;
		}

		if (arg_type->is_auto()) {
			_errors.push_back(ParseError("cannot declare an auto argument type", _token()));
			return nullptr;
		}

		bool named = false;
		if (_peek(ptt_identifier) && !_peek(ptt_type_name)) {
			std::string name = _token()->value();
			named = true;
			for (std::string& n : names) {
				if (name == n) {
					_errors.push_back(ParseError("duplicate argument name", _token()));
					return nullptr;
				}
			}
			names.push_back(name);
			_consume(1);
		}
		
		args.push_back(arg_type);

		if (_peek(ptt_comma)) {
			_consume(1);
		} else if (!_peek(ptt_close_paren)) {
			_errors.push_back(ParseError(named ? "expected comma or end of argument list" : "expected comma, name, or end of argument list", _token()));
			return nullptr;
		}		
	}

	_consume(1); // consume close parenthesis

	if (args_are_named) {
		*args_are_named = (args.size() == names.size());
	}

	Scope& scope = _scopes.back();
	auto global_name = scope.global_prefix() + tok->value();
	if (global_name == _scopes.front().prefix + "main") {
		global_name = "main";
	}
	C3FunctionPtr func = C3FunctionPtr(new C3Function(return_type, tok->value(), global_name, std::move(args), tok));

	auto fit = scope.functions.find(tok->value());
	if (fit != scope.functions.end()) {
		if (func->signature() != fit->second->signature()) {
			_errors.push_back(ParseError("function has different signature than previous declaration", tok));
			return nullptr;
		}
		return new ASTFunctionProto(fit->second, names);
	}

	scope.functions[scope.local_prefix() + func->name()] = func;
	return new ASTFunctionProto(func, names);
}

ASTFunctionCall* Parser::_parse_function_call(ASTExpression* func) {
	if (func->type->type() != C3TypeTypeFunction) {
		_errors.push_back(ParseError("previous expression is not a function", _token()));
		return nullptr;
	}
	
	if (!_peek(ptt_open_paren)) {
		_errors.push_back(ParseError("expected '('", _token()));
		return nullptr;
	}
	_consume(1); // consume '('

	std::vector<ASTExpression*> args;
	const std::vector<C3TypePtr>& arg_types = func->type->signature().arg_types();

	for (size_t i = 0; i < arg_types.size(); ++i) {
		if (i > 0) {
			if (!_peek(ptt_comma)) {
				_errors.push_back(ParseError("expected ','", _token()));
				for (ASTExpression* exp : args) {
					delete exp;
				}
				return nullptr;
			}
			_consume(1); // consume comma
		}
		TokenPtr arg_tok = _token();
		ASTExpression* arg = _parse_expression();
		if (!arg) {
			for (ASTExpression* exp : args) {
				delete exp;
			}
			return nullptr;
		}
		auto converted = _implicit_conversion(arg, arg_types[i]);
		if (!converted) {
			std::string msg = "invalid type for argument (expected '";
			msg += arg_types[i]->name() + "' but got '" + arg->type->name() + "')";
			_errors.push_back(ParseError(msg, arg_tok));
			// try to recover...
		}
		args.push_back(converted ? converted : arg);
	}

	if (!_peek(ptt_close_paren)) {
		_errors.push_back(ParseError("expected ')'", _token()));
		for (ASTExpression* exp : args) {
			delete exp;
		}
		return nullptr;
	}
	_consume(1); // consume ')'

	return new ASTFunctionCall(func, args);
}

ASTNode* Parser::_parse_class_dec_or_def() {
	// TODO: allow separate declarations / definitions

	if (!_peek(ptt_keyword_class)) {
		return nullptr;
	}
	
	_consume(1); // struct
	
	if (!_peek(ptt_new_type_name)) {
		_errors.push_back(ParseError("expected new type name", _token()));
		return nullptr;
	}
	
	auto name = _consume_token();
	
	if (!_peek(ptt_open_brace)) {
		_errors.push_back(ParseError("expected opening brace", _token()));
		return nullptr;
	}
	
	_consume(1); // {
	
	std::vector<C3StructDefinition::MemberVariable> member_vars;
	
	_push_scope(name->value());
	while (!_peek(ptt_close_brace)) {
		C3TypePtr type = _try_parse_type();
		if (!type) {
			_errors.push_back(ParseError("expected type", _token()));
			return nullptr;
		}
		if (type->is_auto()) {
			_errors.push_back(ParseError("cannot declare an auto member type", _token()));
			return nullptr;
		}
		if (!_peek(ptt_new_variable_name)) {
			_errors.push_back(ParseError("expected new member name", _token()));
			return nullptr;
		}
		auto name = _consume_token();
		member_vars.emplace_back(name->value(), type);
		if (!_peek(ptt_semicolon)) {
			_errors.push_back(ParseError("expected semicolon", _token()));
			// try to recover
		} else {
			_consume(1); // ;
		}
	}
	_pop_scope();

	if (!_peek(ptt_close_brace)) {
		_errors.push_back(ParseError("expected closing brace", _token()));
		return nullptr;
	}

	_consume(1); // }

	Scope& scope = _scopes.back();
	scope.types[scope.local_prefix() + name->value()] = C3Type::StructType(name->value(), scope.global_prefix() + name->value(), C3StructDefinition(std::move(member_vars)));

	return new ASTNop();
}

ASTExpression* Parser::_parse_binop_rhs(ASTExpression* lhs) {
	TokenPtr tok = _consume_token();

	if (tok->type() != TokenTypePunctuator || tok->value() == ";") {
		_errors.push_back(ParseError("expected binary operator", _token()));
		delete lhs;
		return nullptr;
	}
	
	if (tok->value() == "." || tok->value() == "->") {
		if (tok->value() == "->") {
			if (C3Type::RemoveReference(lhs->type)->type() != C3TypeTypePointer) {
				_errors.push_back(ParseError(std::string("dereferencing selection operator used on non-pointer type '" + lhs->type->name() + "'"), _token()));
				delete lhs;
				return nullptr;
			}
			lhs = new ASTUnaryOp("*", lhs, C3Type::ReferenceType(C3Type::RemoveReference(lhs->type)->pointed_to_type()));
		}
		auto type = lhs->type;
		auto rr_type = C3Type::RemoveReference(type);
		if (rr_type->type() != C3TypeTypeStruct) {
			_errors.push_back(ParseError(std::string("selection operator used on non-struct type '") + type->name() + "'", _token()));
			delete lhs;
			return nullptr;
		}
		if (!rr_type->is_defined()) {
			_errors.push_back(ParseError("selection operator used on undefined struct", _token()));
			delete lhs;
			return nullptr;
		}
		auto member_vars = rr_type->struct_definition().member_vars();
		for (size_t i = 0; i < member_vars.size(); ++i) {
			if (member_vars[i].name == _token()->value()) {
				_consume(1); // member name
				return new ASTStructMemberRef(lhs, i);
			}
		}
		_errors.push_back(ParseError("expected struct member", _token()));
		delete lhs;
		return nullptr;
	}
	
	auto precedence = _binary_ops[tok->value()];

	ASTExpression* rhs = _parse_expression(precedence);

	if (!rhs) {
		delete lhs;
		return nullptr;
	}

	auto lhs_rr_type = C3Type::RemoveReference(lhs->type);
	auto rhs_rr_type = C3Type::RemoveReference(rhs->type);

	C3TypePtr result_type = lhs_rr_type;
	bool compatible = false;

	if (tok->value() == "=") {
		if (!lhs->type->referenced_type() || lhs->type->is_constant()) {
			compatible = false;
		} else if (auto converted = _implicit_conversion(rhs, lhs->type->referenced_type())) {
			compatible = true;
			rhs = converted;
		} else {
			compatible = false;
		}
	} else if (tok->value() == "==" || tok->value() == "!=" || tok->value() == "<" || tok->value() == "<=" || tok->value() == ">" || tok->value() == ">=") {
		compatible = ((lhs_rr_type->is_floating_point() && rhs_rr_type->is_floating_point()) || (lhs_rr_type->is_integer() && rhs_rr_type->is_integer()));
		result_type = C3Type::BoolType();
	} else if (tok->value() == "&&" || tok->value() == "||") {
		if (auto converted = _explicit_conversion(lhs, C3Type::BoolType())) {
			lhs = converted;
		}
		if (auto converted = _explicit_conversion(rhs, C3Type::BoolType())) {
			rhs = converted;
		}
		compatible = C3Type::RemoveReference(lhs->type)->type() == C3TypeTypeBool && C3Type::RemoveReference(rhs->type)->type() == C3TypeTypeBool;
		result_type = C3Type::BoolType();
	} else if (*lhs_rr_type == *rhs_rr_type) {
		// values of the same type are compatible
		compatible = true;
	} else if (lhs_rr_type->is_integer() && rhs_rr_type->is_integer()) {
		// integers are compatible because they get promoted as necessary
		compatible = true;
		result_type = C3Type::Int64Type();
	} else if (lhs_rr_type->type() == C3TypeTypePointer && lhs_rr_type->pointed_to_type()->type() != C3TypeTypeVoid && rhs_rr_type->is_integer() && (tok->value() == "+" || tok->value() == "-")) {
		// pointer arithmetic
		compatible = true;
	}

	if (!compatible) {
		std::string msg = "incompatible types to binary operator ('";
		msg += lhs->type->name() + "' and '" + rhs->type->name() + "')";
		_errors.push_back(ParseError(msg, tok));
		// try to recover...
	}

	return new ASTBinaryOp(tok->value(), lhs, rhs, result_type);
}

ASTNode* Parser::_parse_external_declaration() {
	if (!_peek(ptt_keyword_extern)) {
		_errors.push_back(ParseError("expected 'extern'", _token()));
		return nullptr;
	}
	
	_consume(1); // extern
	
	auto proto = _parse_function_proto();
	if (!proto) {
		return nullptr;
	}
	
	if (!_peek(ptt_colon)) {
		_errors.push_back(ParseError("expected colon", _token()));
		// try to continue
	} else {
		_consume(1);
	}

	if (!_peek(ptt_string_literal)) {
		_errors.push_back(ParseError("expected external symbol name (a string literal)", _token()));
		delete proto;
		return nullptr;
	}
	
	auto symbol = _consume_token();
	proto->func->set_global_name(symbol->value());
	
	return proto;
}

ASTReturn* Parser::_parse_return() {
	if (!_peek(ptt_keyword_return)) {
		_errors.push_back(ParseError("expected 'return'", _token()));
		return nullptr;
	}
	
	C3TypePtr expected_type = _scopes.back().return_type;
	
	if (!expected_type) {
		_errors.push_back(ParseError("unexpected return statement", _token()));
		// recover...
	}

	_consume(1); // consume 'return'

	if (expected_type->type() == C3TypeTypeVoid) {
		return new ASTReturn(nullptr);
	}

	ASTExpression* exp = _parse_expression();

	if (!exp) {
		return nullptr;
	}
	
	auto converted = _implicit_conversion(exp, expected_type);
	
	if (!converted) {
		std::string msg = "invalid return type (expected '";
		msg += expected_type->name() + "' but got '" + exp->type->name() + "'";
		_errors.push_back(ParseError(msg, _token()));
		// recover...
	}
	
	return new ASTReturn(converted ? converted : exp);
}

ASTExpression* Parser::_parse_primary() {
	if (auto var = _try_parse_variable()) {
		return new ASTVariableRef(var);
	} else if (auto func = _try_parse_function()) {
		return new ASTFunctionRef(func);
	} else if (_peek(ptt_keyword_static_cast)) {
		// static cast
		return _parse_static_cast();
	} else if (_peek(ptt_keyword_nullptr)) {
		// null pointer
		_consume(1); // nullptr
		return new ASTNullPointer(C3Type::NullPointerType());
	} else if (_peek(ptt_number)) {
		// number
		TokenPtr tok = _consume_token();
		if (tok->value().find_first_of('.') != std::string::npos) {
			return new ASTFloatingPoint(atof(tok->value().c_str()), C3Type::DoubleType());
		} else {
			// TODO: support other bases
			return new ASTInteger(strtoll(tok->value().c_str(), NULL, 10), C3Type::Int64Type());
		}
	} else if (_peek(ptt_char_constant)) {
		// character constant
		TokenPtr tok = _consume_token();
		uint64_t value = 0;
		for (const char& c : tok->value()) {
			value <<= 8;
			value |= (unsigned char)c;
		}
		return new ASTInteger(value, C3Type::Int64Type());
	} else if (_peek(ptt_string_literal)) {
		// string literal
		TokenPtr tok = _consume_token();
		return new ASTConstantArray(tok->value().c_str(), tok->value().size() + 1, C3Type::ModifiedType(C3Type::Int8Type(), C3TypeModifierUnsigned | C3TypeModifierConstant));
	} else if (_peek(ptt_open_paren)) {
		// parenthesized expression
		_consume(1); // (
		auto exp = _parse_expression();
		if (!exp) {
			return nullptr;
		}
		if (!_peek(ptt_close_paren)) {
			_errors.push_back(ParseError("expected closing parenthesis", _token()));
			delete exp;
			return nullptr;
		}
		_consume(1); // )
		return exp;
	}

	_errors.push_back(ParseError("unexpected token", _token())); // intentionally vague
	return nullptr;
}

ASTCast* Parser::_parse_static_cast() {
	if (!_peek(ptt_keyword_static_cast)) {
		_errors.push_back(ParseError("expected static_cast", _token()));
		return nullptr;
	}
	
	auto static_cast_tok = _consume_token();
	
	if (!_peek(ptt_open_angle)) {
		_errors.push_back(ParseError("expected opening angle bracket for type", _token()));
		return nullptr;
	}
	_consume(1); // <

	auto type = _try_parse_type();
	if (!type) {
		_errors.push_back(ParseError("expected type", _token()));
		return nullptr;
	}

	if (type->is_auto()) {
		_errors.push_back(ParseError("cannot static cast to auto type", _token()));
		return nullptr;
	}

	if (!_peek(ptt_close_angle)) {
		_errors.push_back(ParseError("expected closing angle bracket for type", _token()));
		return nullptr;
	}
	_consume(1); // >

	if (!_peek(ptt_open_paren)) {
		_errors.push_back(ParseError("expected opening parenthesis", _token()));
		return nullptr;
	}
	_consume(1); // (

	auto expression = _parse_expression();
	if (!expression) {
		return nullptr;
	}

	if (!_peek(ptt_close_paren)) {
		_errors.push_back(ParseError("expected closing parenthesis", _token()));
		delete expression;
		return nullptr;
	}
	_consume(1); // )
		
	if (type->type() == C3TypeTypePointer && expression->type->type() == C3TypeTypePointer && type->pointed_to_type()->is_constant() && !expression->type->pointed_to_type()->is_constant()) {
		_errors.push_back(ParseError("cannot cast away constness", static_cast_tok));
		delete expression;
		return nullptr;
	}

	// TODO: enforce more of the limitations of static_cast

	return new ASTCast(expression, type);
}
		
ASTExpression* Parser::_parse_expression(Precedence minPrecedence) {
	ASTExpression* exp = nullptr;

	if (_peek(ptt_unary_op)) {
		// unary operation

		auto precedence = _unary_ops[_token()->value()];
		
		if (precedence.rank < minPrecedence.rank || (precedence.rank == minPrecedence.rank && !minPrecedence.rtol)) {
			return nullptr;
		}

		auto tok = _consume_token();
		auto rhs_tok = _token();

		auto rhs = _parse_expression(precedence);
		if (!rhs) {
			return nullptr;
		}

		if (tok->value() == "&") {
			if (!rhs->type->referenced_type()) {
				_errors.push_back(ParseError("operand to '&' operator must be a reference", rhs_tok));
			} else {
				exp = new ASTUnaryOp(tok->value(), rhs, C3Type::PointerType(rhs->type));
			}
		} else if (tok->value() == "*") {
			if (C3Type::RemoveReference(rhs->type)->type() != C3TypeTypePointer) {
				_errors.push_back(ParseError("operand to '*' operator must be a pointer type", rhs_tok));
			} else {
				exp = new ASTUnaryOp(tok->value(), rhs, C3Type::ReferenceType(C3Type::RemoveReference(rhs->type)->pointed_to_type()));
			}
		} else if (tok->value() == "!") {
			auto converted = _explicit_conversion(rhs, C3Type::BoolType());
			if (!converted) {
				_errors.push_back(ParseError("operand to '!' operator must be convertible to bool", rhs_tok));
			} else {
				exp = new ASTUnaryOp(tok->value(), converted, C3Type::BoolType());
			}
		} else if (tok->value() == "-") {
			auto rr_type = C3Type::RemoveReference(rhs->type);
			if (!rr_type->is_integer() && !rr_type->is_floating_point()) {
				_errors.push_back(ParseError("operand to unary '-' operator must be integer or floating point", rhs_tok));
			} else {
				rr_type->set_modifiers(0);
				exp = new ASTUnaryOp(tok->value(), rhs, rr_type);
			}
		}
		
		if (!exp) {
			delete rhs;
			return nullptr;
		}
	}

	if (!exp) {
		exp = _parse_primary();
	}

	if (!exp) {
		return nullptr;
	}
	
	if (_peek(ptt_open_paren)) {
		// function call
		ASTExpression* call = _parse_function_call(exp);
		if (!call) {
			delete exp;
			return nullptr;
		}
		exp = call;
	}

	while (_peek(ptt_binary_op)) {
		auto precedence = _binary_ops[_token()->value()];
		
		if (precedence.rank < minPrecedence.rank || (precedence.rank == minPrecedence.rank && !minPrecedence.rtol)) {
			break;
		}
		
		if (!(exp = _parse_binop_rhs(exp))) {
			return nullptr;
		}
	}

	return exp;
}

ASTSequence* Parser::_parse_block() {
	ASTSequence* seq = new ASTSequence();

	while (true) {
		while (_peek(ptt_semicolon)) { _consume(1); }

		if (_peek(ptt_end_token) || _peek(ptt_close_brace)) {
			return seq;
		}

		ASTNode* node = _parse_statement();
		if (!node) {
			break;
		}

		seq->sequence.push_back(node);
	}
	
	delete seq;
	return nullptr;
}

ASTNode* Parser::_parse_statement() {
	ASTNode* node = nullptr;
	bool expect_semicolon = true;

	if (_peek(ptt_keyword_import)) {
		auto import_token = _consume_token();
		if (_scopes.size() > 1) {
			_errors.push_back(ParseError("imports can only be made in the global scope", import_token));
			return nullptr;
		}
		Scope& s = _scopes.back();
		if (!s.current_namespace.empty()) {
			_errors.push_back(ParseError("imports can only be made in the top level namespace", import_token));
			return nullptr;
		}
		if (!_peek(ptt_identifier)) {
			_errors.push_back(ParseError("expected module name", _token()));
			return nullptr;
		}
		auto name = _consume_token()->value();
		if (_imported_modules.insert(name).second) {
			Preprocessor pp;
			// TODO: some sort of module searching
			if (!pp.process_file((std::string("modules/") + name + "/" + name + ".c3").c_str())) {
				_errors.push_back(ParseError("unable to import module", import_token));
				return nullptr;
			}
			node = generate_ast(pp.tokens());
			if (!node) {
				return nullptr;
			}
		} else {
			node = new ASTNop();
		}
	} else if (_peek(ptt_keyword_namespace)) {
		_consume(1); // namespace
		if (!_peek(ptt_nontype_name)) {
			_errors.push_back(ParseError("expected namespace name", _token()));
			return nullptr;
		}
		auto name = _token()->value();
		_consume(1); // name
		if (!_peek(ptt_open_brace)) {
			_errors.push_back(ParseError("expected opening brace", _token()));
			return nullptr;
		}
		_consume(1); // {
		Scope& s = _scopes.back();
		s.current_namespace.push_back(name);
		node = _parse_block();
		s.current_namespace.pop_back();
		if (node) {
			if (!_peek(ptt_close_brace)) {
				_errors.push_back(ParseError("expected closing brace", _token()));
				delete node;
				return nullptr;
			}
			_consume(1); // }
			expect_semicolon = false;
		}
	} else if (_peek(ptt_open_brace)) {
		// new code block
		_consume(1); // {
		_push_scope();
		node = _parse_block();
		_pop_scope();
		if (node) {
			if (!_peek(ptt_close_brace)) {
				_errors.push_back(ParseError("expected closing brace", _token()));
				delete node;
				return nullptr;
			}
			_consume(1); // }
			expect_semicolon = false;
		}
	} else if (_peek(ptt_keyword_if)) {
		// if block
		_consume(1); // if
		if (!_peek(ptt_open_paren)) {
			_errors.push_back(ParseError("expected opening parenthesis", _token()));
			return nullptr;
		}
		_consume(1); // (
		auto condition = _parse_expression();
		if (!condition) {
			return nullptr;
		}
		auto converted = _explicit_conversion(condition, C3Type::BoolType());
		if (!converted) {
			_errors.push_back(ParseError("condition not convertible to boolean", _token()));
			delete condition;
			return nullptr;
		}
		condition = converted;
		if (!_peek(ptt_close_paren)) {
			_errors.push_back(ParseError("expected closing parenthesis", _token()));
			delete condition;
			return nullptr;
		}
		_consume(1); // )
		_push_scope();
		auto true_path = _parse_statement();
		_pop_scope();
		if (!true_path) {
			delete condition;
			return nullptr;
		}
		ASTNode* false_path = nullptr;
		if (_peek(ptt_keyword_else)) {
			_consume(1); // else
			_push_scope();
			false_path = _parse_statement();
			_pop_scope();
			if (!false_path) {
				delete condition;
				delete true_path;
				return nullptr;
			}
		}
		node = new ASTCondition(condition, true_path, false_path ? false_path : new ASTSequence());
		expect_semicolon = false;
	} else if (_scopes.size() == 1 && _peek(ptt_keyword_extern)) {
		// external declaration
		node = _parse_external_declaration();
	} else if (_peek(ptt_keyword_return)) {
		// return statement
		node = _parse_return();
	} else if (_peek(ptt_keyword_class)) {
		// struct declaration or definition
		node = _parse_class_dec_or_def();
		expect_semicolon = false;
	} else if (_peek(ptt_keyword_static)) {
		// static variable declaration
		node = _parse_variable_dec();
	} else if (_scopes.size() == 1 && _peek({ptt_type, ptt_identifier, ptt_open_paren})) {
		// function proto or def
		bool just_proto = true;
		node = _parse_function_proto_or_def(&just_proto);
		expect_semicolon = just_proto;
	} else if (_peek(ptt_type)) {
		// variable declaration
		node = _parse_variable_dec();
	} else {
		// expression
		node = _parse_expression();
	}

	if (node && expect_semicolon && !_peek(ptt_semicolon)) {
		node->print();
		_errors.push_back(ParseError("expected semicolon", _token()));
		// try to continue anyways
	}
	
	return node;
}

