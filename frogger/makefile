CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g
NAME = a1-basic
OBJECTS = ${NAME}.o Displayable.o XInfo.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = frogger
MAC_OPT = -I/opt/X11/include
$EXEC : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -L/opt/X11/lib -lX11
-include ${DEPENDS}
.PHONY: clean
all : 
	@echo "Compiling..."
	${MAKE}
run : all
	@echo "Running..."
	./${EXEC}
run60: all
	@echo "Running..."
	./${EXEC} 60
clean :
	-rm *o *d
