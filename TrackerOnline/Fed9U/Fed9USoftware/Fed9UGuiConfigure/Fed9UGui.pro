TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -L$(ICUTILSDIR) -L../Fed9UUtils/ -L$(XERCESCROOT)/lib -lICUtils -lFed9UUtils -lxerces-c
DEFINES	+= XERCES=2
INCLUDEPATH	+= $(ICUTILSDIR)/include ../Fed9UUtils/include $(XERCESCROOT)/include

SOURCES	+= main.cpp
FORMS	= fed9uguiconfig.ui
IMAGES	= images/editcopy \
	images/editcut \
	images/editpaste \
	images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/redo \
	images/searchfind \
	images/undo

































unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}









































































































































































































TEMPLATE	= app
LANGUAGE	= C++

CONFIG	+= qt warn_on release

LIBS	+= -L../Fed9UUtils/ -L$(XERCESCROOT)/lib -lFed9UUtils -lxerces-c
DEFINES	+= XERCES=2
INCLUDEPATH	+= ../Fed9UUtils/include $(XERCESCROOT)/include


