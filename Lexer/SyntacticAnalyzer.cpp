// Project 3

#ifndef Syntactic_Analyzer_H
#define Syntactic_Analyzer_H


#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "SymbolTable.cpp"

using namespace std;

const int ASSEMBLY_INSTRUCTIONS_ARRAY_SIZE = 1000; 


class SyntacticAnalyzer
{
public:
	// Constructor
	SyntacticAnalyzer()
	{
		tokenList.open("Token_List.txt", ios::in);
		parseTree.open("Parse_Tree.txt", ios::out);
		assemblyList.open("Assembly_Code.txt", ios::out);
		

		tokenList >> currentToken;
		tokenList >> currentLexeme;
		expectedInput = "";

	}

	~SyntacticAnalyzer()
	{
		tokenList.close();
		parseTree.close();
		assemblyList.close();
	}

	struct instruction {
		int address = 0;
		string operation = "";
		int operand = 0;
	};

	void OutputInstructionsToFile()
	{
		for (int i = 1; i < numberOfInstructions; ++i)
		{
			assemblyList << instruction_table[i].address << " ";
			assemblyList << instruction_table[i].operation << " ";
			if (instruction_table[i].operand >= 0)
			{
				assemblyList << instruction_table[i].operand;
			}
			else
			{
				//assemblyList << "NIL";
			}

			assemblyList << endl;
		}
	}

	void Generate_Instructions(string newOperation, int newOperand)
	{

		instruction_table[numberOfInstructions].address = numberOfInstructions;
		instruction_table[numberOfInstructions].operation = newOperation;
		instruction_table[numberOfInstructions].operand = newOperand;

		++numberOfInstructions;
	}

	void Back_Patch(int jumpAddress)
	{
		int address = Pop_Jumpstack();
		instruction_table[address].operand = jumpAddress;
	}

	int Pop_Jumpstack() {

		int result = 0;

		if (jumpStackNumOfElem <= 0)
		{
			cerr << "Error, Popping Jumpstack while empty!" << endl;
		}
		else
		{
			result = jumpStack[jumpStackNumOfElem - 1];
			--jumpStackNumOfElem;
		}

		return result;
	}

	void Push_Jumpstack(int address)
	{
		jumpStack[jumpStackNumOfElem] = address;
		++jumpStackNumOfElem;
	}

	// Starting Function
	void Begin()
	{
		cout << "Generating Parse Tree" << endl << endl << endl;
		Rat18S();

	}


	// Helper Functions
	void incrementParser()
	{

		parseTree << "\tToken: " << currentToken << "\tLexeme: " << currentLexeme << endl << endl;

		expectedInput = "";

		if (!tokenList.eof())
		{
			tokenList >> currentToken;
			tokenList >> currentLexeme;
		}
		else
		{
			parseTree << "FATAL ERROR! EXPECTED NEW INPUT, BUT END OF INPUT FILE REACHED! XD" << endl << endl;
		}
	}

	bool isIdentifier()
	{
		return (currentToken == "Identifier");
	}


	// Productions

	void Rat18S() 
	{
		parseTree << "<Rat18S> ::= <Opt Declaration List> <Statement List>" << endl << endl;
		
		if (currentLexeme == "%%")
		{
			incrementParser();
			OptDeclarationList();
			StatementList();

			OutputInstructionsToFile();
			symbolTable.print();
			parseTree << "Syntactic Parsing Complete." << endl << endl;

		}
		else
		{
			expectedInput = "%%";
			Error();
		}

	}


	void Qualifier()
	{
		parseTree << "<Qualifier> ::= int | boolean | real " << endl << endl;

		if (currentLexeme == "boolean" || currentLexeme == "int")
		{
			incrementParser();
		}
		else
		{
			expectedInput = "boolean or int";
			Error();
		}
	}

	void Body()
	{
		parseTree << "<Body> ::= { <Statement List> }" << endl << endl;

		if (currentLexeme == "{")
		{
			incrementParser();
			StatementList();

			if (currentLexeme == "}")
			{
				incrementParser();
			}
			else
			{
				expectedInput = "}";
				Error();
			}
		}
		else
		{
			expectedInput = "{";
			Error();
		}
	}

	void OptDeclarationList()
	{
		parseTree << "<Opt Declaration List> ::= <Declaration List> | <Empty>" << endl << endl;

		if (currentLexeme == "boolean" || currentLexeme == "int" || currentLexeme == "real")
		{
			DeclarationList();
		}
		else
		{
			Empty();
		}
	}

	void DeclarationList()
	{
		parseTree << "<Declaration List> ::= <Declaration> ; <Declaration List2>" << endl << endl;

		if (currentLexeme == "boolean" || currentLexeme == "int")
		{

			Declaration();

			if (currentLexeme == ";")
			{
				incrementParser();
				DeclarationList2();
			}
			else
			{	
				parseTree << "Error in DeclarationList()" << endl;
				expectedInput = ";";
				Error();

			}
		}
		else
		{
			expectedInput = "booleann, int, or real";
			Error();
		}
	}

	void DeclarationList2()
	{
		parseTree << "<Declaration List2> ::= <Declaration List> | <Empty>" << endl << endl;

		if (currentLexeme == "boolean" || currentLexeme == "int" || currentLexeme == "real")
		{
			DeclarationList();
		}
		else
		{
			Empty();
		}
	}

	void Declaration()
	{
		parseTree << "<Declaration> ::= <Qualifier> <DeclareIDs> " << endl << endl;

		if (currentLexeme == "boolean" || currentLexeme == "int")
		{
			
			string currentType = currentLexeme;
			Qualifier();
			DeclareIDs(currentType);
		}
		else
		{
			expectedInput = "boolean or int";
			Error();
		}
	}

	void DeclareIDs(string currentType)
	{
		parseTree << "<DeclareIDs> ::= <Identifier> <DeclareIDs2>" << endl << endl;

		if (isIdentifier())
		{
			symbolTable.insert(currentLexeme, currentType);
			incrementParser();
			DeclareIDs2(currentType);
		}
		else
		{
			expectedInput = "<Identifier>";
			Error();
		}
	}

	void DeclareIDs2(string currentType)
	{
		parseTree << "<IDs2> ::= , <DeclareIDs> | <Empty>" << endl << endl;

		if (currentLexeme == ",")
		{
			incrementParser();
			DeclareIDs(currentType);
		}
		else
		{
			Empty();
		}
	}

	void IDs()
	{
		parseTree << "<IDs> ::= <Identifier> <IDs2>" << endl << endl;

		if (isIdentifier())
		{
			Generate_Instructions("PUSHM", symbolTable.getAddress(currentLexeme));
			incrementParser();
			IDs2();
		}
		else
		{
			expectedInput = "<Identifier>";
			Error();
		}
	}

	void IDs2()
	{
		parseTree << "<IDs2> ::= , <IDs> | <Empty>" << endl << endl;

		if (currentLexeme == ",")
		{
			incrementParser();
			IDs();
		}
		else
		{
			Empty();
		}
	}

	void StatementList()
	{
		parseTree << "<Statement List> ::= <Statement> <Statement List2>" << endl << endl;

		if (isIdentifier() || currentLexeme == "{" || currentLexeme == "if" || currentLexeme == "return" || currentLexeme == "put" || currentLexeme == "get" || currentLexeme == "while")
		{
			Statement();
			StatementList2();
		}
		else
		{
			expectedInput = "<Identifier>, {, if, return, put, get, or while";
			Error();
		}
	}

	void StatementList2()
	{
		parseTree << "<Statement List2> ::= <Statement List> | <Empty>" << endl << endl;

		if (isIdentifier() || currentLexeme == "{" || currentLexeme == "if" || currentLexeme == "return" || currentLexeme == "put" || currentLexeme == "get" || currentLexeme == "while")
		{
			StatementList();
		}
		else
		{
			Empty();
		}
	}

	void Statement()
	{
		parseTree << "<Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>" << endl << endl;

		if (isIdentifier())
		{
			Assign();
		}
		else if (currentLexeme == "{")
		{
			Compound();
		}
		else if (currentLexeme == "if")
		{
			If();
		}
		else if (currentLexeme == "return")
		{
			Return();
		}
		else if (currentLexeme == "put")
		{
			Print();
		}
		else if (currentLexeme == "get")
		{
			Scan();
		}
		else if (currentLexeme == "while")
		{
			While();
		}
		else
		{
			parseTree << "STATEMENT() ERROR!" << endl << endl;
			expectedInput = "<Statement>";
			Error();
		}
	}

	void Compound()
	{
		parseTree << "<Compound> ::= { <Statement List> }" << endl << endl;

		if (currentLexeme == "{")
		{
			incrementParser();
			StatementList();

			if (currentLexeme == "}")
			{
				incrementParser();
			}
			else
			{
				expectedInput = "}";
				Error();
			}
		}
		else
		{
			expectedInput = "{";
			Error();
		}

	}

	void Assign()
	{
		parseTree << "<Assign> ::= <Identifier> = <Expression> ;" << endl << endl;

		if (isIdentifier())
		{
			globalCurrentType = "";
			string savedLexeme = currentLexeme;
			string savedType = symbolTable.getType(savedLexeme);
			//symbolTable.insert(save);

			incrementParser();

			if (currentLexeme == "=")
			{
				incrementParser();
				Expression();

				if (savedType == globalCurrentType)
				{
					Generate_Instructions("POPM", symbolTable.getAddress(savedLexeme));

					if (currentLexeme == ";")
					{

						incrementParser();
					}
					else
					{
						expectedInput = ";";
						Error();
					}
				}
				else
				{
					string temp = "boolean";

					if (savedLexeme == "boolean")
					{
						temp = "int";
					}

					expectedInput = "Integer Boolean Mismatch! Found: " + savedLexeme + " " + savedType + " which cannot be assigned type " + temp;
					Error(expectedInput);
				}
			}
			else
			{
				expectedInput = "=";
				Error();
			}
		}
		else
		{
			expectedInput = "<Identifier>";
			Error();
		}
	}

	void If()
	{
		parseTree << "<If> ::= if ( <Condition> ) <Statement> <If2>" << endl << endl;

		if (currentLexeme == "if")
		{
			int address = numberOfInstructions;
			incrementParser();

			if (currentLexeme == "(")
			{
				incrementParser();
				Condition();

				if (currentLexeme == ")")
				{
					incrementParser();
					Statement();
					If2();
					Back_Patch(numberOfInstructions);
					
				}
				else
				{
					expectedInput = ")";
					Error();
				}
			}
			else
			{
				expectedInput = "(";
				Error();
			}
		}
		else
		{
			expectedInput = "if";
			Error();
		}
	}

	void If2()
	{
		parseTree << "<If2> ::= endif | else <Statement> endif" << endl << endl;

		if (currentLexeme == "endif")
		{
			incrementParser();
		}
		else if (currentLexeme == "else")
		{
			incrementParser();
			Statement();

			if (currentLexeme == "endif")
			{
				incrementParser();
			}
		}
		else
		{
			parseTree << "Error! <If2> Failed." << endl;
			expectedInput = "endif, or else";
			Error();
		}
	}

	void Return()
	{
		parseTree << "<Return> ::= return <Return2>" << endl << endl;

		if (currentLexeme == "return")
		{
			incrementParser();
			Return2();
		}
		else
		{
			expectedInput = "return";
			Error();
		}

	}

	void Return2()
	{
		parseTree << "<Return2> ::= ; | <Expression> ;" << endl << endl;

		if (currentLexeme == ";")
		{
			incrementParser();
		}
		else if (isIdentifier() || currentLexeme == "-" || currentToken == "Integer" ||
			currentToken == "Real" || currentLexeme == "(" || currentLexeme == "true" ||
			currentLexeme == "false")
		{
			Expression();

			if (currentLexeme == ";")
			{
				incrementParser();
			}
			else
			{
				expectedInput = ";";
				Error();
			}
		}
	}

	void Print()
	{
		parseTree << "<Print> ::= put ( <Expression> ) ;" << endl << endl;

		if (currentLexeme == "put")
		{
			incrementParser();

			if (currentLexeme == "(")
			{
				incrementParser();

				Expression();

				if (currentLexeme == ")")
				{
					incrementParser();
					Generate_Instructions("STDOUT", -1);

					if (currentLexeme == ";")
					{
						incrementParser();
					}
					else
					{
						expectedInput = ";";
						Error();
					}
				}
				else
				{
					expectedInput = ")";
					Error();
				}
			}
			else
			{
				expectedInput = "(";
				Error();
			}
		}
		else
		{
			expectedInput = "put";
			Error();
		}
	}

	void Scan() /////  Limitation? Can only use get with one ID?
	{
		parseTree << "<Scan> ::= get ( <IDs> );" << endl << endl;

		if (currentLexeme == "get")
		{
			Generate_Instructions("STDIN", -1);
			incrementParser();

			if (currentLexeme == "(")
			{
				incrementParser();

				if (isIdentifier())
				{
					Generate_Instructions("POPM", symbolTable.getAddress(currentLexeme));
					incrementParser();

					while (currentLexeme == ",")
					{
						incrementParser();
						Generate_Instructions("STDIN", -1);
						Generate_Instructions("POPM", symbolTable.getAddress(currentLexeme));
						incrementParser();
					}

				}
				else
				{
					expectedInput = "Identifier";
					Error();
				}

				if (currentLexeme == ")")
				{
					incrementParser();

					if (currentLexeme == ";")
					{
						incrementParser();
					}
					else
					{
						expectedInput = ";";
						Error();
					}
				}
				else
				{
					expectedInput = ")";
					Error();
				}
			}
			else
			{
				expectedInput = "(";
				Error();
			}
		}
		else
		{
			expectedInput = "get";
			Error();
		}
	}

	void While()
	{
		parseTree << "<While> ::= while ( <Condition> ) <Statement> " << endl << endl;

		if (currentLexeme == "while")
		{
			int address = numberOfInstructions;
			Generate_Instructions("LABEL", -1);
			incrementParser();

			if (currentLexeme == "(")
			{
				incrementParser();

				Condition();

				if (currentLexeme == ")")
				{
					incrementParser();
					Statement();
					Generate_Instructions("JUMP", address);
					Back_Patch(numberOfInstructions);

				}
				else
				{
					expectedInput = ")";
					Error();
				}
			}
			else
			{
				expectedInput = "(";
				Error();
			}
		}
		else
		{
			expectedInput = "while";
			Error();
		}
	}

	void Condition()
	{
		parseTree << "<Condition> ::= <Expression> <Relop> <Expression>" << endl << endl;

		if (isIdentifier() || currentLexeme == "-" || currentToken == "Integer" ||
			currentToken == "Real" || currentLexeme == "(" || currentLexeme == "true" ||
			currentLexeme == "false")
		{
			Expression();

			string lexeme = currentLexeme;
			incrementParser();

			Expression();

			//////////////////////////////////////////

			if (lexeme == "<") {
				Generate_Instructions("LES", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else if (lexeme == ">") {
				Generate_Instructions("GRT", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else if (lexeme == "==") {
				Generate_Instructions("EQU", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else if (lexeme == "=<") {
				Generate_Instructions("LEQ", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else if (lexeme == "=>") {
				Generate_Instructions("GEQ", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else if (lexeme == "^=") { // bitwise xor assignment
				Generate_Instructions("NEQ", -1);
				Push_Jumpstack(numberOfInstructions); // maybe
				Generate_Instructions("JUMPZ", 999);
			}
			else
			{
				expectedInput = "valid <Relop> input";
				Error();
			}

			//////////////////////////////////////////

		}
		else
		{
			expectedInput = "valid <Expresion> input";
			Error();
		}
	}

	void Expression()
	{
		parseTree << "<Expression> ::= <Term> <Expression4>" << endl << endl;

		if (isIdentifier() || currentLexeme == "-" || currentToken == "Integer" || 
			currentLexeme == "(" || currentLexeme == "true" || currentLexeme == "false")
		{
			Term();
			Expression4();
		}

	}

	void Expression2()
	{
		parseTree << "<Expression2> ::= + <Term> <Expression2> | <Empty>" << endl << endl;

		if (currentLexeme == "+")
		{
			incrementParser();
			Term();
			Generate_Instructions("ADD", -1);
			Expression4();
		}
		else
		{
			Empty();
		}
	}

	void Expression3()
	{
		parseTree << "<Expression3> ::= - <Term> <Expression3> | <Empty>" << endl << endl;

		if (currentLexeme == "-")
		{
			incrementParser();
			Term();
			Generate_Instructions("SUB", -1);
			Expression4();
		}
		else
		{
			Empty();
		}
	}

	void Expression4()
	{
		parseTree << "<Expression4> ::= <Expression2> | <Expression3> | <Empty>" << endl << endl;

		if (currentLexeme == "+")
		{
			Expression2();
		}
		else if (currentLexeme == "-")
		{
			Expression3();
		}
		else
		{
			Empty();
		}
	}

	void Term()
	{
		parseTree << "<Term> ::= <Factor> <Term4>" << endl << endl;

		if (isIdentifier() || currentLexeme == "-" || currentToken == "Integer" ||
			currentToken == "Real" || currentLexeme == "(" || currentLexeme == "true" ||
			currentLexeme == "false")
		{
			Factor();
			Term4();
		}

	}

	void Term2()
	{
		parseTree << "<Term2> ::= * <Factor> <Term2> | <Empty>" << endl << endl;

		if (currentLexeme == "*")
		{
			incrementParser();
			Factor();
			Generate_Instructions("MUL", -1);
			Term4();
		}
		else
		{
			Empty(); 
		}

	}

	void Term3()
	{
		parseTree << "<Term3> ::= / <Factor> <Term3> | <Empty>" << endl << endl;

		if (currentLexeme == "/")
		{
			incrementParser();
			Factor();
			Generate_Instructions("DIV", -1);
			Term4();
		}
		else
		{
			Empty();
		}
	}

	void Term4()
	{
		parseTree << "<Term4> ::= <Term2> | <Term3> | <Empty>" << endl << endl;

		if (currentLexeme == "*")
		{
			Term2();
		}
		else if (currentLexeme == "/")
		{
			Term3();
		}
		else
		{
			Empty();
		}
	}

	void Factor()
	{
		parseTree << "<Factor> ::= - <Primary> | <Primary>" << endl << endl;

		if (isIdentifier() || currentLexeme == "-" || currentToken == "Integer" || currentLexeme == "(" 
			|| currentLexeme == "true" || currentLexeme == "false")
		{
			// Ignore 
			if (currentLexeme == "-")
			{
				incrementParser();
			}
			else if (isIdentifier())
			{
				//Generate_Instructions("PUSHM", symbolTable.getAddress(currentLexeme));

			}
			
			Primary();
		}

	}

	void Primary()
	{
		parseTree << "<Primary> ::= <Identifier> <Primary2> | <Integer> | ( <Expression> ) " << endl << endl;
		int convertedString;

		if (currentToken == "Integer" || currentLexeme == "true" || currentLexeme == "false")
		{
			if (currentToken == "Integer")
			{
				globalCurrentType = "int";
				stringstream stringToInt(currentLexeme);
				stringToInt >> convertedString;
			}
			else
			{
				globalCurrentType = "boolean";
				
				if (currentLexeme == "true")
				{
					convertedString = 1;
				}
				else if (currentLexeme == "false")
				{
					convertedString = 0;
				}
			}

			Generate_Instructions("PUSHI", convertedString);
			incrementParser();
		}
		else if (isIdentifier())
		{
			///
			Generate_Instructions("PUSHM", symbolTable.getAddress(currentLexeme));
			globalCurrentType = symbolTable.getType(currentLexeme);
			incrementParser();
			Primary2();
		}
		else if (currentLexeme == "(")
		{
			incrementParser();
			Expression();

			if (currentLexeme == ")")
			{
				incrementParser();
			}
			else
			{
				parseTree << "Error! Invalid token in <Primary>. Expected ) but found " << currentLexeme << endl;
			}
		}
		else
		{
			parseTree << "Error! Invalid token in <Primary>." << endl;
		}

	}

	void Primary2()
	{
		parseTree << "<Primary2> ::= ( <IDs> ) | <Empty> " << endl << endl;

		if (currentLexeme == "(")
		{
			incrementParser();
			IDs();

			if (currentLexeme == ")")
			{
				incrementParser();
			}
		}
		else
		{
			Empty();
		}
	}

	void Empty()
	{
		parseTree << "<Empty> ::= epsilon" << endl << endl;

	}

	void Error()
	{
		parseTree << endl;
		parseTree << "*** FATAL ERROR! Expected \"" << expectedInput << "\" but found \"" << currentLexeme << "\" ***" << endl << endl;
		expectedInput = "";
	}

	void Error(string errMsg)
	{
		parseTree << "*** FATAL ERROR! " << errMsg << " ***" << endl << endl;
		expectedInput = "";
	}

private:
	instruction instruction_table[ASSEMBLY_INSTRUCTIONS_ARRAY_SIZE];
	int numberOfInstructions = 1;

	int jumpStack[1000];
	int jumpStackNumOfElem = 0;

	string globalCurrentType = "";

	SymbolTable symbolTable;
	string currentToken;
	string currentLexeme;
	string expectedInput;

	fstream tokenList;
	fstream parseTree;
	fstream assemblyList;

};

#endif // !Syntactic_Analyzer_H