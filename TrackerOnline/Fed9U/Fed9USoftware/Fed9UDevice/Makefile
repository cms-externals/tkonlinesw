TARGET = libFed9ULib.so

LINKTOHAL=1
CPPFLAGS += -DTBHARDWARE

libheader: default header

NONSTDSRCVMEDEVICE=$(wildcard ../Fed9UVmeDevice/$(SRC)/*.cc)
NONSTDSRCVMEBASE=$(wildcard ../Fed9UVmeBase/$(SRC)/*.cc)
CUSTOMOBJECTS= $(patsubst ../Fed9UVmeDevice/$(SRC)/%.cc, ../Fed9UVmeDevice/$(OBJ)/%.o, $(NONSTDSRCVMEDEVICE)) \
		$(patsubst ../Fed9UVmeBase/$(SRC)/%.cc, ../Fed9UVmeBase/$(OBJ)/%.o, $(NONSTDSRCVMEBASE))

include ../Makefile.in
include ../Makevars
include ../Makerules

header: $(INC)/Fed9ULib.hh
CUSTOM_LIBS= $(ENV_CMS_TK_FED9U_DIAG_LIBDIR) -lFed9UUtils 

HEADERS=../Fed9UVmeDevice/$(INC)/Fed9UVmeDeviceException.hh \
	    ../Fed9UVmeDevice/$(INC)/Fed9UVmeDevice.hh \
	    ../Fed9UDevice/$(INC)/Fed9UDeviceException.hh      \
	    ../Fed9UDevice/$(INC)/Fed9UDevice.hh \
	    ../Fed9UDevice/$(INC)/Fed9UCrateStatusProbe.hh \
	    ../Fed9UDevice/$(INC)/Fed9UMoFO.hh 

$(INC)/Fed9ULib.hh: $(HEADERS) Makefile
	@echo "**** building $@"
	@echo "#include \"Fed9UUtils.hh\"" >| $@
	@echo "#include \"Fed9UDeviceFactory.hh\"" >> $@
	@cat ../licence.txt $(HEADERS) \
	| grep -v "$\#include *\"Fed9U" | grep -v "$\#include *\"TypeDefs.hh\"" >> $@
