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
#ifndef _FILTERLIST_H
#define _FILTERLIST_H

#include <string>
#include <set>
#include <iostream>

/*
 * There's no use denying this: The R-interpreter uses a lot of
 * calls - in c-scope-debugscopes as well as in R-contextscopes.
 *
 * Any attempt to visualize callcounts without filtering at least some
 * of the information is simply doomed.
 *
 * So we need a way to supply scopes/contexts to be filtered. This class
 * includes the proper data structures and functions to load a filterfile
 * and check againt the given filters while converting logfile to graph.
 */
namespace graphCreator{
  class filterlist{
  private:
    std::set<std::string> filters;
  public:
    //! true if string should not appear
    bool isFiltered(const std::string& checkEntry)const;
    
    static filterlist loadFromStream(std::istream& inStream);
    void printList(std::ostream& out)const;
    
    
    
    
    
    
    
  }; // class filterlist
  
  




} // namespace
 




#endif // _FILTERLIST_H
