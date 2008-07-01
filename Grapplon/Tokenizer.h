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
