# Include the list of sources and headers
include(gandalf.pri)

# Set correct library path for Mac bundle
mac:QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/

# Build a dynamic library in DEBUG mode
TEMPLATE = lib
CONFIG = qt debug warn_on
DESTDIR = ../build/debug

# Include parent directory, to allow <gandalf/...> includes
INCLUDEPATH += ..

# Include image libraries
INCLUDEPATH += ../libjpeg ../libtiff 
LIBS += -L../build/debug -ljpeg -ltiff -lpng -lz

# Add frameworks on the Mac
mac:LIBS += -framework QtGui -framework OpenGL -framework Accelerate
