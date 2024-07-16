#include "../includes/webh.hpp"

std::vector<std::string> split(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream token_stream(str);

	while (std::getline(token_stream, token, delimiter))
		tokens.push_back(token);
	return tokens;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos)
	{
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start, end));
	return tokens;
}

std::string cpp_itoa(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
}
