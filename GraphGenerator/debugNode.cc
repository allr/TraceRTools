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

#include <string>

#include "debugNode.h"

static const std::string quoteChar="\"";

// creates a node
graphCreator::debugNode::debugNode(
  const unsigned int givenBirth, 
  const std::string& givenName
  ):
birth(givenBirth),
scopeName(givenName),
death(givenBirth) // to have this set to any value
{ /* all done in initlist */ }

void graphCreator::debugNode::setDeath(const unsigned int givenDeath){
  death = givenDeath;
}

std::string graphCreator::debugNode::getScopeName() const{ return scopeName;}
unsigned int graphCreator::debugNode::getBirth() const {return birth;}
unsigned int graphCreator::debugNode::getDeath() const {return death;}



void graphCreator::debugNode::writeBirthName(std::ostream& out)const{
  out << quoteChar << "node" << birth << quoteChar;
}


