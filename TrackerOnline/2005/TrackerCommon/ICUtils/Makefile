
TARGET=libICUtils.so
LIB_FILES:=./libICUtils.so
INCLUDE_FILES:=include/*.h*
include Makevars

install:
	if [[ "$(LIB_DEST)" != "" ]]; \
	then cp $(LIB_FILES) $(LIB_DEST); \
	fi;
	if [[ "$(INCLUDE_DEST)" != "" ]]; \
	then cp $(INCLUDE_FILES) $(INCLUDE_DEST); \
	fi;
