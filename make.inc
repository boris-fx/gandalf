LIB = $(TOPLEVEL)/libgandalf.la
ifeq ($(support_png), no)
   PNG_LIBS=
endif

LIBS += $(PNG_LIBS) $(JPEG_LIB) $(TIFF_LIB) $(LAPACK_LIBS) $(GLUT_LIBS) $(OPENGL_LIBS) -lm $(X_LIBS)

GARBAGE = $(OBJS) core a.out *~ *.o

PATH_BUILDER_C = $(TOPLEVEL)/TestFramework/path_builder.c
