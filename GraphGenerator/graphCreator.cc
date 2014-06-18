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

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility> // make_pair

#include <cstdlib>

#include "graphCreator.h"
#include "timercsvreader.h"
#include "mapfilereader.h"

// iterates over complete file
int graphCreator::main(const graphCreator::cmdLineOptions& myOptions){
  using std::endl; using std::cout;
  
  std::map<std::string, unsigned int> timerMap;
  if ("NULL" != myOptions.timerFile)
  { // create timerMap
    std::ifstream inFile(myOptions.timerFile.c_str());
    if (!inFile.is_open()){
      std::cerr << "could not open timerfile" << std::endl;
      return EXIT_FAILURE;
    }
    timerMap = graphCreator::timerExtract(inFile);
    
  } // create timerMap
  
  
  //! \todo: make print_timerextract a cmdline-option instead of preprocessor-macro 
  //#define PRINT_TIMEREXTRACT 
  #undef PRINT_TIMEREXTRACT
  #ifdef PRINT_TIMEREXTRACT
  { // print timerMap - if wanted
    cout << " --- TimerMap: " << endl;
    
    std::map<std::string, unsigned int>::const_iterator mapIterator;
    for (
      mapIterator  = timerMap.begin();
      mapIterator != timerMap.end();
      mapIterator++
      )
    {
      cout << "Scope: " << mapIterator->first;
      cout << " ; Count: " << mapIterator->second; 
      cout << std::endl;
    }
    cout << " --- End TimerMap" << endl;
  }
  #endif // PRINT_TIMEREXTRACT
    
  std::multimap<std::string,std::string> timer2ScopeMap;
  if ("NULL" != myOptions.mappingFile)
  { // read timerMapping
    std::ifstream inFile(myOptions.mappingFile.c_str());
    if (!inFile.is_open()){
      std::cerr << "could not open mappingFile" << std::endl;
      return EXIT_FAILURE;
    }
    timer2ScopeMap = graphCreator::timerMapExtract(inFile);
  } // read timerMapping
    
  //! \todo: move print_mappingExtract to cmdline-option - like print_timerextract
  //#define PRINT_MAPPINGEXTRACT
  #undef PRINT_MAPPINGEXTRACT
  #ifdef PRINT_MAPPINGEXTRACT
  { // print mappingmap - if wanted
    std::cout << " --- mappingMap: " << endl;
    std::multimap<std::string,std::string>::const_iterator mapIterator;
    for (
      mapIterator  = timer2ScopeMap.begin();
      mapIterator != timer2ScopeMap.end();
      mapIterator++
      )
    {
      std::cout << mapIterator->first  << "->";
      std::cout << mapIterator->second << std::endl;
    } // for all entries in map
    std::cout << " -- end mappingMap" << endl; 
  }
  #endif // PRINT_MAPPINGEXTRACT
  
  
  
  
  
  // prepare input stream for reading
  std::ifstream inFile(myOptions.debugScopesFile.c_str());
  //std::ostream& out(std::cout);
  
  if (!inFile.is_open()){
    std::cerr << "File: " << myOptions.debugScopesFile << " could not be opened for reading" << std::endl;
    return (EXIT_FAILURE);
  }

  // prepare graph writer for output
  
  std::ofstream outStream(myOptions.graphOutputFile.c_str());
  if (!outStream.is_open()){
    std::cerr << "Output file " << myOptions.graphOutputFile << " could not be opened for writing" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::ofstream contextOutFile(myOptions.contextGraphOutputFile.c_str());
  if (!contextOutFile.is_open()){
    std::cerr << "Output file " << myOptions.contextGraphOutputFile << " could not be opened for writing" << std::endl;
    return EXIT_FAILURE;
  }
  
  //graphWriterFunctionNames myGraphWriter(std::cout,false);
  graphCreator::graphWriterGraphML myGraphWriter(outStream,false);
  graphCreator::graphWriterGraphML myContextGraphWriter(contextOutFile, false);
  //graphWriterDot myGraphWriter(std::cout,false);
  //graphWriter2 myGraphWriter(std::cout);

  /*
   * general idea:
   *
   * while not eof
   * getline(stream,string)
   * clear crlf / whitespace from end of line
   * get type of line (enter, exit, longjump, other)
   * -> ENTER: (this is the only time we take the string
   * --> ENTER via longjump:
   * <- EXIT:
   *
   * Update:
   * contextScopes use
   * -> doSomething -> soSomething"
   * so no ENTER-keyword is used.
   */
  graphCreator::callState callStater(myGraphWriter);
  
  std::map<std::string,unsigned int> mergedMap;
  { // merging the maps
    /*
     * General idea:
     * for each of the known names in the timermap,
     * check whether other names have been supplied
     * in the mappingmap. 
     *
     * If yes, write (add) the numbers to the new names
     * If no, write (add) the number to the old name
     *
     * Note: This work even if one of the maps is empty.
     * However, it may be quicker to implement an explicit check
     * (and potential skip)
     */
    std::map<std::string,unsigned int>::const_iterator timerIterator;
    for (
      timerIterator  = timerMap.begin();
      timerIterator != timerMap.end();
      timerIterator++
      )
    { // for all entry in the timermap
      const std::string& timerName = timerIterator->first;
      if(timer2ScopeMap.count(timerName)==0){ // no mapping
        if(mergedMap.count(timerName)==0){ // not yet existed
          mergedMap[timerName] = timerIterator->second; // set
        }else{ // already existed
          mergedMap[timerName] += timerIterator->second; // add
        }
      }else{ // at least one exists
        // iterate them
        std::multimap<std::string,std::string>::const_iterator scoperIterator;
        for(
          scoperIterator  = timer2ScopeMap.lower_bound(timerName);
          scoperIterator != timer2ScopeMap.upper_bound(timerName);
          scoperIterator++
          )
        { // iterate scopes with the given timerName
          const std::string& scoperName = scoperIterator->second;
          if(mergedMap.count(scoperName)==0){ // not yet existed
            mergedMap[scoperName] = timerIterator->second; // set
          }else{ // already existed
            mergedMap[scoperName] += timerIterator->second; // add
          }
        } // iterate scopes with the given timerName
      } // given timerName is mapped
    } // for all entrys in the timermap.
  } // merging maps
  
  // Again: This could and should be a command line option
  #define PRINT_MERGERESULTS
  //#undef PRINT_MERGERESULTS
  #ifdef PRINT_MERGERESULTS
  {
    std::cout << " --- MergedMap" << std::endl;
    std::map<std::string,unsigned int>::const_iterator mergedIterator;
    for (
      mergedIterator  = mergedMap.begin();
      mergedIterator != mergedMap.end();
      mergedIterator++
      )
    {
      std::cout << "* " << mergedIterator->first;
      std::cout << " -> " << mergedIterator->second << std::endl;
    }
    std::cout << " --- End MergedMap" << std::endl;
  }
  #endif // PRINT_MERGERESULTS
  
  callStater.setTimerMap(timerMap);
  
  graphCreator::arcCounter myArcCounter(myContextGraphWriter);
  while(!inFile.eof()){ // convert the input file line by line
    std::string currentLine;
    std::getline(inFile,currentLine);
    size_t keyword_finder;
    { // check for ENTER
      const std::string keyphrase_ENTER = "] -> ENTER: ";  
      keyword_finder = currentLine.find(keyphrase_ENTER);
      if (std::string::npos != keyword_finder){ // found it!
        std::string scopeName = currentLine.substr(keyword_finder + keyphrase_ENTER.size());
        callStater.enterFunction(scopeName);
        continue; // next line
      }
    }
    { // check for EXIT
      keyword_finder = currentLine.find("] <- EXIT: ");
      if (std::string::npos != keyword_finder){ // found it!
        callStater.exitFunction();
        continue; // next line
      }
    }
    { // check for ENTER via longjump
      keyword_finder = currentLine.find("] <-- ENTER via longjump: ");
      if (std::string::npos != keyword_finder){ // found it!
        /*
         * reality check
         *
         * 0123456
         * [57] ->
         * search returns 3, number is two digit long, starts at 1
         */
        std::string targetScopeString = currentLine.substr(1,keyword_finder-1);
        unsigned int targetScope;
        {
          std::istringstream convertStream(targetScopeString);
          convertStream >> std::dec >> targetScope;
        }
        callStater.longJumpBack(targetScope);
        continue; // next line
      }
    }
    { // check for variable assignment
      keyword_finder = currentLine.find("DEBUGASSIGN: ");
      if(0 == keyword_finder){ // found it - at beginning of line
        size_t dividerPos = currentLine.find(" := ");
        if (std::string::npos == dividerPos){
          std::cerr << "Assignment Fail in line: " << currentLine << std::endl;
          continue; // do not process line any further
        }
        size_t startOfVariableName = 13; // length of "DEBUGASSIGN: "
        size_t lenOfVariableName = dividerPos-13;
        size_t startOfVariableValue = dividerPos+4;
        size_t lenOfVariableValue = currentLine.length()-startOfVariableValue;
        
        // do some reality check here
        
        std::string variableName = currentLine.substr(13);
        std::string variableValue;
        
        std::cout << "<!-- Assignment: " << variableName << " := " << variableValue << "-->" << std::endl;
      } // if found DEBUGASSIGN
    } // check for variable assignment 
    { // check for opening context
      // "-> doSomething -> soSomething"
      const std::string keyphrase = "-> ";
      keyword_finder = currentLine.find(keyphrase);
      if(0 == keyword_finder){ // found it - has to be beginning of line
        std::string callLine = currentLine.substr(keyword_finder+keyphrase.size());
        // find next occurance
        const std::string dividerString = " -> ";
        keyword_finder = callLine.find(dividerString);
        if (std::string::npos != keyword_finder){
          std::string oldScopeName = callLine.substr(0,keyword_finder);
          std::string newScopeName = callLine.substr(keyword_finder + dividerString.size());
          myArcCounter.enterFunction(oldScopeName,newScopeName);
          //enterFunction(cleanupScopeName(newScopeName));
          continue;
        }
      }
    }
    { // check for closing context
      // <- <Anonymous> <- <Anonymous>
      const std::string keyPhrase = "<- ";
      keyword_finder = currentLine.find(keyPhrase);
      if (0 == keyword_finder){ // found it at beginning of line
        //callStater.exitFunction();
        continue;
      }
    } // check for closing context
        
    // seems to be general comment - nothing wrong with that.
  }
  inFile.close(); // finished working with the source file
  
  /*
   * please note:  
   *
   * The closing curly braces end the scope for callStater,
   * thus the destructor is called - containing the 
   * writing functions (or the calls to the appropiate graphwriter
   */
}

bool graphCreator::extractCmdOption(
  const std::string& argValue, 
  const std::string& searchString,
  std::string& targetString
  )
{
  unsigned int searchStringLength=searchString.size();
  if (
    (0==argValue.find(searchString)) &&
    (searchStringLength < argValue.size())
    )
  {
    targetString = argValue.substr(searchStringLength);
  }
}

graphCreator::cmdLineOptions graphCreator::extractCmdLine(int argc, char* argv[]){
  // note: this is very simple and only allows for "--foo=value" passing
  cmdLineOptions returnValue;
  { // set default values
    returnValue.timerFile = "NULL";
    returnValue.debugScopesFile = "/dev/stdin";
    returnValue.mappingFile = "NULL";
    returnValue.graphOutputFile = "/dev/stdout";
    returnValue.contextGraphOutputFile = "/dev/null";
  }
  
  
  // #define DEBUG_CMDLINEEXTRACTION
  #undef DEBUG_CMDLINEEXTRACTION
  #ifdef DEBUG_CMDLINEEXTRACTION
  {
    std::cout << "argument count: " << std::dec << argc << std::endl;
  }
  #endif // DEBUG_CMDLINEEXTRACTION
  unsigned int curArgIndex;
  for (
    curArgIndex = 1; // zero is command name and can be skipped
    curArgIndex < argc;
    curArgIndex++
    )
  { // for all arguments
    std::string curArg = argv[curArgIndex];
    #ifdef DEBUG_CMDLINEEXTRACTION
    {
      std::cout << "Arg "<<std::dec << curArgIndex << " : " << curArg << std::endl;
    }
    #endif // DEBUG_CMDLINEEXTRACTION
    { // check for help request
      if("--help"==curArg){
        printHelp(argv[0]);
        exit(EXIT_SUCCESS);
      }
    }
    { // check for timerFile
      graphCreator::extractCmdOption(
        curArg,
        "--timerFile=",
        returnValue.timerFile
        );
    }
    { // check for inputFile
      graphCreator::extractCmdOption(
        curArg,
        "--inputFile=",
        returnValue.debugScopesFile
        );
    }
    { // check for mappingfile
      graphCreator::extractCmdOption(
        curArg,
        "--mappingFile=",
        returnValue.mappingFile
        );
    }
    { // check for outputfile
      graphCreator::extractCmdOption(
        curArg,
        "--outFile=",
        returnValue.graphOutputFile
        );
    }
    { // check for context graph output file
        graphCreator::extractCmdOption(
          curArg,
          "--contextOut=",
          returnValue.contextGraphOutputFile
          );
    } // graph output file?
  } // for all arguments
  //#define DEBUG_CMDLINEEXTRACTION
  #undef DEBUG_CMDLINEEXTRACTION
  #ifdef DEBUG_CMDLINEEXTRACTION
  {
    std::cout << "--- extracted CmdLineOptions:" << std::endl;
    std::cout << "Timerfile: " << returnValue.timerFile << std::endl;
    std::cout << "Inputfile: " << returnValue.debugScopesFile << std::endl;
    std::cout << "Mappingfile: " << returnValue.mappingFile << std::endl;
    std::cout << "Outputfile: " << returnValue.graphOutputFile << std::endl;
    std::cout << "--- extracted CmdLineOptions (end)" << std::endl;
  }
  #endif // DEBUG_CMDLINEEXTRACTION
  return returnValue;
}

void graphCreator::printHelp(const std::string& programName){
  using std::cout; using std::endl;
  cout << "Usage: " << programName << " [..Options..]" << endl;
  cout << "  --inputFile=<file>       Use <file> as input (/dev/stdin)" << endl;
  cout << "  --outFile=<file>         Write output to <file> (/dev/stdout)" << endl;
  cout << "  --contextOut=<file>      Write context output to <file> (/dev/null)" << endl;
  cout << "  --timerFile=<file>       Read TimeR-table from <file> (NULL)" << endl;
  cout << "  --mappingFile=<file>     Read timerR->Scope-Mapping from <file>" << endl;
  cout << "  --filterFile=<file>      Read Filters from <file>" << endl;
  cout << endl;
  cout << "  --help                   You just used it; you should know what it does" << endl;
  cout << endl;
}

int main(int argc, char* argv[]){
  using std::cout; using std::endl;
  
  graphCreator::cmdLineOptions myOptions = graphCreator::extractCmdLine(
    argc,argv
    );
  
  graphCreator::main(myOptions);
   
   
  return EXIT_SUCCESS; 
}

graphCreator::callState::callState(graphCreator::graphWriter2& givenGraphWriter) 
: myGraphWriter(givenGraphWriter), currentTime(0){
  myGraphWriter.writeHeader();
}
graphCreator::callState::~callState(){
  while(debugStack.size()>0){ // some scopes still active - kill them! kill them with fire!
    /*
     * this is needed as nodes are not printed before they are dead.
     * In some early-termination-cases this means that some nodes 
     * will not be printed - except for if we remember them, here
     */
    graphCreator::debugNode terminatedNode = debugStack.top();
    debugStack.pop();
    terminatedNode.setDeath(currentTime);
    myGraphWriter.writeNodeVerbose(terminatedNode);
  }
  
  myGraphWriter.writeAllFunctions(calledFunctions);
  
  myGraphWriter.writeCallSummary(callCounter_forward,graphCreator::FLOW_FORWARD);
  //myGraphWriter.writeCallSummary(callCounter_backward,FLOW_RETURN);
  myGraphWriter.writeCallSummary(callCounter_longjump,graphCreator::FLOW_LONGJMP);
  
  
  // end of graph structure
  myGraphWriter.writeFooter();
}  

void graphCreator::callState::countCall(
  std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
  const std::string& parent, 
  const std::string& child
  )
{
  std::pair<std::string,std::string> callParticipants;
  callParticipants = std::make_pair<std::string, std::string> (parent, child);
  if (0==givenCallCounter.count(callParticipants)){ // did not yet exist
    givenCallCounter[callParticipants] = 1; // one - this one
  }else{ // exists
    givenCallCounter[callParticipants]++; // one more
  }
}

void graphCreator::callState::noteFlow(
  const graphCreator::debugNode& parent, 
  const graphCreator::debugNode& child, 
  const unsigned int givenTime,
  const graphCreator::flowDirection direction
  )
{
  myGraphWriter.writeFlowVerbose(parent,child,givenTime,direction);
  { // insert this call into the callcounter-map
    switch (direction){
    case graphCreator::FLOW_FORWARD: 
      countCall(callCounter_forward,parent.getScopeName(),child.getScopeName()); 
      break;
    case graphCreator::FLOW_RETURN: 
      countCall(callCounter_backward,parent.getScopeName(),child.getScopeName()); 
      break;
    case graphCreator::FLOW_LONGJMP:
      countCall(callCounter_longjump,parent.getScopeName(),child.getScopeName()); 
      break;
    }// switch
  }
  
}

void graphCreator::callState::enterFunction(const std::string& givenName){
  graphCreator::debugNode newFunction(currentTime, givenName); // create the new one
  if (debugStack.size()>0){ // if there is a parent, note the call
    graphCreator::debugNode parentFunction = debugStack.top();
    noteFlow(parentFunction,newFunction,currentTime);
  }
  // in any case: push the new function
  debugStack.push(newFunction);
  // and note that we've seen this function during the course of the program
  calledFunctions.insert(givenName);
  // and this is an event - time passes
  currentTime++;
}

void graphCreator::callState::exitFunction(){
  if (debugStack.empty()){ // this should not happen
    std::cerr << "Trying to exit function - but no function is known.." << std::endl;
    return;
  }
  graphCreator::debugNode dyingFunction= debugStack.top();
  debugStack.pop();
  dyingFunction.setDeath(currentTime);
  if (debugStack.size()>0){ // if there is a parent, note the return
    graphCreator::debugNode parentFunction = debugStack.top();
    noteFlow(dyingFunction,parentFunction,currentTime,graphCreator::FLOW_RETURN);
  }
  // as the node has died, it needs to be printed before it is forgotten
  myGraphWriter.writeNodeVerbose(dyingFunction);
  
  // and this is an event - time passes
  currentTime++;
}

void graphCreator::callState::longJumpBack(const unsigned int targetDepth){
  if (debugStack.empty()){ // this should not happen
    std::cerr << "Trying to exit function - but no function is known.." << std::endl;
    return;
  }
  graphCreator::debugNode jumpingFunction = debugStack.top();
  if (targetDepth == debugStack.size()-1){ // don't end any scopes - stay local
    noteFlow(jumpingFunction,jumpingFunction,currentTime,graphCreator::FLOW_LONGJMP);
  }else{
    // real jump
    debugStack.pop();
    // kill the scopes in-between
    /*
     * Reality-check:
     *
     * I am in scope 2 (stacksize is 3, 2 after pop).
     * I want to jump to scope 0 (for which stacksize is 1)
     * 
     * So if stacksize is (targetdepth+1), this is the target.
     * if stacksize is targetdepth+2ormore, this is an intermediate scope
     */  
    while(targetDepth < debugStack.size()-1){ // intermediate scope
      graphCreator::debugNode skippedFunction = debugStack.top();
      debugStack.pop();
      // as the function is skipped, it is dead by now - it died before
      skippedFunction.setDeath(currentTime-1);
      myGraphWriter.writeNodeVerbose(skippedFunction);
    }
    { // now, targetDepth = debugStack.size()-1
      graphCreator::debugNode parentFunction = debugStack.top();
      // don't pop! scope stays active.
      
      // note return to this function
      noteFlow(jumpingFunction,parentFunction,currentTime,graphCreator::FLOW_LONGJMP);
    }
    jumpingFunction.setDeath(currentTime);
    // with that, the jumping function is gone - print its information
    myGraphWriter.writeNodeVerbose(jumpingFunction);
  }
  // and this is an event - time passes
  currentTime++;
}
  
void graphCreator::callState::setTimerMap(const std::map<std::string, unsigned int> givenMap){
  timerMap = givenMap;
}
  
graphCreator::arcCounter::arcCounter(graphCreator::graphWriter2& givenGraphWriter)
: myGraphWriter(givenGraphWriter){
  myGraphWriter.writeHeader();
}

graphCreator::arcCounter::~arcCounter(){

  myGraphWriter.writeAllFunctions(calledFunctions);
  myGraphWriter.writeCallSummary(callCounter_forward,graphCreator::FLOW_FORWARD);
  
  // end of graph structure
  myGraphWriter.writeFooter();
}  

void graphCreator::arcCounter::enterFunction(
  const std::string& from,
  const std::string& to
  )
{
  std::pair<std::string,std::string> callParticipants;
  callParticipants = std::make_pair<std::string, std::string> (from, to);
  if (0==callCounter_forward.count(callParticipants)){ // did not yet exist
    callCounter_forward[callParticipants] = 1; // one - this one
  }else{ // exists
    callCounter_forward[callParticipants]++; // one more
  }
  calledFunctions.insert(from); calledFunctions.insert(to);
}
  
  
  



