# ------------------------------------------------------------------
# This material is distributed under the GNU General Public License
# Version 2. You may review the terms of this license at
# http://www.gnu.org/licenses/gpl-2.0.html
#
# Copyright (c) 2014, Markus Kuenne
# TU Dortmund University
# ------------------------------------------------------------------

BIN_GRAPHCREATOR      := ./graphCreator.bin
OBJ_GRAPHCREATOR      := graphCreator.o
OBJ_DEBUGNODE         := debugNode.o
OBJ_GRAPHFILEWRITER   := graphWriter.o
OBJ_TIMERFILEREADER   := timercsvreader.o
OBJ_MAPFILEREADER     := mapfilereader.o
OBJ_FILTERFILEREADER  := filterlist.o

OBJECTS := \
		$(OBJ_GRAPHCREATOR)        \
		$(OBJ_DEBUGNODE)           \
		$(OBJ_GRAPHFILEWRITER)     \
		$(OBJ_TIMERFILEREADER)     \
		$(OBJ_MAPFILEREADER)       \
		$(OBJ_FILTERFILEREADER)

all: $(BIN_GRAPHCREATOR)

$(BIN_GRAPHCREATOR) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ 
	
run : $(BIN_GRAPHCREATOR)
	$(BIN_GRAPHCREATOR)

clean :
	rm -f $(BIN_GRAPHCREATOR) $(OBJECTS)
	
doc :
	doxygen doxygen.conf


