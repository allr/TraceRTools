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
#ifndef _GRAPHCREATOR_H
#define _GRAPHCREATOR_H

#include <stack>
#include <map>
#include <set>

#include "debugNode.h"
#include "graphWriter.h"

namespace graphCreator{
  
  
  
  void writeLifeMarker(std::ostream& out, unsigned int date);
  
  class callState{
  private:
    //! number of calls from parent (first) to child (second)
    std::map<std::pair<std::string,std::string>,unsigned int> callCounter_forward;
    std::map<std::pair<std::string,std::string>,unsigned int> callCounter_backward;
    std::map<std::pair<std::string,std::string>,unsigned int> callCounter_longjump;
    std::stack<graphCreator::debugNode> debugStack;
    //! functions that were seen at least once
    std::set<std::string> calledFunctions;
    /*!
     * \brief the graph write to use while converting the debug information
     *
     * This member contains a reference to a graph writer to be used while
     * converting/traversing the debug information.
     *
     * Why? Why not collect all information in one step and then pass it to 
     * a graphwriter?
     * One early idea was to do exactly this.
     * This idea later was scrapped as there is a lot of information
     * (e.g. calls, ended scopes) that better is written while traversing
     * instead of saving - there is a colossal amount of scopes which would just
     * consume more and more memory.
     */
    graphCreator::graphWriter2&         myGraphWriter;
    
    std::map<std::string, unsigned int> timerMap; 
    unsigned int     currentTime;
    //! this can be used for all directions and even longjumps!
    void noteFlow(
      const graphCreator::debugNode& parent, 
      const graphCreator::debugNode& child, 
      const unsigned int givenTime,
      const graphCreator::flowDirection = graphCreator::FLOW_FORWARD
      );
    void countCall(
      std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
      const std::string& parent, 
      const std::string& child
      );
  public:
    callState(graphCreator::graphWriter2& givenGraphWriter);
    ~callState();
    void setTimerMap(const std::map<std::string, unsigned int> givenMap);
    void enterFunction(const std::string& givenName);
    void exitFunction();
    void longJumpBack(const unsigned int targetDepth);
    
    void printContracted();
  }; // class callState
  
  // just count arcs - without time or state
  class arcCounter{
  private:
    std::map<std::pair<std::string,std::string>,unsigned int> callCounter_forward;
    graphCreator::graphWriter2& myGraphWriter;
    //! functions that were seen at least once
    std::set<std::string> calledFunctions;
  public:
    void enterFunction(const std::string& from, const std::string& to);
    arcCounter(graphCreator::graphWriter2& givenGraphWriter);
    ~arcCounter();
  }; // class arcCounter
    

  //! Struct containing everything that can be passed or switched via cmdline
  struct cmdLineOptions{
    std::string timerFile;
    std::string debugScopesFile;
    std::string mappingFile;
    std::string graphOutputFile;
    std::string contextGraphOutputFile;
  };
  
  /*!
   * \brief check whether given argument contains the requested option
   *
   * This function checks whether the passed argument containts the requested
   * option and tries to extract it. This only works for --longoptions=.
   * Return Value is true if the argument was found and false, if not.
   * This can be used to check whether one argument is known at all
   * (by checking it against all expected searchstrings and ORing the results
   *
   * If the argument is not found, the targetString remains unchanged.
   * This allows for default options of operator preference.
   */
  bool extractCmdOption(
    const std::string& argValue, 
    const std::string& searchString,
    std::string& targetString
    );
  
  /*!
   * \brief extract cmdlineOptions from cmdline
   *
   * This function takes the standard argc/argv-combination
   * from the environment and extracts everything that can be passed
   * via cmdline from it.
   */
  cmdLineOptions extractCmdLine(int argc, char* argv[]);
  
  //! print short help on command line options
  void printHelp(const std::string& programName);
  
  
  //! inner main
  int main(const cmdLineOptions& myOptions);
};





//! outer main
int main(int argc, char* argv[]);


#endif // _GRAPHCREATOR_H
