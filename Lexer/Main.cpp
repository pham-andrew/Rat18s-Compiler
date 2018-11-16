/*
// Jason Hasselle & Andrew Pham
//
// Project 3
//
// Thank you to Dr. Choi for the (vital) partial solutions to project 3
// Thank you to Prof. Bein for all general instruction of the project.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

#include "SyntacticAnalyzer.cpp"

using namespace std;

void lexer(string inputString, fstream& outputStream);
bool isSymbol(char c);
bool isKeyword(string s);
string getSymbolToken(char c);

int main() {

	fstream inputStream;
	inputStream.open("Input.txt", ios::in);
	fstream outputStream;
	outputStream.open("Token_List.txt", ios::out);

	bool isComment = 0;
	string inputString;

	while (inputStream >> inputString) {

		if (inputString == "!") {
			isComment = !isComment;
			if (isComment == true) {
				cout << "Comment Skipped" << endl;
			}
		}
		else if (isComment == 0) {
			lexer(inputString, outputStream);
		}
	}

	inputStream.close();
	outputStream.close();

	SyntacticAnalyzer parser;
	parser.Begin();

	cout << "Testing Complete." << endl << endl;

	return 0;
}

void lexer(string inputString, fstream& outputStream) {

	stringstream isstream(inputString);

	char currentChar = '0';
	string returnLexeme = "";
	string returnToken = "";
	int state = 0;

	/*
	States of the DFSM (can change later):
	0 = Start
	1 = Identifier
	2 = Integer
	3 = Real
	4 = Completed
	5 = Fatal Error

	*/

	while (isstream.peek() != EOF) {

		isstream >> currentChar;
		currentChar = tolower(currentChar);

		switch (state) {

		case 0: //Starting State
			returnLexeme += currentChar;
			if (isalpha(currentChar)) {
				state = 1;
				returnToken = "Identifier";
			}
			else if (isdigit(currentChar)) {
				state = 2;
				returnToken = "Integer";
			}
			else if (currentChar == int('.')) {
				if (isstream.peek() == EOF) {
					state = 5;
				}
				else if (isdigit(isstream.peek())) {
					state = 3;
				}
				else {
					state = 5;
				}
			}
			else if (currentChar == '$') {
				state = 5;
			}
			else if (isSymbol(currentChar)) {

					if (currentChar == '^' && isstream.peek() == '=') {
						returnLexeme += isstream.peek();
						isstream.get();
					}

					if (currentChar == '=') {
						if (isstream.peek() == '=' || isstream.peek() == '<' || isstream.peek() == '>')
						{
							returnLexeme += isstream.peek();
							isstream.get();
						}
					}
					state = 0;
					returnToken = getSymbolToken(currentChar);

					if (currentChar == '%' && isstream.peek() == '%') {
						returnLexeme += currentChar;
						isstream.get();
					}
					outputStream << returnToken << " " << returnLexeme << endl;
					returnToken = "";
					returnLexeme = "";
				//}

			}
			break;

		case 1: //Identifier

			if (isalpha(currentChar)) {
				state = 1;
			}
			else if (isdigit(currentChar)) {
				if (isstream.eof()) {
					state = 5;
				}
				else {
					state = 1;
				}
			}
			else if (currentChar == int('.')) state = 5;
			else if (currentChar == '$') {
				if (isalnum(isstream.peek()))
					state = 5;
				else
					state = 1;
			}
			else if (isSymbol(currentChar)) {
				state = 4;
			}
			if (state == 1) {
				returnLexeme += currentChar;
				returnToken = "Identifier";
			}
			break;

		case 2: //Integer
			if (isdigit(currentChar)) {
				state = 2;
				returnLexeme += currentChar;
				returnToken = "Integer";
			}
			else if (!isdigit(currentChar)) {
				if (currentChar == int('.')) {
					state = 5;
					returnLexeme += currentChar;
					returnToken = "Real";
				}
				else if (isSymbol(currentChar))
					state = 4;
				else
					state = 5;
			}
			break;

		case 3: //Floating Point

			if (isdigit(currentChar)) {
				returnLexeme += currentChar;
				state = 3;
				returnToken = "Real";
			}
			else  if (!isdigit(currentChar))
			{
				if (currentChar == int('.'))
					state = 5;
				else
					state = 4;
			}
			break;

		case 4: //Completed
			break;

		default: //Fatal Error
			outputStream << "***Fatal Error!*** : currentChar = " << currentChar << " returnLexeme = "
				<< returnLexeme << endl;
			cout << "***Fatal Error!*** : currentChar = " << currentChar << " returnLexeme = "
				<< returnLexeme << endl;

			if (returnToken == "Real") {
				outputStream << "Type REAL is not supported!" << endl;
				cout << "Type REAL is not supported!" << endl;
			}

			cout << "Enter any key to continue: " << endl;

			return;
			break;
		} // end of switch

		if (state == 4) {
			if (returnToken == "Identifier" && isKeyword(returnLexeme)) {
				returnToken = "Keyword";
			}
			outputStream << returnToken << " " << returnLexeme << endl;
			state = 0;
			returnToken = "";
			returnLexeme = "";
			isstream.putback(currentChar);
		}

	} // end of while
	if (state != 4 && state != 0 && state != 5) {
		if (returnToken == "Identifier" && isKeyword(returnLexeme)) {
			returnToken = "Keyword";
		}
		outputStream << returnToken << " " << returnLexeme << endl;
	}

}

bool isSymbol(char c) {
	bool returnBool = false;
	switch (c) {
	case ',':
	case ';':
	case ':':
	case '{':
	case '}':
	case '(':
	case ')':
	case '[':
	case ']':
	case '%':
	case '/,':
	case '<':
	case '>':
	case '=':
	case '+':
	case '-':
	case '/':
	case '*':
	case '^':
		returnBool = true;
		break;
	default:
		break;
	}
	return returnBool;
}

string getSymbolToken(char c) {
	string returnToken = "";
	switch (c) {
	case ',':
	case ';':
	case ':':
	case '{':
	case '}':
	case '(':
	case ')':
	case '[':
	case ']':
	case '%':
		returnToken = "Separator";
		break;
	case '<':
	case '>':
	case '=':
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
		returnToken = "Operator";
		break;
	default:
		returnToken = "Error, unknown symbol token!";
		break;
	}
	return returnToken;
}

// Reason for long if statement: strings cannot be used in a switch statement in C++, not without enums, which seemed like a hassle.
bool isKeyword(string s) {
	return (s == "if" || s == "else" || s == "while" || s == "for" || s == "return" || s == "get"
		|| s == "put" || s == "int" || s == "function" || s == "boolean" || s == "endif" || s == "true" || s == "false");
}

