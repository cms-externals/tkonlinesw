
ROOTSYS=/opt/offline/slc6_amd64_gcc481/lcg/root/5.34.18-cms12/

ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
EVENTLIB      = $(EVENTO)
OutPutOpt     = -o

ROOTLIBS      = `/opt/offline/slc6_amd64_gcc481/lcg/root/5.34.18-cms12//bin/root-config --libs` -lNew -lg2c
ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d
ROOTCINT      = /opt/offline/slc6_amd64_gcc481/lcg/root/5.34.18-cms12//bin/rootcint

# Linux with egcs
#
#
#
#
 CXX           = g++
 CXXFLAGS      = -O3 -Wall -fPIC -I`/opt/offline/slc6_amd64_gcc481/lcg/root/5.34.18-cms12//bin/root-config --incdir`
 LD            = g++
 LDFLAGS       = -g
 SOFLAGS       = -shared
LIBS          = $(ROOTLIBS) -L/opt/cmsswLocal -lm -lstd++ -ldl -rdynamic -lg2c
GLIBS         = `root-config --glibs`   -lg2c
SHELL         = /bin/bash




# Librairies DELPHI
FLIBS = `cernlib packlib,mathlib` -lg2c

FORT = g77 -O -c -w 
# fortran file
zbr.o: zbr.f 
	$(FORT) -c zbr.f

#------------------------------------------------------------------------------

###
TBEventCint.o: $(HEADERS) TBEventCint.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBEventCint.cxx
TBBuilder.o: TBBuilder.h TBBuilder.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBBuilder.cxx
TBClu.o: TBClu.h TBClu.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBClu.cxx
TBCor.o: TBCor.h TBCor.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBCor.cxx
TBEvent.o: TBEvent.h TBEvent.cxx
	$(CXX) $(CXXFLAGS) -w -c TBEvent.cxx 
TBEventHeader.o:TBEventHeader.h TBEventHeader.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBEventHeader.cxx
TBGeo.o: TBGeo.h TBGeo.cxx
	$(CXX) $(CXXFLAGS) -w -c TBGeo.cxx 
TBMonitor.o: TBMonitor.h TBMonitor.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBMonitor.cxx
TBPed.o: TBPed.h TBPed.cxx
	$(CXX) $(CXXFLAGS) -w -c TBPed.cxx 
TBPlane.o: TBPlane.h TBPlane.cxx
	$(CXX) $(CXXFLAGS) -w -c TBPlane.cxx 
TBRana.o: TBRana.h TBRana.cxx
	$(CXX) $(CXXFLAGS) -w -c TBRana.cxx
TBRuAna.o: TBRuAna.h TBRuAna.cxx
	$(CXX) $(CXXFLAGS) -w -c TBRuAna.cxx
TBRaw.o: TBRaw.h TBRaw.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBRaw.cxx
TBRun.o: TBRun.h TBRun.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBRun.cxx
TBRU.o: TBRU.h TBRU.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBRU.cxx
TBZana.o: TBZana.h TBZana.cxx
	$(CXX) $(CXXFLAGS) -w -c TBZana.cxx 
TBZebraCint.o: TBZebraCint.cxx
	$(CXX) $(CXXFLAGS) -w -c  TBZebraCint.cxx
MainTBEvent.o: TBEvent.h MainTBEvent.cxx
	$(CXX) $(CXXFLAGS) -w -c MainTBEvent.cxx


OBJC          = $(EVENTO) 
EVENTO        = TBMonitor.$(ObjSuf) TBPlane.$(ObjSuf) TBEvent.$(ObjSuf) \
		TBGeo.$(ObjSuf)  TBRun.$(ObjSuf) TBRaw.$(ObjSuf)  \
		TBEventHeader.$(ObjSuf) TBCor.$(ObjSuf) TBClu.$(ObjSuf) \
		TBPed.$(ObjSuf) TBBuilder.$(ObjSuf) TBRana.$(ObjSuf) \
                TBRU.o TBRuAna.o TBEventCint.$(ObjSuf)

MAINEVENTO    = MainTBEvent.$(ObjSuf)


EVENTSO       = libTBEvent.$(DllSuf)
$(EVENTSO): $(EVENTO)
	$(LD) $(SOFLAGS) $(LDFLAGS) $(EVENTO) $(OutPutOpt) $(EVENTSO)
#	strip $(EVENTSO)


EVENT = $$SCRATCH/TBEvent$(ExeSuf)

PROGRAMS      = $(EVENT)


lib:	$(EVENTSO)
obj:  $(EVENTO)
all:            $(EVENTSO)




$(EVENT):       $(EVENTSO) TBZana.o TBZebraCint.o $(MAINEVENTO) zbr.o 
		#strip $(EVENTSO)
		g++ $(LDFLAGS) $(MAINEVENTO)  $(EVENTSO) $(GLIBS) zbr.o  $(FLIBS) $(FLIBS)  $(OutPutOpt) $(EVENT)
		#strip $(EVENT)
		@echo "$(EVENT) done"

clean:
		@rm -f *.o TBEventCint.* core








HEADERS = TBGeo.h TBRun.h TBEventHeader.h TBRaw.h TBPed.h TBCor.h \
          TBClu.h TBEvent.h TBBuilder.h TBRana.h TBPlane.h TBMonitor.h \
	  TBRU.h TBRuAna.h
TBEventCint.$(SrcSuf): $(HEADERS) TBLinkDef.h
	@echo "Generating dictionary ..."
	@touch TBEventCint.$(SrcSuf)
	@rm TBEventCint.$(SrcSuf)
	ROOTSYS=$(ROOTSYS) LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTCINT) TBEventCint.$(SrcSuf) -c $(HEADERS)  TBLinkDef.h 

TBZebraCint.$(SrcSuf): TBZana.h TBLinkZebra.h
	@echo "Generating dictionary ..."
	@touch TBZebraCint.$(SrcSuf)
	@rm TBZebraCint.$(SrcSuf)
	ROOTSYS=$(ROOTSYS) LD_LIBRARY_PATH=$(ROOTSYS)/lib $(ROOTCINT) TBZebraCint.$(SrcSuf) -c TBZana.h TBLinkZebra.h 









