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
#include <sstream>

#include "timercsvreader.h"

std::map<std::string, unsigned int> graphCreator::timerExtract(
  std::istream& inStream
  )
{
  using std::endl;
  std::map<std::string, unsigned int> returnMap;
  // search for "# --- individual timers"
  std::string currentLine;
  unsigned int counter=0;
  while (true){
    if (inStream.eof()){
      std::cerr << "no individual timers found in file" << std::endl;
      return returnMap;
    }
    if (0 == currentLine.find("# --- individual timers")){
      /*
       * note: we could also scan for the complete "magic string"
       *
       * # --- individual timers	self_percentage	self	total	calls	aborts	has_bcode
       */
      break;
    }
    std::getline(inStream,currentLine);
    counter++;
    
    //#define TRACERREADER_PRINTPRELINES
    #undef TRACERREADER_PRINTPRELINES
    #ifdef TRACERREADER_PRINTPRELINES
    {
      std::cout << counter << ": " << currentLine << std::endl;
    }
    #endif // TRACERREADER_PRINTPRELINES
  }
  while (!inStream.eof()){
    std::getline(inStream,currentLine);
    if (currentLine.empty()){
      //std::cout << "empty line" << endl;
      return returnMap;
    }
    /*
     * each row now has n columns:
     * name
     * self percentage
     * self
     * total
     * calls
     * aborts
     * has_bcode
     *
     * I think that name and self is most important.
     * However, self percentage may be chosen instead
     *
     * Values are tab-seperated.
     *
     * A real tokenizer could be a nice idea; however, it seems like slightly overkill.
     *
     * Tracer has methods to read the timer-files and convert it into 
     * SQLite; this seems like overkill, too.
     */
    std::string& lineRemainder = currentLine;
    std::string scopeName;
    { // get the name
      size_t firstTabPos = lineRemainder.find("\t");
      if (std::string::npos == firstTabPos){
        std::cerr << "Problem in reading timerFile: First Tab not found" << endl;
        return returnMap;
      }
      /*
       * "blabla\tfoo";
       *  01234566789
       * find is 6, first six ("blabla") are before tab
       * foo starts at 7 = 6+1;
       */
      scopeName = lineRemainder.substr(0,firstTabPos);
      if (lineRemainder.length() < firstTabPos+1){
        std::cerr << "Problem in reading timerFile: No info after first tab" << endl;
        return returnMap;
      }
      lineRemainder = lineRemainder.substr(firstTabPos+1);
    }
    { // ignore the percentage
      size_t secondTabPos = lineRemainder.find("\t");
      if (std::string::npos == secondTabPos){
        std::cerr << "Problem in reading timerFile: second Tab not found" << endl;
        return returnMap;
      }
      lineRemainder = lineRemainder.substr(secondTabPos+1);
    }
    unsigned int scopeTime;
    { // get the self count
      size_t thirdTabPos = lineRemainder.find("\t");
      if (std::string::npos == thirdTabPos){
        std::cerr << "Problem in reading timerFile: third Tab not found" << endl;
        return returnMap;
      }
      std::string scopeTimeAsString = lineRemainder.substr(0,thirdTabPos);
      { // convert scopeTime to int;
        std::istringstream convertStream(scopeTimeAsString);
        convertStream >> std::dec >> scopeTime;
      }
    }
    returnMap[scopeName] = scopeTime;
  }
  return returnMap;
}



