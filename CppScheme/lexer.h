#ifndef _MINISCHEME_LEXER_H
#define _MINISCHEME_LEXER_H

#include <iostream>
#include <string>
#include "Object.h"
#include <cctype>

#ifdef DEBUG_TOKEN
#include <iostream>
#define DEBUG_STDERR_TOKEN_TOKEN(x, y) (std::cerr <<"current TOKEN is " << (x) << " "<< y<< "\n");
#define DEBUG_STDOUT_TOKEN(x, y) (std::cout <<"current TOKEN is " << (x) << " "<< y<< "\n");
#include <iosfwd>
//... etc
#else 
#define DEBUG_STDERR_TOKEN_TOKEN(x, y)
#define DEBUG_STDOUT_TOKEN(x, y)
//... etc
#endif

enum TOKEN{

	IDENTIFIER = 1,
	NUMBER,
	INTEGER,


	//关键字
	LAMBDA,
	IF,
	DEFINE,
	COND,
	ELSE,
	LET,

	//基本操作符
	/*ADD,
	SUB,
	MUL,
	DIV,

	LESS,
	GREATER,
	EQUAL,*/

	//左右括号
	LBRACE,
	RBRACE,

	//换行

	UNKNOWN,
	TEOF,
};


static double current_double;
static int current_int;
static std::string current_identifer;
static char cur_char;
static int current_line_number = 1;
static TOKEN current_tok;

static bool is_identifier_char(char ch){
	return isalpha(ch) || ch == '_' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '<' || ch == '>' || ch == '=' || ch == '!' || ch == '&' || ch == '*' || ch == '^';
}

bool push_token = false;
TOKEN token_pushed;

void push(TOKEN _tok){
	if (push_token == false){
		push_token = true;
		token_pushed = _tok;
	}
}

static TOKEN get_token(std::istream &input){
	if (push_token){
		push_token = false;
		current_tok = token_pushed;
		return token_pushed;
	}

	while (1){
		//重构，以switch语句替代ifelse
		switch (cur_char)
		{
		case EOF:
			std::cout << "EOF" << std::endl;
			return TOKEN::TEOF;
		case ' ':
		case '\t':
		case '\r':
			cur_char = input.get();
			break;
		case '\n':
			//DEBUG_STDOUT_TOKEN(cur_char, "\"\\n\"");
			++current_line_number;
			cur_char = input.get();
			break;
		case '(':
			DEBUG_STDOUT_TOKEN("", "\"(\", TOKEN::LBRACE");
			cur_char = input.get();
			return TOKEN::LBRACE;
		case ')':
			DEBUG_STDOUT_TOKEN("", "\")\", TOKEN::RBRACE");
			cur_char = input.get();
			return TOKEN::RBRACE;
		case ';':		//comment begin;
			DEBUG_STDOUT_TOKEN("", "\";\", Comment");
			while (cur_char != '\n') {
				cur_char = input.get();
			}
			break;
			//或者在default里利用isdigit处理
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '9':
		{
			std::string tmp;
			int dot_number = 0;
			bool isNumber = true;
			while (!isspace(cur_char) && cur_char != '(' && cur_char != ')' && cur_char != ';' && cur_char != '#' && cur_char != EOF) {
				if (cur_char == '.'){
					++dot_number;
				}

				if (!isdigit(cur_char) && cur_char != '.'){
					isNumber = false;
				}
				tmp += cur_char;
				cur_char = input.get();
			}

			if (isNumber){
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DOUBLE");
				current_double = stod(tmp);
				return TOKEN::NUMBER;
			}
			else{
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFIER");
				current_identifer = tmp;
				return TOKEN::IDENTIFIER;
			}
		}
		default:
			//identifier类型
			if (is_identifier_char(cur_char) || cur_char == '_'){
				std::string tmp(1, cur_char);
				cur_char = input.get();
				while (!isspace(cur_char) && cur_char != '(' && cur_char != ')' && cur_char != ';' && cur_char != EOF) {					tmp += cur_char;					cur_char = input.get();
				}
				current_identifer = tmp;

				if (tmp == "define"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::DEFINE");
					return TOKEN::DEFINE;
				}
				else if (tmp == "if"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IF");
					return TOKEN::IF;
				}
				else if (tmp == "cond"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::COND");
					return TOKEN::COND;
				}
				else if (tmp == "else"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::ELSE");
					return TOKEN::ELSE;
				}
				else if (tmp == "lambda"){
					DEBUG_STDOUT_TOKEN(tmp, "TOKEN::LAMBDA");
					return TOKEN::LAMBDA;
				}
				DEBUG_STDOUT_TOKEN(tmp, "TOKEN::IDENTIFER");
				return TOKEN::IDENTIFIER;
			}
			else
				DEBUG_STDOUT_TOKEN(cur_char, "TOKEN::UNKNOWN");
			cur_char = input.get();
			return TOKEN::UNKNOWN;
		}


		/*
				if (cur_char == ' ' || cur_char == '\t' ||cur_char == '\r'){
				cur_char = input.get();
				continue;
				}
				else if (cur_char == '\n'){
				DEBUG_STDOUT_TOKEN("\\n");
				++current_line_number;
				cur_char = input.get();
				continue;
				}
				else if (cur_char == '('){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::LBRACE;
				}
				else if (cur_char == ')'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::RBRACE;
				}
				//eat comment
				else if (cur_char == ';'){

				while (cur_char != '\n') {
				cur_char = input.get();
				}
				continue;
				}
				//如果目前的cur_char为数字
				else if (isdigit(cur_char)){
				std::string tmp;
				int dot_number = 0;
				char next_char;
				while (isdigit(cur_char) || cur_char == '.') {
				tmp += cur_char;
				next_char = input.get();

				if (cur_char == '.'){
				++dot_number;
				if (dot_number > 1){
				std::cerr << "invalid double, more one dot" << std::endl;
				DEBUG_STDOUT_TOKEN(tmp);
				return current_tok = TOKEN::UNKNOWN;
				}

				if (!isdigit(next_char)){
				std::cerr << "invalid number, \"find dot at the last position\"" << std::endl;
				DEBUG_STDOUT_TOKEN(tmp);
				return current_tok = TOKEN::UNKNOWN;
				}
				}
				cur_char = next_char;
				}

				//可能是RBRACE,所以不能如此判断
				/ *if (!isspace(next_char)){
				std::cerr << "inviliad number, unknown end character at the end of the number" << std::endl;
				return UNKNOWN;
				}* /

				DEBUG_STDOUT_TOKEN(tmp);
				if (dot_number == 0){
				current_int = std::stoi(tmp);
				return current_tok = TOKEN::INTEGER;
				}
				else{
				current_double = std::stod(tmp);
				return current_tok = TOKEN::DOUBLE;
				}
				}
				/ *else if (cur_char == '+'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::ADD;
				}
				else if (cur_char == '-'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::SUB;
				}
				else if (cur_char == '*'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::MUL;
				}
				else if (cur_char == '\\'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::DIV;
				}
				else if (cur_char == '<'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::LESS;
				}
				else if (cur_char == '>'){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::GREATER;
				}
				else if (cur_char == '='){
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::EQUAL;
				}* /
				else if (is_identifier_char(cur_char)){
				std::string name(1, cur_char);
				cur_char = input.get();

				while (is_identifier_char(cur_char) || isdigit(cur_char)) {
				name += cur_char;
				cur_char = input.get();
				}

				DEBUG_STDOUT_TOKEN(name);

				if (name == "lambda"){
				return current_tok = TOKEN::LAMBDA;
				}
				else if (name == "if"){
				return current_tok = TOKEN::IF;
				}
				else if (name == "define"){
				return current_tok = TOKEN::DEFINE;
				}
				else if (name == "cond"){
				return current_tok = TOKEN::COND;
				}
				else if (name == "else"){
				return current_tok = TOKEN::ELSE;
				}

				current_identifer = name;
				return current_tok = TOKEN::IDENTIFIER;
				}
				else if (cur_char == EOF){
				return current_tok = TOKEN::TEOF;
				}
				else{
				DEBUG_STDOUT_TOKEN(cur_char);
				cur_char = input.get();
				return current_tok = TOKEN::UNKNOWN;
				}*/


	}
}

#endif //_MINISCHEME_LEXER_H
