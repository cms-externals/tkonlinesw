TARGET = libFed9UUtils.so
utilheaders: default header

NONSTDSRC=$(wildcard ../Fed9UNonStd/$(SRC)/*.cc)
CUSTOMOBJECTS= $(patsubst ../Fed9UNonStd/$(SRC)/%.cc, ../Fed9UNonStd/$(OBJ)/%.o, $(NONSTDSRC))

include ../Makefile.in
include ../Makevars
include ../Makerules

header: $(INC)/Fed9UUtils.hh
CPPFLAGS += -DTBHARDWARE

CUSTOM_LIBS= -lICUtils


HEADERS=../Fed9UUtils/$(INC)/TypeDefs.hh         \
	    ../Fed9UNonStd/$(INC)/Fed9UFileHelpers.hh \
	    ../Fed9UNonStd/$(INC)/Fed9UWait.hh \
	    ../Fed9UUtils/$(INC)/Fed9UChannelInterface.hh     \
	    ../Fed9UUtils/$(INC)/Fed9UAddress.hh     \
	    ../Fed9UUtils/$(INC)/Fed9UBeStatus.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UStripDescription.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UStrips.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UTempControl.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UFrontEndDescription.hh    \
	    ../Fed9UUtils/$(INC)/Fed9UTtcrxDescription.hh \
	    ../Fed9UUtils/$(INC)/Fed9UTtcrxDescriptionInfo.hh \
	    ../Fed9UUtils/$(INC)/Fed9UVoltageControl.hh \
	    ../Fed9UUtils/$(INC)/Fed9UVoltageControlInfo.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEpromDescription.hh \
	    ../Fed9UUtils/$(INC)/Fed9UABC.hh     \
	    ../Fed9UUtils/$(INC)/Fed9UGuiAbcMap.hh     \
	    ../Fed9UUtils/$(INC)/Fed9UDescription.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEntry.hh \
	    ../Fed9UUtils/$(INC)/Fed9UCrateStatus.hh \
	    ../Fed9UUtils/$(INC)/Fed9UStrX.hh \
	    ../Fed9UUtils/$(INC)/Fed9UXStr.hh \
	    ../Fed9UUtils/$(INC)/Fed9UDOMCountErrorHandler.hh \
	    ../Fed9UUtils/$(INC)/Fed9UXMLDescriptionException.hh \
	    ../Fed9UUtils/$(INC)/Fed9UXMLDescription.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UXMLDescriptionFromFile.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UDescriptionToXml.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UEventException.hh      \
	    ../Fed9UUtils/$(INC)/Fed9UEvent.hh      \
            ../Fed9UUtils/$(INC)/Fed9UFakeBufferCreator.hh \
	    ../Fed9UUtils/$(INC)/Fed9USu8.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventStreamLineException.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventUnitStreamLine.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventStreamLine.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferedEvent.hh \
	    ../Fed9UUtils/$(INC)/Fed9UCounters.hh \
	    ../Fed9UUtils/$(INC)/Fed9ULockFile.hh\
	    ../Fed9UUtils/$(INC)/Fed9ULog.hh \
	    ../Fed9UUtils/$(INC)/Fed9UCreateDescriptionException.hh \
	    ../Fed9UUtils/$(INC)/Fed9UCreateDescription.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventInfo.hh \
	    ../Fed9UUtils/$(INC)/Fed9USiStripReordering.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferDescription.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferTrailer.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferData.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferHeader.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferCreator.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferCreatorRaw.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferCreatorProcRaw.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferCreatorZS.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferCreatorScopeMode.hh \
	    ../Fed9UUtils/$(INC)/Fed9UBufferGenerator.hh \
	    ../Fed9UUtils/$(INC)/Fed9UFakeBufferCreator.hh \
	    ../Fed9UUtils/$(INC)/Fed9USu8.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventStreamLineException.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventUnitStreamLine.hh \
	    ../Fed9UUtils/$(INC)/Fed9UEventStreamLine.hh \
	    ../Fed9UUtils/$(INC)/Fed9ULogTemplate.hh \
	    ../Fed9UUtils/$(INC)/Fed9USpyEventException.hh \
	    ../Fed9UUtils/$(INC)/Fed9USpyEvent.hh \
	    ../Fed9UUtils/$(INC)/Fed9USpyApvDataException.hh \
	    ../Fed9UUtils/$(INC)/Fed9USpyApvData.hh \
	    ../Fed9UUtils/$(INC)/Fed9UDescriptionException.hh \
	    ../Fed9UUtils/$(INC)/Fed9UCrc.hh

$(INC)/Fed9UUtils.hh: $(HEADERS) Makefile
	@echo "**** building $@"
	@cat ../licence.txt $(HEADERS) \
	| grep -v "$\#include *\"Fed9U" | grep -v "$\#include *\"TypeDefs.hh\"" >| $@
