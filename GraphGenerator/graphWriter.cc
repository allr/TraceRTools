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

#include "graphWriter.h"

static const std::string quoteChar="\"";

graphCreator::graphWriter2::graphWriter2(std::ostream& givenOut, const bool completeGraph)
: out(givenOut), verbosePrint(completeGraph)
{ /* no constructor body necessary? */ }

void graphCreator::graphWriter2::writeHeader(){/* nothing, more output in children */}
void graphCreator::graphWriter2::writeNodeVerbose(const graphCreator::debugNode& givenNode){/* nothing, more output in children */}
void graphCreator::graphWriter2::writeFooter(){/* nothing, more output in children */}
void graphCreator::graphWriter2::writeFlowVerbose(
  const graphCreator::debugNode& parent, 
  const graphCreator::debugNode& child, 
  const unsigned int givenTime,
  const graphCreator::flowDirection direction
  )
{/* nothing, more output in children */}
void graphCreator::graphWriter2::writeCallSummary(
  const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
  const graphCreator::flowDirection givenDirection
  )
{/* nothing, more output in children */}
void graphCreator::graphWriter2::writeAllFunctions(const std::set<std::string>& givenFunctionSet)
{/* nothing, more output in children */}

void graphCreator::graphWriter2::writeAllFunctions(
  const std::set<std::string>& givenFunctionSet,
  const std::map<std::string, unsigned int>& timerMap
  )
{
  // if not overwritten, just print functions without timer information
  writeAllFunctions(givenFunctionSet);
}


graphCreator::graphWriterDot::graphWriterDot(std::ostream& givenOut, const bool completeGraph)
:graphWriter2(givenOut,completeGraph)
{ /* no body needed */ }
  

void graphCreator::graphWriterDot::writeHeader(){
  out << "digraph g{" << std::endl;
  out << "graph [ rankdir = " << quoteChar << "LR" << quoteChar << "];" << std::endl;
}

void graphCreator::graphWriterDot::writeFooter(){
  out << "}" << std::endl;
}
  
void graphCreator::graphWriterDot::writeNodeVerbose(const graphCreator::debugNode& givenNode){
  if (!verbosePrint){ // verbose print not requested
    return;
  }
  unsigned int nodeBirth = givenNode.getBirth();
  unsigned int nodeDeath = givenNode.getDeath();
  using std::endl;
  givenNode.writeBirthName(out);
  /*
  out << quoteChar << "node" << birth << quoteChar << 
  */
  out << " [" << endl;
  out << "  label = " << quoteChar;
  writeLifeMarker(nodeBirth);
  out << givenNode.getScopeName() << " " ;
  { // a line for every tick of life
    unsigned int life_iterator;
    for (
      life_iterator  = nodeBirth+1;
      life_iterator <= nodeDeath;
      life_iterator++
      )
    {
      out << "| ";
      writeLifeMarker(life_iterator);
    }
  }
  out << quoteChar << endl;
  out << "  shape=" << quoteChar << "record" << quoteChar << endl;
  out << "];" << endl;
}
  
void graphCreator::graphWriterDot::writeFlowVerbose(
  const graphCreator::debugNode& parent, 
  const graphCreator::debugNode& child, 
  const unsigned int givenTime,
  const graphCreator::flowDirection direction
  )
{
  if (!verbosePrint){ // verbose not requested
    return;
  }
  parent.writeBirthName(out);
  out << ":" << quoteChar << givenTime << quoteChar;
  out << " -> ";
  child.writeBirthName(out);
  out << ":" << quoteChar << givenTime << quoteChar;
  out << ";" << std::endl;
}

void graphCreator::graphWriterDot::writeCallSummary(
  const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
  const graphCreator::flowDirection givenDirection
  )
{
  if (verbosePrint){ // do not write summary if we have already have detailed info
    return;
  }
  std::string edgeStyle;
  switch (givenDirection){
  case graphCreator::FLOW_FORWARD:  edgeStyle="solid";  break;
  case graphCreator::FLOW_RETURN:   edgeStyle="dashed"; break;
  case graphCreator::FLOW_LONGJMP:  edgeStyle="dottet"; break;
  } // switch
    
    
  using std::endl;
  out << "// starting " << edgeStyle << endl;
  std::map<std::pair<std::string,std::string>,unsigned int>::const_iterator mapIterator;
  for (
    mapIterator  = givenCallCounter.begin();
    mapIterator != givenCallCounter.end();
    mapIterator++
    )
  {
    const std::string& parent=(mapIterator->first).first;
    const std::string& child=(mapIterator->first).second;
    out << quoteChar << parent << quoteChar;
    out << " -> " ;
    out << quoteChar << child << quoteChar;
    out << " [label=" << quoteChar << std::dec << mapIterator->second << quoteChar << "]";
    out << " [style=" << quoteChar << edgeStyle << quoteChar << "]" << endl;
  } // for all calls
}

void graphCreator::graphWriterDot::writeLifeMarker(unsigned int date){
  out << "<" << std::dec << date << ">" << " ";
}


graphCreator::graphWriterFunctionNames::graphWriterFunctionNames(std::ostream& givenOut, const bool completeGraph)
:graphWriter2(givenOut,completeGraph)
{ /* no body needed */ }
void graphCreator::graphWriterFunctionNames::writeAllFunctions(
  const std::set<std::string>& givenFunctionSet
  )
{
  std::set<std::string>::const_iterator functionIterator;
  for(
    functionIterator  = givenFunctionSet.begin();
    functionIterator != givenFunctionSet.end();
    functionIterator++
    )
  {
    out << (*functionIterator) << std::endl;
  }
  
}




graphCreator::graphWriterGraphML::graphWriterGraphML(std::ostream& givenOut, const bool completeGraph)
:graphWriter2(givenOut,completeGraph), nodeCount(0), edgeCount(0)
{ /* no body needed */ }

std::string graphCreator::graphWriterGraphML::cleanupScopeName(std::string oldString){
  // at the moment, this removes '<' as those are "dangerous" for graphml/xml
  std::ostringstream convertStream;
  std::string::const_iterator stringIterator;
  for (
    stringIterator  = oldString.begin();
    stringIterator != oldString.end();
    stringIterator++
    )
  {
    switch (*stringIterator) {
    case '<':
      convertStream << "&lt;";
      break;

    case '>':
      convertStream << "&gt;";
      break;

    case '&':
      convertStream << "&amp;";
      break;

    default:
      convertStream << *stringIterator;
      break;
    }
  } // for all chars
  return convertStream.str();
}


void graphCreator::graphWriterGraphML::writeHeader(){
  using std::endl;
  { // standard xml preamble
    out << "<?xml version=" << quoteChar << "1.0" << quoteChar;
    out << " encoding=" << quoteChar << "UTF-8" << quoteChar;
    out << " standalone="<< quoteChar << "no" << quoteChar << "?>" << endl;
  }
  { // root element in graphml namespace
    out << "<graphml xmlns=" << quoteChar << "http://graphml.graphdrawing.org/xmlns" << quoteChar;
    out << " xmlns:xsi=" << quoteChar << "http://www.w3.org/2001/XMLSchema-instance" << quoteChar;
    out << " xmlns:y=" << quoteChar << "http://www.yworks.com/xml/graphml" << quoteChar;
    out << " xsi:schemaLocation=" << quoteChar << "http://graphml.graphdrawing.org/xmlns ";
    out << "  http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd"<<quoteChar << ">" << endl;
  }
  { // label key for nodes
    out << "<key for=" << quoteChar << "node" << quoteChar;
    out << " id=" << quoteChar << "nlabel" << quoteChar;
    out << " attr.name=" << quoteChar << "nodeLabel" << quoteChar;
    out << " attr.type=" << quoteChar << "string" << quoteChar << " />" << endl;
  }
  { // count key for edges
    out << "<key for=" << quoteChar << "edge" << quoteChar;
    out << " id=" << quoteChar << "eCount" << quoteChar;
    out << " attr.name=" << quoteChar << "callCount" << quoteChar;
    out << " attr.type=" << quoteChar << "int" << quoteChar << " />" << endl;
  }
  { // style/call direction key for edges
    out << "<key for=" << quoteChar << "edge" << quoteChar;
    out << " id=" << quoteChar << "eDir" << quoteChar;
    out << " attr.name=" << quoteChar << "callDirection" << quoteChar;
    out << " attr.type=" << quoteChar << "string" << quoteChar << " />" << endl;
  }
  { // graph start
    out << "<graph edgedefault=" << quoteChar << "directed" << quoteChar;
    out << " id=" << quoteChar << "G" << quoteChar << ">" << endl;
  }
}

void graphCreator::graphWriterGraphML::writeFooter(){
  using std::endl;
  { // end of graph tag
    out << "</graph>" << endl;
  }
  { // end of graphml file
    out << "</graphml>" << endl;
  }
}

void graphCreator::graphWriterGraphML::writeAllFunctions(
  const std::set<std::string>& givenFunctionSet
  )
{
  // simply use the other writer but with an empty map
  writeAllFunctions(
    givenFunctionSet,
    std::map<std::string,unsigned int>()
    );

}

void graphCreator::graphWriterGraphML::writeAllFunctions(
  const std::set<std::string>& givenFunctionSet,
  const std::map<std::string,unsigned int>& timerMap
  )
{
  using std::endl;
  std::set<std::string>::const_iterator functionIterator;
  for(
    functionIterator  = givenFunctionSet.begin();
    functionIterator != givenFunctionSet.end();
    functionIterator++
    )
  {
    std::string nodeID;
    const std::string& functionName = (*functionIterator);
    { // create nodeID for this function
      std::ostringstream convertStream;
      /*
       * reality check: 
       * - If there are no nodes, the first one becomes "n0"
       * - if there is one node, the next one becomes "n1"
       * full induction:
       * if there are n nodes present, the number n has not yet been used
       */
      convertStream << "n" << std::dec << nodeCount;
      nodeID = convertStream.str();
    }
    // store nodeID for later edge creation
    nodeIDs[functionName]=nodeID;
    
    out << "<node id=" << quoteChar << nodeID << quoteChar << ">" << endl;
    
    out << "<data key=" << quoteChar << "nlabel" << quoteChar << ">";
    out << cleanupScopeName(functionName);
    out << "</data>" << std::endl;
    
    if (timerMap.count(functionName)){
      out << "<data key=" << quoteChar << "nTime" << quoteChar << ">";
      out << std::dec << timerMap.at(functionName);
      out << "</data>" << std::endl;
    }
    
    out << "</node>" << std::endl;
    nodeCount++;
  }
}


  
  
void graphCreator::graphWriterGraphML::writeCallSummary(
  const std::map<std::pair<std::string,std::string>,unsigned int>& givenCallCounter,
  const graphCreator::flowDirection givenDirection
  )
{
  using std::endl;
  std::string dirString;
  switch (givenDirection){
  case graphCreator::FLOW_FORWARD:  dirString="forward";  break;
  case graphCreator::FLOW_RETURN:   dirString="return"; break;
  case graphCreator::FLOW_LONGJMP:  dirString="longjump"; break;
  } // switch


  std::map<std::pair<std::string,std::string>,unsigned int>::const_iterator mapIterator;
  for (
    mapIterator  = givenCallCounter.begin();
    mapIterator != givenCallCounter.end();
    mapIterator++
    )
  {
    const std::string& parent=(mapIterator->first).first;
    const std::string& child=(mapIterator->first).second;
    std::string edgeLabel;
    { // create a label for this edge
      std::ostringstream convertStream;
      convertStream << "e" << std::dec << edgeCount;
      edgeLabel = convertStream.str();
    }
    { // starting edge tag
      out << "<edge id=" << quoteChar << edgeLabel << quoteChar;
      // note: ID may not really be necessary
      out << " source=" << quoteChar << nodeIDs[parent] << quoteChar;
      out << " target=" << quoteChar << nodeIDs[child] << quoteChar << ">" << endl;
    }
    { // data: Number of calls
      out << "<data key=" << quoteChar << "eCount" << quoteChar << ">";
      out << mapIterator->second << "</data>" << endl;
    }
    { // data: call direction
      out << "<data key=" << quoteChar << "eDir" << quoteChar << ">";
      out << dirString << "</data>" << endl;
    }
    { // ending edge tag
      out << "</edge>" << endl;
    }
    edgeCount++;
  } // for all calls
}




