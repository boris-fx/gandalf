# Include the list of sources and headers
include(gandalf.pri)

# Set correct library path for Mac bundle
mac:QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/

# Build a dynamic library in RELEASE mode
TEMPLATE = lib
CONFIG = dll qt warn_on release
DESTDIR = ../build/release

# Include parent directory, to allow <gandalf/...> includes
INCLUDEPATH += ..

# Include image libraries
INCLUDEPATH += ../libjpeg ../libtiff 
LIBS += -L../build/release -ljpeg -ltiff -lpng -lz

# Add frameworks on the Mac
mac:LIBS += -framework QtGui -framework OpenGL -framework Accelerate

