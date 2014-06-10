/* used online tutorial http://www.dreamincode.net/forums/topic/183191-create-a-simple-configuration-file-parser/ to build the simple config parser */

//#include <iostream>
//#include <string>
//#include <sstream>
//#include <map>
//#include <fstream>

#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include "cfgfile.h"


// This function removes all comments from the configuration file
	
void CfgFile::removeComment(std::string &line) const
{
   	  if (line.find("//") != line.npos)
     	  line.erase(line.find("//"));
}


// This function returns true when a line only contains whitespaces 
	 
bool CfgFile::onlyWhitespace(const std::string &line) const
{
     	  return (line.find_first_not_of(' ') == line.npos);
}


// This function checks if a line is a valid key/value pair (key = value <value2> <value3> ...)
	
bool CfgFile::validLine(const std::string &line) const
{
  	  std::string temp = line;
    	  temp.erase(0, temp.find_first_not_of("\t "));			// This removes all whitespace and tabs from the beginning of the line
    	  if (temp[0] == '=')						// if the first character now is '=' it means no key was given so the line is invalid
	  {
	    return false;
 	  }

	  for (size_t i = temp.find('=') + 1; i < temp.length(); i++)	// loop through string starting after "= " 
	  {
	    if (temp[i] != ' ')						// if a whitespace character is found it means we've found at least one value ...
	    {
	      return true;						// ... which means its a valid line so return true
	    }
	  }

	  return false;
}


// This function extracts the key from a string
	
void CfgFile::extractKey(std::string &key, size_t const &sepPos, const std::string &line) const
{
	  key = line.substr(0, sepPos);					// sepPos is the position of the key/value seperator '='

	  if (key.find('\t') != line.npos || key.find(' ') != line.npos)
	  {
	    key.erase(key.find_first_of("\t "));
	  }
}


// This function extracts the value(s)
	
void CfgFile::extractValue(std::string &value, size_t const &sepPos, const std::string &line) const
{
   	  value = line.substr(sepPos + 1);				// sepPos is the position of the key/value seperator '='. +1 because we want to skip the trailing whitespace
    	  value.erase(0, value.find_first_not_of("\t "));
    	  value.erase(value.find_last_not_of("\t ") + 1);
}



void CfgFile::extractContents(const std::string &line)
{
    	  std::string temp = line;

	  temp.erase(0, temp.find_first_not_of("\t "));
	  size_t sepPos = temp.find('=');
  	
          std::string key, value;
	  extractKey(key, sepPos, temp);
	  extractValue(value, sepPos, temp);
 
	  if (!keyExists(key))
	  {
     	    contents.insert(std::pair<std::string, std::string>(key, value));
	  } else
	  {
      	    perror("CFG: Can only have unique key names!\n");
	  }

}


void CfgFile::parseLine(const std::string &line, size_t const lineNo)
{
    	  if (line.find('=') == line.npos)
	  {
            perror("CFG: Couldn't find separator on line\n");
 	  }

	  if (!validLine(line))
    	  {
            perror("CFG: Bad format for line\n");
	  }

	    extractContents(line);
	}

	void CfgFile::ExtractKeys()
	{
    	  std::ifstream file;
    	  file.open(fName.c_str());
    	  if (!file)
          {
            perror("Configuration file couldn't be found!\n");
            exit(EXIT_FAILURE);
          }
    	  std::string line;
    	  size_t lineNo = 0;

	  while (std::getline(file, line))
    	  {
            lineNo++;
            std::string temp = line;
 
            if (temp.empty())
              continue;

            removeComment(temp);

            if (onlyWhitespace(temp))
              continue;
 
            parseLine(temp, lineNo);
          }
 
    	  file.close();
} 


CfgFile::CfgFile(const std::string &fName)
{
   	  this->fName = fName;
    	  ExtractKeys();
}

bool CfgFile::keyExists(const std::string &key) const
{
    	  return contents.find(key) != contents.end();
}

std::string CfgFile::getValueOfKey(const std::string &key) const
{
    if (!keyExists(key))
        return "";
 
    return contents.find(key)->second;
}

