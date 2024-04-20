#ifndef CONFIGCHECK_HPP
# define CONFIGCHECK_HPP

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

class ConfigCheck
{
	private:
			ConfigCheck(void);
			std::string		checkFile(std::string path);
			std::string 	file_content;
			std::string		path;

	public:
			ConfigCheck(std::string path);
			ConfigCheck(ConfigCheck const &copy);
			~ConfigCheck(void);
			ConfigCheck &operator=(ConfigCheck const &copy);
			std::string	getFileContent(void) const;
};

#endif
