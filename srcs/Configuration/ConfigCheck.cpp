#include "ConfigCheck.hpp"

ConfigCheck::ConfigCheck(void)
{
	return ;
}

std::string	ConfigCheck::checkFile(std::string path)
{
	if (path.empty())
		throw std::invalid_argument("File path doesnt exist");

	if ((access(path.c_str(), F_OK) != 0))
		throw std::invalid_argument("File doesnt exist");

	std::ifstream	file(path.c_str());

	if (!file.is_open())
		throw std::invalid_argument("File cant be opened");

	std::stringstream	buffer;
	std::string 		line;

	while(std::getline(file, line))
	{
		line = line.erase(line.find_last_not_of(" \n") + 1);
		if (line.find_first_not_of(" \n") != std::string::npos)
		{
			line = line.substr(line.find_first_not_of(" \n"));
			buffer << line << "\n";
		}
	}

	if (!file.eof())
		throw std::ifstream::failure("Error reading file");

	file.close();

	if (file.is_open())
		throw std::ifstream::failure("Error closing file");

	return (buffer.str());
}

/*════════════════════ public ════════════════════════════════════════════════════════════*/

ConfigCheck::ConfigCheck(std::string path) : path(path)
{
	file_content = checkFile(path);
	return ;
}

ConfigCheck::ConfigCheck(ConfigCheck const &copy)
{
	*this = copy;
	return ;
}

ConfigCheck::~ConfigCheck(void)
{
	return ;
}

ConfigCheck	&ConfigCheck::operator=(ConfigCheck const &copy)
{
	file_content = copy.file_content;
	path = copy.path;
	return *this;
}

std::string	ConfigCheck::getFileContent(void) const
{
	return (file_content);
}
