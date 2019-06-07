EXEC = main
OBJECTS = main.o
#FILES = main.cpp shader.cpp camera.cpp stlLoader.cpp surface.cpp glad.c   
FILES = main.cpp shader.cpp camera.cpp stlLoader.cpp glad.c
FILES += $(shell find ./lib/local -name '*.cpp')

# The flags to use for compilation                                    
FLAGS = -L/usr/local/lib -lglfw3 -lrt -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp -lGLU -lGL -lglut -lboost_system -lopenblas  

INC_OPENCV = -I/usr/local/include/opencv2 -I/usr/local/include
LIB_OPENCV = -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_calib3d

INC_OPENFACE = -I /usr/local/ -I/home/adrienhc/CS168/lib/local/FaceAnalyser/include/ -I/home/adrienhc/CS168/lib/local/GazeAnalyser/include/ -I/home/adrienhc/CS168/lib/local/LandmarkDetector/include/ -I/home/adrienhc/CS168/lib/local/Utilities/include/
LIB_OPENFACE = -L /usr/local/ -ldlib

INC_BOOST = -I /usr/include/ -I /usr/include/boost/
LIB_BOOST = -L /usr/lib/x86_64-linux-gnu -lboost_filesystem #-lstdc++fs

# The code compiler to use for compilation                            
CC = g++                                                              

# Perform action on all object files (May or may not exist)           
all:                                                       
	$(CC) $(INC_OPENCV) $(INC_OPENFACE) $(INC_BOOST) $(FILES) $(LIB_OPENCV) $(LIB_OPENFACE) $(LIB_BOOST) $(FLAGS) -o $(EXEC)  

clean:
	rm -rf $(EXEC) *.o