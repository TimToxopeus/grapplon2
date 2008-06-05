/************************************************************************************
*	Assignment 5 - The maze of despair												*
*	Tim Toxopeus - 3206947															*
*	Cetin Demir - 3236943															*
************************************************************************************/
#pragma once
#include <string>
#include <vector>

// String tokenizer
class CTokenizer
{
public:
	CTokenizer();
	~CTokenizer();

	std::vector<std::string> GetTokens( std::string szString, std::string delimiters = " ,;[]:" );
};
