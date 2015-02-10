#!/bin/bash



if [[ "$1" = "-s" ]]; then
    INSTALL_SOURCE=1
else
    unset INSTALL_SOURCE
fi

if [[ "$1" = "-d" ]]; then
    INSTALL_DEV=1
else
    unset INSTALL_DEV
fi

if [[ $INSTALL_DEV ]]; then
    INSTALL_LEVEL=3
    echo -n "
    Please make sure that you have sourced the xdaq_profile and ldconf.sh 
    in your DAQBASE directory.
    This is the directory where your Tracker DAQ Software is installed. "
else
    echo -n "
    Before installing the FED software you must check that you have all the 
    the required complimentary components. Then select one of the following 
    options:
    
    1. If you are just going to install the Fed Utilities library for use in 
    stand alone analysis etc, then you do not need to install anything else. 
    
    2. If you are going to use the Fed Library to control a FED and read out 
    data, you will need to install HAL which you can find here: 
    http://cmsdoc.cern.ch/~cschwick/software/documentation/HAL/index.html
    
    3. If you intend to use the full functionality of the FED Supervisor under
    XDAQ, Then you will need a full installation of TriDAS, if you don't, 
    the FED software will not compile without the libraries that TriDAS contains.
    To install TriDAS go to:
    http://cmsdoc.cern.ch/cms/cmt/Sytem_aspects/Daq
    
    Which level of FED software would you like to install? (1,2 or 3) "
    
    temp=
    while [[ $temp != ?* ]]; do
        read temp
    done
    echo ""
    
    if [[ $temp != 1 && $temp != 2 && $temp != 3 ]]; then
        echo "Invalid response... Installation aborting...!"
        exit
    fi
    
    INSTALL_LEVEL=$temp
fi
    
if [[ $INSTALL_SOURCE ]]; then
   if [[ (! -e Fed9UUtils_$INSTALLVERSION.tgz)                     \
        || (INSTALL_LEVEL > 1 && ! -e Fed9ULib_$INSTALLVERSION.tgz) \
        || (INSTALL_LEVEL > 2 && ! -e Fed9USupervisor_$INSTALLVERSION.tgz) ]]; then
      echo "You must obtain the required tar files for this installation and place them
              in the same directory as this script, please contact the vendor for any advice."
      exit
   fi
fi

temp=

if [[ $INSTALL_LEVEL = 3 ]]; then
    temp=yes
else

    while [[ "$temp" != "yes" && "$temp" != "no" ]]; do
	echo -n " Are you installing the FED software in conjunction with the CMS Tracker DAQ software? (yes/no) "
	read temp
	temp=`echo $temp | tr A-Z a-z`
    done
    echo ""
fi

if [[ $temp = yes ]]; then
    if [[ $INSTALL_LEVEL < 3 ]]; then
        echo "If you are using DAQ software you will require the Fed9USupervisor, which is a level 3 install
              You will need to start the installation again and select level 3 install."
        exit
    else
        USINGTRACKERDAQ=1
        while [[ "$temp" != "yes" && "$temp" != "no" ]]; do
            echo -n " Are you using Tracker DAQ software version 2? (yes/no) "
            read temp
            temp=`echo $temp | tr A-Z a-z`
        done
        echo ""
        if [[ $temp = yes ]]; then
            DAQ_VERSION=2
        else
            DAQ_VERSION=1
        fi
    fi
else
    USINGTRACKERDAQ=0
fi

if [[ $USINGTRACKERDAQ = 1 && $INSTALL_LEVEL > 1 ]]; then
    echo "Please type the directory in which you have your DAQ installation (This is normally 
        defined by the environmental variable DAQBASE, and where your TriDAS folder is located"
else
    echo "Please type the directory in which you have your XERCES installation"
fi

founddir=0
while [[ $founddir != 1 ]]; do

    temp=   
    while [[ $temp != ?* ]]; do
        read temp
    done
    echo ""
    if [ -d $temp ]; then
        echo "Found Directory"
        founddir=1
    else
        echo "I cannot find this directory please type another:"
    fi
done

DAQBASE=$temp #note that this variable contains the location of the daq software or the location of the xerces software if daq is not being used

echo "Please type the directory in which you would like to install the FED software.
    We recommend that you install in your home directory (/home/username/). 
    If you wish to install in another directory there may be issues to resolve
    with expected paths."    

founddir=0
while [[ $founddir != 1 ]]; do

    temp=   
    while [[ $temp != ?* ]]; do
	read temp
    done
    echo ""
    if [ -d $temp ]; then
	echo "Found Directory Preparing to install Fed Software in this folder"
	founddir=1
    else
        echo "I cannot find this directory please type another:"
    fi
done

INSTALLDIR=$temp

if [ -e $INSTALLDIR/Fed9USoftware ]; then
  temp=
  while [[ "$temp" != "yes" && "$temp" != "no" ]]; do
    echo -n "
    You have a current installation of Fed9USoftware,
    do you want to overwrite it? (yes/no)"

    read temp
    temp=`echo $temp | tr A-Z a-z`
  done
  echo ""
  
  if [[ "$temp" != "yes" ]]; then
    echo  "Cannot continue, Fed Software Already exists. Installation aborting...!"
    exit
  fi
fi
    
echo "Now we will install FED software"
if [[ $XERCES_VERSION ]]; then
    if [[ $XERCES_VERSION = 1 ]]; then
        temp=no
    else
        temp=yes
    fi
    echo $temp
else
    temp=
    while [[ "$temp" != "yes" && "$temp" != "no" ]]; do
        echo -n "
        Do you have version 2 or above of the XERCES XML library installed? (yes/no)"
        read temp
        temp=`echo $temp | tr A-Z a-z`
    done
    echo ""
fi

# XERCES_ROOT defaults to DAQBASE, if INSTALL_LEVEL > 1 it will be overridden
XERCES_ROOT=$DAQBASE
if [[ "$temp" != "yes" ]]; then
    XERCES_VERSION=1
    if [[ $INSTALL_LEVEL > 1 ]]; then
        if [[ $USINGTRACKERDAQ = 1 ]]; then
            XERCES_ROOT=$DAQBASE/TriDAS/daq/extern/xerces/linuxx86
        else
            XERCES_ROOT=$DAQBASE/xerces/linuxx86
        fi
    fi
else
    XERCES_VERSION=2
    if [[ $INSTALL_LEVEL > 1 ]]; then
        XERCES_ROOT=$DAQBASE
        #NOTE this is what the XERCESCROOT will be set to, but we don't actually have to put it in the Fed_Profile
        #if we are using the Tracker DAQ
    fi
fi

cd $INSTALLDIR

#now we must set up the environment put all env variables into a profile 
#file called Fed_Profile



echo "CREATING THE Fed_Profile FILE"

cat << EOF >| Fed_Profile
export FED9UROOT=$INSTALLDIR/Fed9USoftware
export ICUTILSDIR=$INSTALLDIR/ICUtils
export XERCES_VERSION=$XERCES_VERSION
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$ICUTILSDIR:\$FED9UROOT/Fed9UUtils:\$XERCESCROOT/lib:\$ROOTSYS/bin:\$ROOTSYS/lib
EOF

if [[ $DAQ_VERSION = 2 ]]; then
cat << EOF >> Fed_Profile
export TRACKER_DAQ_VERSION=2
EOF
fi

if [[ $INSTALL_LEVEL > 1 ]]; then
    temp=
    while [[ "$temp" != "nivxi" && "$temp" != "sbs" ]]; do
        echo "Are you using SBS or NIVXI Bus Adaptor (SBS/NIVXI)" 
        read temp
        temp=`echo $temp | tr A-Z a-z`
    done
    echo ""
    if [[ "$temp" = "sbs" ]]; then
        HAL_BUS=SBS
    else
        HAL_BUS=NIVXI
    fi
fi

echo "set bus adaptor..."

if [[ $USINGTRACKERDAQ = 0 ]]; then

    if [[ $INSTALL_LEVEL > 1 ]]; then
	founddir=0
	while [[ $founddir != 1 ]]; do
           echo "Please insert the root directory where hal is installed (eg. /home/user/hal)" 
	    temp=   
	    while [[ $temp != ?* ]]; do
		read temp
	    done
	    echo ""
	    if [ -d $temp ]; then
		echo "Found Directory"
		founddir=1
	    else
		echo "I cannot find this directory please type another:"
	    fi
	done
cat << EOF >> Fed_Profile
export HAL=$temp
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$temp/busAdapter/lib/linux/x86/:$temp/generic/lib/linux/x86/
EOF
	founddir=0
        while [[ $founddir != 1 ]]; do
	    echo "Please insert the directory where your Bus Adaptor library is installed (eg. for SBS /home/user/SBS)" 
	    temp=   
	    while [[ $temp != ?* ]]; do
		read temp
	    done
	    echo ""
	    if [ -d $temp ]; then
		echo "Found Directory"
		founddir=1
	    else
		echo "I cannot find this directory please type another:"
	    fi
	done

cat << EOF >> Fed_Profile
export BUSADAPTORDIR=$temp
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$temp/1003/v2.0/lib:$temp/lib
EOF

    fi

cat << EOF >> Fed_Profile
export XERCESCROOT=$XERCES_ROOT
export DAQBASE=$DAQBASE
export ROOTSYS=/usr/local/root
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$ROOTSYS/bin:\$ROOTSYS/lib:
EOF

else

    if [[ $INSTALL_LEVEL > 1 ]]; then
	echo "Adding more stuff to the Fed Profile..."

cat << EOF >> Fed_Profile
export BUSADAPTORDIR=\${DAQBASE}/SBS/1003/v2.0
EOF
    fi


# No longer need this since the new makefile for supervisor does no require the links
#    if [[ $INSTALL_LEVEL = 3 && "$DAQBASE" != "$INSTALLDIR" ]]; then
#cat <<EOF >> Fed_Profile
#rm \${DAQBASE}/Fed9USoftware
#rm \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#ln -s \$FED9UROO \${DAQBASE}/Fed9USoftware
#ln -s \${DAQBASE}/Fed9USoftware/Fed9USupervisor \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#chmod 777 \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#chmod 777 \${DAQBASE}/Fed9USoftware
#EOF
#    else
#cat <<EOF >> Fed_Profile
#rm \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#ln -s \${DAQBASE}/Fed9USoftware/Fed9USupervisor \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#chmod 777 \${DAQBASE}/TriDAS/daq/Fed9USupervisor
#EOF
#    fi

fi


if [[ $INSTALL_LEVEL > 1 ]]; then
cat << EOF >> Fed_Profile
export HALBUS=$HAL_BUS
export TRIDASDIR=\${DAQBASE}/TriDAS
export FECSOFTDIR=\${DAQBASE}/FecSoftware
export GENERICFEDDIR=\${DAQBASE}/Tracker/Interface
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:\$FED9UROOT/Fed9UDevice:\$FED9UROOT/Fed9USupervisor
EOF
else
cat << EOF >> Fed_Profile
export TRIDASDIR=$HOME/this/directory/does/not/exist/i/hope
export FECSOFTDIR=$HOME/this/one/does/not/exist/either
export BUSADAPTORDIR=$HOME/you/guessed/it/nor/does/this/one
EOF
fi

source Fed_Profile

if [[ $USINGTRACKERDAQ = 1 ]]; then
    cd $DAQBASE
    source xdaq_profile
    source ldconf.sh
fi

#If we are installing a dev version we need not create a Fed9USoftware directory or get the tar files instead we must check out from cvs

if [[ $INSTALL_DEV ]]; then


    USERNAME=
    while [[ $USERNAME != ?* ]]; do
        echo "Enter your cvs username:" 
        read USERNAME
    done
    echo ""
    
    
    uname -n | grep heppc41
    temp=$?
    if [[ $temp != 0 ]]; then
        xterm -e sshTunnel.bash &
        echo "Hit enter to continue the installation..."
        read temp
        PORT=24001
    else
        PORT=2401
    fi

    cd $INSTALLDIR
    cvs -d :pserver:$USERNAME@localhost:$PORT/home/cvs/repository login

    cvs -d :pserver:$USERNAME@localhost:$PORT/home/cvs/repository co Fed9USoftware
    
    cd Fed9USoftware

    for dir in Fed9U* 
    do
        mkdir -p $dir/obj
    done

else
    cd $INSTALLDIR
    mkdir Fed9USoftware
    cd Fed9USoftware
    #first get the utils, since if we have arrived here, then they at least
    #want that.
    if [[ $INSTALL_SOURCE ]]; then
        tar -xzvf ../Fed9UUtils_$INSTALLVERSION.tgz
        mkdir Fed9UUtils/obj
        mkdir Fed9UNonStd/obj
    else
        wget http://www.cern.ch/fed9u/download/xerces$XERCES_VERSION/LibFed9UUtils_$INSTALLVERSION.tgz
        tar -xzvf LibFed9UUtils_$INSTALLVERSION.tgz
    fi

    #now we check to see if they want the lib
    if [[ $INSTALL_LEVEL > 1 ]]; then
        if [[ $INSTALL_SOURCE ]]; then
            tar -xzvf ../Fed9ULib_$INSTALLVERSION.tgz
            mkdir Fed9UDevice/obj
            mkdir Fed9UVMEDevice/obj
            mkdir Fed9UVMEBase/obj
            mkdir Fed9UStandAlone/obj
        else
            wget http://www.cern.ch/fed9u/download/xerces$XERCES_VERSION/LibFed9ULib_$INSTALLVERSION.tgz
            tar -xzvf LibFed9ULib_$INSTALLVERSION.tgz
        fi
    fi

    #now we check to see if they want the supervisor

    if [[ $INSTALL_LEVEL > 2 ]]; then
        if [[ $INSTALL_SOURCE ]]; then
            tar -xzvf ../Fed9USupervisor_$INSTALLVERSION.tgz
            mkdir Fed9USupervisor/lib/
            mkdir Fed9USupervisor/lib/linux    
            mkdir Fed9USupervisor/lib/linux/x86    
            mkdir Fed9USupervisor/src/linux
            mkdir Fed9USupervisor/src/linux/x86
        else
            wget http://www.cern.ch/fed9u/download/xerces$XERCES_VERSION/LibFed9USupervisor_$INSTALLVERSION.tgz
            tar -xzvf LibFed9USupervisor_$INSTALLVERSION.tgz
        fi
    fi

    #rm *.tgz
fi


if [[ ($INSTALL_SOURCE) || ($INSTALL_DEV) ]]; then
    cd $INSTALLDIR/Fed9USoftware
#    gmake
    
    if [[ $INSTALL_LEVEL > 2 ]]; then
        ln -s
#        gmake Fed9USupervisor
    fi
fi

if [[ $INSTALL_LEVEL > 1 ]]; then
    ln -s ${DAQBASE}/hal ${DAQBASE}/TriDAS/daq/hal
    ln -s ${DAQBASE}/hal ${DAQBASE}/TriDAS/hal
    cd Fed9UStandAlone
    mkdir -p ~/data
    ln -s ~/data .
    ln -s ../Fed9UVMEBase/Fed9UAddressTable.dat .
    ln -s Fed9USoftware/Fed9UVMEBase/Fed9UAddressTable.dat $INSTALLDIR
fi

cd $INSTALLDIR

echo "
  Please Check Fed_Profile and make sure it is added to your .bashrc file
  or appropriate shell initialization file!

  Installation Complete! Wow... bet your amazed!"
