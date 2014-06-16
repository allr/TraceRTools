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

#ifndef _GRAPHWRITER_H
#define _GRAPHWRITER_H

#include <map>
#include <set>

#include "debugNode.h"

namespace graphCreator{
  /*! 
   * \brief graph writer, writing null to output, to be extended
   *
   * This graph writer is the parent for all other graph writers
   * and happily does nothing.
   *
   * Originally, this was an abstract class but it seemed a better idea
   * to have an empty writer and then implement the changes in inheriting
   * classes
   */
  class graphWriter2{
  protected:
    std::ostream& out;
    bool verbosePrint;
  public:
    graphWriter2(std::ostream& givenOut, const bool completeGraph=false);
    //! If the output format requires a header, write it
    virtual void writeHeader();
    virtual void writeNodeVerbose(const graphCreator::debugNode& givenNode);
    virtual void writeFlowVerbose(
      const graphCreator::debugNode& parent, 
      const graphCreator::debugNode& child, 
      const unsigned int givenTime,
      const graphCreator::flowDirection direction
      ) ;
    virtual void writeCallSummary(
      const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
      const graphCreator::flowDirection givenDirection=graphCreator::FLOW_FORWARD
      );
    virtual void writeAllFunctions(const std::set<std::string>& givenFunctionSet);
    virtual void writeAllFunctions(
      const std::set<std::string>& givenFunctionSet,
      const std::map<std::string,unsigned int>& timerMap
      );
      
    //! if the output format requires a footer, write it
    virtual void writeFooter();
  };
  
  //! simple "graph" writer - only listing functions seen (testing purposes)
  class graphWriterFunctionNames : public graphWriter2{
  public:
    graphWriterFunctionNames(std::ostream& givenOut, const bool completeGraph=false);
    void writeAllFunctions(const std::set<std::string>& givenFunctionSet);
  }; // graphWriterFunctionNames
  
  class graphWriterDot : public graphWriter2 {
  private:
    //! write a life marker (needed for the block-type look of function scopes
    void writeLifeMarker(const unsigned int date);
      
  public:
    graphWriterDot(std::ostream& givenOut, const bool completeGraph=false);
    void writeHeader();
    void writeNodeVerbose(const graphCreator::debugNode& givenNode);
    void writeFlowVerbose(
      const graphCreator::debugNode& parent, 
      const graphCreator::debugNode& child, 
      const unsigned int givenTime,
      const graphCreator::flowDirection direction
      ) ;
    void writeCallSummary(
      const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
      const graphCreator::flowDirection givenDirection=graphCreator::FLOW_FORWARD
      );
    void writeFooter();
  };
  
  class graphWriterGraphML : public graphWriter2{
  private:
    //! map for each function and it's node IDs (needed for edges)
    std::map<std::string, std::string> nodeIDs;
    
    unsigned int nodeCount; //!< number of currently stored nodes
    unsigned int edgeCount; //!< number of currently stored edges
    std::string cleanupScopeName(std::string oldString);
    
  public:
    graphWriterGraphML(std::ostream& givenOut, const bool completeGraph=false);
    void writeHeader();
    
    void writeAllFunctions(const std::set<std::string>& givenFunctionSet);
    void writeAllFunctions(
      const std::set<std::string>& givenFunctionSet,
      const std::map<std::string,unsigned int>& timerMap
      );

    void writeCallSummary(
      const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
      const graphCreator::flowDirection givenDirection
      );
    
    void writeFooter();
  }; // class graphWriterGraphML
  
  
  
}; // namespace graphCreator






#endif // _GRAPHWRITER_H
