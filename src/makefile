CXX=g++

CXXFLAGS=-g -Wall -std=c++17
LIBFLAGS=-g -Wall -std=c++17 -shared -fPIC
OBJFLAGS=-g -Wall -std=c++17 -c -fPIC

LIBS = -L./build/ -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lm -lwayland++
LLIBS = -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lm


TARGET=crystals

INC=-I/usr/include/**/ -I/usr/include/drm/ -I./Wayland++/ -I./MyCompositor/


LIBSRCS = ./Wayland++/WBackendDRM.cpp 
LIBSRCS += ./Wayland++/WCompositor.cpp 
LIBSRCS += ./Wayland++/WInput.cpp 
LIBSRCS += ./Wayland++/WView.cpp 

LIBOBJS = ./build/WBackendDRM.o 
LIBOBJS += ./build/WCompositor.o 
LIBOBJS += ./build/WInput.o 
LIBOBJS += ./build/WView.o 

all: crystals

crystals: libwayland++
	$(CXX) $(CXXFLAGS) $(INC) ./MyCompositor/*.cpp -o ./build/crystals $(LIBS)

libwayland++ : wayland++
	$(CXX) $(LIBFLAGS) $(INC) ./build/*.o  -o ./build/libwayland++.so  $(LLIBS)

wayland++ : 
	$(CXX) $(OBJFLAGS) $(INC) ./Wayland++/WBackendDRM.cpp  -o ./build/WBackendDRM.o  $(LLIBS)
	$(CXX) $(OBJFLAGS) $(INC) ./Wayland++/WCompositor.cpp  -o ./build/WCompositor.o  $(LLIBS)
	$(CXX) $(OBJFLAGS) $(INC) ./Wayland++/WInput.cpp  -o ./build/WInput.o  $(LLIBS)
	$(CXX) $(OBJFLAGS) $(INC) ./Wayland++/WView.cpp  -o ./build/WView.o  $(LLIBS)

clean:
	$(RM) $(TARGET)