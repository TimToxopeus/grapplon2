/************************************************************************************
*	Assignment 5 - The maze of despair												*
*	Tim Toxopeus - 3206947															*
*	Cetin Demir - 3236943															*
************************************************************************************/
#include "Tokenizer.h"

#include <iostream>
using namespace std;

CTokenizer::CTokenizer()
{
}

CTokenizer::~CTokenizer()
{
}

vector <string> CTokenizer::GetTokens(string szString, string delimiters)
{
	vector<string> tokens;

	// Get the first non-delimiter position
    string::size_type lastPos = szString.find_first_not_of(delimiters, 0);
	// Find the first delimiter position
    string::size_type pos     = szString.find_first_of(delimiters, lastPos);

	string token = "";
	bool inQuotes = false;

	// While not at the end of the string
    while (string::npos != pos || string::npos != lastPos)
    {
		// Get a substring
		string subString = szString.substr(lastPos, pos - lastPos);

		// Check quotes to make sure we turn those into one token
		if ( subString[0] == '\"' )
		{
			inQuotes = true;
			if ( subString[subString.size() - 1] == '\"' )
			{
				inQuotes = false;
				tokens.push_back(subString.substr(1,subString.size()-2));
			}
			else
				token = subString.substr(1, subString.size()-1);
		}
		else if ( subString[subString.size() - 1] == '\"' )
		{
			inQuotes = false;
			token += " " + subString.substr(0, subString.size()-1);
			tokens.push_back( token );
			token = "";
		}
		else
		{
			if ( inQuotes )
			{
				token += " " + subString;
			}
			else
			{
				tokens.push_back(subString);
			}
		}

		// Go to the next token
        lastPos = szString.find_first_not_of(delimiters, pos);
        pos = szString.find_first_of(delimiters, lastPos);
    }

	return tokens;
}
