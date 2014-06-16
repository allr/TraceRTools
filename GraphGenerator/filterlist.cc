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
#include "filterlist.h"
#include <string>

bool graphCreator::filterlist::isFiltered(const std::string& checkEntry)const{
  if(filters.count(checkEntry)!=0){
    return true;
  }else{ // not in filterlist
    return false;
  }
}

graphCreator::filterlist graphCreator::filterlist::loadFromStream(std::istream& inStream){
  filterlist returnList;
  
  while (!inStream.eof()){
    std::string currentLine;
    std::getline(inStream,currentLine);
    { // clear whitespace
      std::string::iterator stringIterator;
      for (
        stringIterator  = currentLine.begin();
        stringIterator != currentLine.end();
        /* iteration done in loop */
        )
      {
        if (isspace(*stringIterator)){ // if whitespace
          // delete it
          stringIterator = currentLine.erase(stringIterator);
        }else{
          stringIterator++; // iterate;
        }
      } // iterate string
    } // clear whitespace
    if (currentLine.empty()){ // line is empty
      // ignore it
      continue;
    }
    if ('#' == currentLine.at(0)){ // use hash as comment lines
      // comment lines are to be ignored
      continue;
    }
    returnList.filters.insert(currentLine);
  } // while not eof
  
  return (returnList);
}

void graphCreator::filterlist::printList(std::ostream& out) const{
  std::set<std::string>::const_iterator filterIterator;
  for (
    filterIterator  = filters.begin();
    filterIterator != filters.end();
    filterIterator++
    )
  {
    out << (*filterIterator) << std::endl;
  }
}

