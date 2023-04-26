#include "parsing.hpp"
#include <fstream>
#include <ios>

const std::string ParsingError(std::string error) throw()
{
	return ("parsing error: " + error + "\n");
}
const std::vector<server> parse_conf_file(const std::string path)
{
	if (path.find(".conf") + 5 != path.length())
		throw (ParsingError("configuration file must have \".conf\" extension."));
	std::ifstream conf_file(path.c_str(), std::ios_base::in);
	if (!conf_file.is_open())
		throw (ParsingError("can't open configuration file."));
	std::vector<server> servers;

	conf_file.close();
	return (servers);
}
