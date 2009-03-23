# Include the list of sources and headers
include(gandalf.pri)
TEMPLATE = lib

CONFIG(qtestlib) {
#configuration happens in test/test.pro	
} else {
   #go up one directory to get to root
   TOROOT = ../../..
}

# COMMON TO TEST AND NON-TEST CONFIGURATIONS
include($${TOROOT}/Dependencies/PNG.pri)
include($${TOROOT}/Dependencies/JPEG.pri)
include($${TOROOT}/Dependencies/TIFF.pri)

LIBS += -lpng -ltiff -ljpeg  
	# this comment just keeps the line from being deleted

debugqmake: message ($${CONFIG})

CONFIG(debug,debug|release) {
	debugqmake: message (building debug mode)
	DEFINES += _DEBUG	
} else {
}


macx{
    # Set mac attributes for Mac OS 10.4
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    CONFIG+= x86 ppc	
    DESTDIR  = ../lib/Mac
}

win32{
 DEFINES += _WIN32
 DEFINES += WIN32
 DEFINES += GANDALF_EXPORTS
 DESTDIR  = ../lib/Win/Debug
}

CONFIG += dll
	
QT += opengl core

INCLUDEPATH	+=	. \
				.. \
				../.. \
            ../../.. \
				$(QTDIR)/include
				# this comment just keeps the line from being deleted

DEPENDPATH	+=	../.. \
				# this comment just keeps the line from being deleted

debugqmake : message($${CONFIG})