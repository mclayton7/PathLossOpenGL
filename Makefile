CXX=g++
CXXFLAGS = -Wno-deprecated
LD_FLAGS= -framework OpenGL -framework GLUT 


pathlossGL:	pathlossGL.o
	@echo Linking pathlossGL
	$(CXX) -o pathlossGL pathlossGL.o ${LD_FLAGS}

clean:
	@rm -f *.o pathloss
