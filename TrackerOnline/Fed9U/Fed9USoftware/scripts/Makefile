#FileName : 		Makefile
#Content :		Fed9U Documentation Makefile
#Programmer : 		Jonathan Fulcher : Emlyn Corrin
#Version : 		2.0.0
#Support : 		mail to : jr.fulcher@imperial.ac.uk

TARGET = FedInstallDev.bash

SCRIPTS = FedInstallFullRelease.bash
.PHONY: default upload

default: upload

upload: $(TARGET)
	scp -r FedInstallFullRelease.bash fulcher@lxplus.cern.ch:www/fed9u/download
