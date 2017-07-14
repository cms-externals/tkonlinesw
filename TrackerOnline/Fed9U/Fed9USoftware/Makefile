#FileName : 		Makefile
#Content :		Fed9USoftware Master Makefile
#Programmer : 		Jonathan Fulcher : Emlyn Corrin
#Version : 		2.0.0
#Support : 		mail to : jr.fulcher@imperial.ac.uk

CONFIG_DEST?=config_temp
MAKE=gmake
STDOUT_FILE=std_output.log
UPLOADDIR=www/fed9u/download/
# KH for easy make install
LIB_FILES = Fed9UDevice/libFed9ULib.so \
	Fed9UDeviceFactory/libFed9UDeviceFactory.so \
	 Fed9UUtils/libFed9UUtils.so
INCLUDE_FILES = Fed9UUtils/include/*.h* \
		Fed9UDeviceFactory/include/*.h* \
		Fed9UDevice/include/*.h* \
		Fed9UVmeBase/include/Fed9UBusAdaptor.h*

include Makefile.in
include Makevars

.PHONY: default all force forceall clearlog clean cleandeps            \
	backup docupload update library source                         \
	Fed9UStandAlone Fed9UTester Fed9UDocumentation \
	Fed9UDevice Fed9UVmeDevice Fed9UVmeBase Fed9UDeviceFactory Fed9UUtils Fed9UNonStd \
	scripts check_version
#$(ENV_CMS_TK_FED9U_SUPERVISOR) 
default: check_version clearlog Fed9UStandAlone Fed9UGuiConfigure

all: check_version default Fed9UDocumentation
#$(ENV_CMS_TK_FED9U_SUPERVISOR)  
check_version: FED9U_BUILD_VERSION_$(ENV_CMS_TK_FED9U_XERCES_VERSION)
	@echo "**** Building version $(ENV_CMS_TK_FED9U_XERCES_VERSION)"

FED9U_BUILD_VERSION_$(ENV_CMS_TK_FED9U_XERCES_VERSION):
	@echo "**** Detected a version change, forcing a rebuild."
	@rm -f FED9U_BUILD_VERSION_* Fed9U*/obj/*.o
	@touch $@

force: clean default

forceall: cleanall all

clearlog:
	@rm -fv $(STDOUT_FILE)

cleanall: clearlog
	@$(MAKE) -C Fed9UNonStd        clean
	@$(MAKE) -C Fed9UUtils         clean
	@$(MAKE) -C Fed9UDeviceFactory clean
	@$(MAKE) -C Fed9UVmeBase       clean
	@$(MAKE) -C Fed9UVmeDevice     clean
	@$(MAKE) -C Fed9UDevice        clean
	@$(MAKE) -C Fed9UStandAlone    clean
	@$(MAKE) -C Fed9UDocumentation clean
#	@$(MAKE) -C Fed9USupervisor    clean
# >> $(STDOUT_FILE)

clean: clearlog
	@$(MAKE) -C Fed9UNonStd        clean
	@$(MAKE) -C Fed9UUtils         clean
	@$(MAKE) -C Fed9UDeviceFactory clean
	@$(MAKE) -C Fed9UVmeBase       clean
	@$(MAKE) -C Fed9UVmeDevice     clean
	@$(MAKE) -C Fed9UDevice        clean
	@$(MAKE) -C Fed9UStandAlone    clean

# Removes the dependency files in case a file is renamed and the dependencies become f****d
cleandeps:
	@rm -vf Fed9U*/obj/*.d

# Archives all source files, but makes sure that no build errors exist first
#  cleandeps: forces dependencies to be rebuilt, ensuring discovery of missing files
#  all: builds all targets, if this fails the archiving will not be completed
#  clean: removes files not needed for archiving, reducing the size of the archive
backup: Fed9UBackup.tgz
	scp -B -i ~/.ssh/identity_icva_fulchrjr Fed9UBackup.tgz \
	  fulchrjr@icva:Fed9U/Fed9USoftware-`date -I`.tgz

CONFIGFILES =

test: Fed9UUtils/include/Fed9UVersion.inc
#@ls -l $(CONFIGFILES)

Fed9UBackup.tgz: all
	@find Fed9UStandAlone Fed9UDevice           \
              Fed9UVmeDevice Fed9UVmeDevice Fed9UVmeBase Fed9UUtils Fed9UDeviceFactory Fed9UNonStd \
	      -type f ! -perm -a+x                                              \
	 | grep -v "~\>\|\/#.*#\>\|.d\>\|\.o\>\|\.a\>\|\.so\>\|\.fed\>\|\.log\>\|\.dat\>\|\.bak\>" \
	 | xargs tar -cvzf Fed9UBackup.tgz Makefile Makevars      \
	     Fed9UDocumentation/doxygen.config $(CONFIGFILES)
#$(ENV_CMS_TK_FED9U_SUPERVISOR) 
Fed9UUtils/include/Fed9UVersion.inc: .Fed9UReleaseVersion
	@echo "**** Updating version file"
	@scripts/MakeVersion.bash >| $@


release: Fed9UStandAlone
	scripts/Release.bash $(UPLOADDIR) $(ENV_CMS_TK_FED9U_XERCES_VERSION)
#$(ENV_CMS_TK_FED9U_SUPERVISOR)  $(ENV_CMS_TK_FED9U_SUPERVISOR)
releasenoincr: Fed9UStandAlone
	scripts/Release.bash $(UPLOADDIR) $(ENV_CMS_TK_FED9U_XERCES_VERSION) -noincr
#$(ENV_CMS_TK_FED9U_SUPERVISOR) $(ENV_CMS_TK_FED9U_SUPERVISOR) 

library: Fed9ULibrary.tgz

Fed9ULibrary.tgz: default
	@find Fed9UDevice Fed9UUtils Fed9UDeviceFactory Fed9UNonStd \
	      -type f ! -perm -a+x               \
	 | grep "\.a\>\|\.so\>\|\.hh\>"          \
	 | xargs tar -cvzf Fed9ULibrary.tgz      \
	     Makefile Makevars Fed9UStandAlone/{Makefile,src/*.c*,include/*.hh} $(CONFIGFILES)

source: Fed9USource.tgz

Fed9USource.tgz:
	@find Fed9UDevice Fed9UUtils Fed9UDeviceFactory Fed9UNonStd \
              -type f ! -perm -a+x               \
	 | grep "\/Makefile\>\|\.cc\>\|\.hh\>"   \
	 | xargs tar -cvzf Fed9USource.tgz       \
	     Makefile Makevars Fed9UStandAlone/{Makefile,src/*.c*,include/*.hh} $(CONFIGFILES)

docupload:
	$(MAKE) upload -C Fed9UDocumentation

update:
#	mv updates/Fed9UAddressTable.dat Fed9UStandAlone/
#	mv updates/ConfigureFed9U.cc Fed9UVmeBase/src/
#	mv updates/ConfigureFed9UException.cc Fed9UVmeBase/src/
#	mv updates/ConstructCommandString.cc Fed9UVmeBase/src/
#	mv updates/Fed9UHalInterface.cc Fed9UVmeBase/src/
#	mv updates/SerialCommunication.cc Fed9UVmeBase/src/
#	mv updates/ConfigureFed9U.hh Fed9UVmeBase/include/
#	mv updates/ConfigureFed9UException.hh Fed9UVmeBase/include/
#	mv updates/ConstructCommandString.hh Fed9UVmeBase/include/
#	mv updates/Fed9UHalInterface.hh Fed9UVmeBase/include/
#	mv updates/SerialCommunication.hh Fed9UVmeBase/include/
#	mv updates/Fed9UVmeDevice.cc Fed9UVmeDevice/src/
#	mv updates/Fed9UVmeDevice.hh Fed9UVmeDevice/include/
#	mv updates/Fed9UVmeDeviceException.hh Fed9UVmeDevice/include/
#	mv updates/Fed9UVmeDeviceException.cc Fed9UVmeDevice/src/

Fed9UStandAlone: Fed9UDevice
	$(MAKE) -C Fed9UStandAlone
#          >> $(STDOUT_FILE)

#Fed9USupervisor: Fed9UDevice
#	@$(MAKE) -C Fed9USupervisor -f Makefile
#          >> $(STDOUT_FILE)

#Fed9UTester:
#	@$(MAKE) -C Fed9UTester
#              >> $(STDOUT_FILE)

Fed9UDocumentation:
	@$(MAKE) -C Fed9UDocumentation
#       >> $(STDOUT_FILE)

Fed9UDevice: Fed9UVmeDevice
	@$(MAKE) -C Fed9UDevice
#              >> $(STDOUT_FILE)

Fed9UVmeDevice: Fed9UVmeBase
	@$(MAKE) -C Fed9UVmeDevice
#           >> $(STDOUT_FILE)

Fed9UVmeBase: Fed9UDeviceFactory
	@$(MAKE) -C Fed9UVmeBase
#             >> $(STDOUT_FILE)

Fed9UDeviceFactory: Fed9UUtils
	@$(MAKE) -C Fed9UDeviceFactory
#             >> $(STDOUT_FILE)

Fed9UUtils: Fed9UNonStd Fed9UUtils/include/Fed9UVersion.inc
	@$(MAKE) -C Fed9UUtils
#               >> $(STDOUT_FILE)

Fed9UNonStd:
	@$(MAKE) -C Fed9UNonStd
#              >> $(STDOUT_FILE)

Fed9UGuiConfigure:
	@cd Fed9UGuiConfigure
	@source changeenv
	@qmake
	@$(MAKE) -C Fed9UGuiConfigure
	@cd ..
scripts:
	@$(MAKE) -C scripts
install:
	if [[ "$(BIN_DEST)" != "" ]]; \
	then cp Fed9UStandAlone/*.exe $(BIN_DEST); \
	fi;
	if [[ "$(LIB_DEST)" != "" ]]; \
	then cp $(LIB_FILES) $(LIB_DEST); \
	fi;
	if [[ "$(CONFIG_DEST)" != "" ]]; \
	then cd config/; \
	ls | grep -E "^[^(CVS)]" | xargs cp --target-directory=$(CONFIG_DEST); \
	cd - ; \
	fi; \
	if [[ "$(INCLUDE_DEST)" != "" ]]; \
	then cp $(INCLUDE_FILES) $(INCLUDE_DEST); \
	fi;
