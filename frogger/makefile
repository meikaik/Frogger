

CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -g
OBJECTS = main.o Displayable.o XInfo.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = frogger
$EXEC : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -I/opt/X11/include -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
-include ${DEPENDS}
.PHONY: clean
run : 
	./${EXEC}
run60: 
	./${EXEC} 60
clean :
	rm ${OBJECTS} ${EXEC} ${DEPENDS}
