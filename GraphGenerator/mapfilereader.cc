/*
 * ------------------------------------------------------------------
 * This material is distributed under the GNU General Public License
 * Version 2. You may review the terms of this license at
 * http://www.gnu.org/licenses/gpl-2.0.html
 *
 * Copyright (c) 2014, Markus Kuenne
 * TU Dortmund University
 * ------------------------------------------------------------------
 */

#include "mapfilereader.h"

std::multimap<std::string,std::string> graphCreator::timerMapExtract(
  std::istream& inStream)
{
  using std::endl;
  std::multimap<std::string,std::string> returnMap;
  std::string currentLine;
  while(!inStream.eof()){
    std::getline(inStream,currentLine);
    
    { // clear all whitespace characters from the string
      std::string::iterator stringIterator;
      for (
        stringIterator  = currentLine.begin();
        stringIterator != currentLine.end();
        /* iteration done in loop */
        )
      {
        if (isspace(*stringIterator)){ // is whitespace
          // delete the sign
          stringIterator = currentLine.erase(stringIterator);
        }else{
          stringIterator++;
        }
      } // iterate string
    } // clear whitespace
    
    
    if (currentLine.empty()){ // line is empty
      // we have no problem with this, we simply skip it
      continue;
    }
    if ('#' == currentLine.at(0)){ // this is a comment line
      // and is perfectly valid
      continue;
    }
    std::string dividerSign="->";
    size_t dividerPos = currentLine.find(dividerSign);
    if (std::string::npos == dividerPos){ // no divider found in line
      // no comment, not empty, no assignment - this is fishy
      std::cerr << "Line: " << currentLine << " has no mapping" << std::endl;
      continue;
    }
    if (currentLine.length() < dividerPos+1){
      std::cerr << "Problem in reading timerfile: Assignment target missing" << endl;
      std::cerr << "Evil line: " << currentLine << std::endl;
      continue;
    }
    std::string timerName = currentLine.substr(0,dividerPos);
    std::string scopeName = currentLine.substr(dividerPos+dividerSign.length());
    
    std::pair<std::string,std::string> newEntry;
    newEntry.first =  timerName;
    newEntry.second = scopeName;
    returnMap.insert(newEntry);
  } // while not end of file
  return returnMap;
} // timerMapExtract




