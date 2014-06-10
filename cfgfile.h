#ifndef CFGFILE_H_
#define CFGFILE_H_


#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

class CfgFile
{
/* This is the main class that parses the configuration file */

private:

	std::map<std::string, std::string> contents;
	std::string fName;


	// This function removes all comments from the configuration file
	
	void removeComment(std::string &line) const;


	// This function returns true when a line only contains whitespaces 
	 
	bool onlyWhitespace(const std::string &line) const;

	// This function checks if a line is a valid key/value pair (key = value <value2> <value3> ...)
	
	bool validLine(const std::string &line) const;

	// This function extracts the key from a string
	
	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const;

	// This function extracts the value(s)
	
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const;

	void extractContents(const std::string &line);

	void parseLine(const std::string &line, size_t const lineNo);

	void ExtractKeys();

public:

	CfgFile(const std::string &fName);

	bool keyExists(const std::string &key) const;

        std::string getValueOfKey(const std::string &key) const;

};


#endif /* CFGFILE_H_ */
