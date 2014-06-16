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

#ifndef _MAPFILEREADER_H
#define _MAPFILEREADER_H

#include <string>
#include <map>
#include <iostream>


namespace graphCreator{

  std::multimap<std::string,std::string> timerMapExtract(std::istream& inStream);
}











#endif // _MAPFILEREADER_H
