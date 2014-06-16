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

#ifndef _DEBUGNODE_H
#define _DEBUGNODE_H

#include <iostream>

namespace graphCreator{
  /*!
   * \brief debugNodes store information about a single scope each.
   *
   * Objects of the class debugNode are used to store information about
   * a single scope and it's runtime (limited by birth and death).  
   * 
   * Note:
   * A debugNode should be created for one *instance* (run) of a function, not
   * for the function itself. If a function is called several times, this 
   * should result in several debugNodes - they are "born" at a different time.
   * They can have different depths and can even give birth to each other
   * (recursive use of a function).
   *
   * This also means that most of the time, several debugNodes are "alive" at 
   * the same time (in different depth). Only the debugNode with the
   * latest birthDate is the one currently processed.
   *
   * Special care has to be taken when using longjmp as this results in 
   * debugNodes not having an official death. Still they are not to be
   * considered "alive". Program flow can never return to them.
   */
  class debugNode{
  private:
    unsigned int birth;
    unsigned int death;
    std::string scopeName;
    unsigned int depth;
  public:
    debugNode(const unsigned int givenBirth, const std::string& givenName);
    void setDeath(const unsigned int givenDeath);
    std::string getScopeName() const;
    /*!
     * \brief write node as birthname to given stream
     *
     * This function writes the node as simple birthtime ("node1234")
     * to the given stream.
     * This is useful to have the same function as different entities -
     * only one function can be "born" at the same time
     */
    void writeBirthName(std::ostream& out)const;
    unsigned int getBirth()const;
    unsigned int getDeath()const;
    
  }; // class debugNode
  
  //! different arc directions.
  enum flowDirection{
    FLOW_FORWARD,
    FLOW_RETURN,
    FLOW_LONGJMP
  };
  
}; // namespace graphCreator


#endif // _DEBUGNODE_H


