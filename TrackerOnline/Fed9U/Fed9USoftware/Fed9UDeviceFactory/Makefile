TARGET = libFed9UDeviceFactory.so
utilheaders: default header

NONSTDSRC=$(wildcard ../Fed9UNonStd/$(SRC)/*.cc)
CUSTOMOBJECTS= $(patsubst ../Fed9UNonStd/$(SRC)/%.cc, ../Fed9UNonStd/$(OBJ)/%.o, $(NONSTDSRC))

include ../Makefile.in
include ../Makevars.noroot
include ../Makerules

header: $(INC)/Fed9UDeviceFactoryLib.hh
CPPFLAGS += -DTBHARDWARE

ifeq ($(ENV_CMS_TK_FED9U_USING_DATABASE),1)
  CUSTOM_FED9USTANDALONE_LIB+= $(ENV_CMS_TK_FED9U_DATABASE_LIBRARY) $(ENV_CMS_TK_FED9U_ORACLE_LIBRARY)
endif

CUSTOM_LIBS=-lFed9UUtils -lICUtils #$(CUSTOM_FED9USTANDALONE_LIB) 


HEADERS=    ../Fed9UDeviceFactory/$(INC)/Fed9UXMLDescriptionFromDb.hh      \
	    ../Fed9UDeviceFactory/$(INC)/Fed9UDescriptionToXmlToDb.hh      \
	    ../Fed9UDeviceFactory/$(INC)/Fed9UDeviceFactoryException.hh      \
	    ../Fed9UDeviceFactory/$(INC)/Fed9UDeviceFactory.hh   

$(INC)/Fed9UDeviceFactoryLib.hh: $(HEADERS) Makefile
	@echo "**** building $@"
	@cat ../licence.txt $(HEADERS) \
	>| $@
#	| grep -v "$\#include *\"Fed9U" | grep -v "$\#include *\"TypeDefs.hh\"" >| $@
