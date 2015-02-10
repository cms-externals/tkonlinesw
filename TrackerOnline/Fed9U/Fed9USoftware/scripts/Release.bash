#!/bin/bash

VERSION=`tail -n 1 .Fed9UReleaseVersion`
MAJORVERSION=${VERSION%*_*}
MINORVERSION=${VERSION#*_*}

function yesno () {
    while [ 1=1 ]; do  
        echo -n "$1 "
        [ -z $2 ] || echo -n "[$2] "
        IFS='@' read ans || exit 1
        [ -z "$ans" ] && ans=$2
        [ -z $ans ] && ans="z" 
        if [ $ans = "y" -o $ans = "Y" ]; then
            return 0
        fi
        if [ $ans = "n" -o $ans = "N" ]; then
            return 1
        fi
        echo
        echo "You MUST reply ""y"" or ""n"" !" 
        echo
    done
}

while [[ "$temp" != "yes" ]]; do
    echo -n "  IMPORTANT! -> Have you run cvs update and cvs commit without conflicts? (yes/no) <- IMPORTANT! "
    read temp
    temp=`echo $temp | tr A-Z a-z`
    if [[ "$temp" = "no" ]]; then echo "Well do it now then!"; exit; fi
done

echo Last version was $MAJORVERSION.$MINORVERSION

if [[ $1 != "-noincr" && $2 != "-noincr" && $3 != "-noincr" && $4 != "-noincr" ]];then
    temp=
    while [[ "$temp" != "yes" && "$temp" != "no" ]]; do
        echo -n "  Are you preforming a major version release? (yes/no) "
        read temp
        temp=`echo $temp | tr A-Z a-z`
    done
    echo ""

    if [[ "$temp" = "yes" ]]; then
        let "MAJORVERSION = MAJORVERSION + 1"    
        MINORVERSION=0
        echo  "Incrementing Major version number to $MAJORVERSION ...!"
    else
        let "MINORVERSION = MINORVERSION + 1"
        echo  "Incrementing Minor version number to $MINORVERSION ...!"
    fi
    if [[ $3 = "Fed9USupervisor" ]];then
	usingSupervisor=1
    else 
	usingSupervisor=0
    fi
else

    if [[ $4 = "Fed9USupervisor" ]];then
	usingSupervisor=1
    else 
	usingSupervisor=0
    fi
fi

VERSION=${MAJORVERSION}_$MINORVERSION
DOTVERSION=$MAJORVERSION.$MINORVERSION
#echo $DOTVERSION

#If we are not incrementing the release version, we should not tag cvs with the same version number.
if [[ $3 != "-noincr" ]];then

echo "$VERSION" >> .Fed9UReleaseVersion

cvs commit -m "Releasing update version $DOTVERSION" .Fed9UReleaseVersion
cvs rtag RV_$VERSION TrackerOnline/Fed9U

fi

rm -rf TempFed9URelease
mkdir TempFed9URelease
for f in Makevars ReleaseMakefile Makefile.in.template Makerules configure {Fed9UUtils,Fed9UNonStd}/{src/*.cc,include/*.hh,Makefile,obj/tar.obj} \
         Fed9UDescriptionFiles/{*.xml,*.xsd} ; do
  mkdir -p TempFed9URelease/$f
  rmdir TempFed9URelease/$f
  if [[ $f = ReleaseMakefile ]]; then
    cp $f TempFed9URelease/Makefile
  elif [[ $f = *.hh || $f = *.cc || $f = *.cxx ]]; then
    (head -n 1 licence.txt ; echo "// # Fed9U Software version $DOTVERSION") | cat - licence.txt > TempFed9URelease/$f
    if [[ $f = Fed9U*/src/Fed9U*Version.cc ]]; then
      scripts/MakeVersion.bash >> TempFed9URelease/$f
      tail -n +2 $f >> TempFed9URelease/$f
    else
      cat $f >> TempFed9URelease/$f
    fi
  else
    cp $f TempFed9URelease/$f
  fi
done
cd TempFed9URelease
tar -cvzf ../Fed9UUtils_$VERSION.tgz *
cd ..

rm -rf TempFed9URelease
mkdir TempFed9URelease
for f in Fed9UDeviceFactory/{src/*.cc,include/*.hh,Makefile,obj/tar.obj}  ; do
  mkdir -p TempFed9URelease/$f
  rmdir TempFed9URelease/$f
  if [[ $f = *.hh || $f = *.cc || $f = *.cxx ]]; then
    (head -n 1 licence.txt ; echo "// # Fed9U Software version $DOTVERSION") | cat - licence.txt > TempFed9URelease/$f
    if [[ $f = Fed9U*/src/Fed9U*Version.cc ]]; then
      scripts/MakeVersion.bash >> TempFed9URelease/$f
      tail -n +2 $f >> TempFed9URelease/$f
    else
      cat $f >> TempFed9URelease/$f
    fi
  else
    cp $f TempFed9URelease/$f
  fi
done
cd TempFed9URelease
tar -cvzf ../Fed9UDeviceFactory_$VERSION.tgz *
cd ..

rm -rf TempFed9URelease
mkdir TempFed9URelease
for f in Fed9UStandAlone/{Makefile,obj/tar.obj,src/FedDebugSuite.cxx,src/*.cc,include/*.hh} Fed9UVmeBase/Fed9UAddressTable.dat \
	           {Fed9UDevice,Fed9UVmeBase,Fed9UVmeDevice}/{Makefile,src/*.cc,include/*.hh,obj/tar.obj} \
                   Fed9UGuiConfigure/{Fed9UDebug,Fed9UDescriptionInterface}/{Makefile,.ui/*.h,.ui/*.cpp,.moc/moc_*.cpp,main.cpp}; do
  mkdir -p TempFed9URelease/$f
  rmdir TempFed9URelease/$f
  if [[ $f = *.hh || $f = *.cc || $f = *.cxx ]]; then
    (head -n 1 licence.txt ; echo "// # Fed9U Software version $DOTVERSION") | cat - licence.txt > TempFed9URelease/$f
    if [[ $f = Fed9U*/src/Fed9U*Version.cc ]]; then
      scripts/MakeVersion.bash >> TempFed9URelease/$f
      tail -n +2 $f >> TempFed9URelease/$f
    else
      cat $f >> TempFed9URelease/$f
    fi
  else
    cp $f TempFed9URelease/$f
  fi
done
cd TempFed9URelease
tar -cvzf ../Fed9ULib_$VERSION.tgz *
cd ..

if [[ "$usingSupervisor" = 1 ]]; then
rm -rf TempFed9URelease
mkdir TempFed9URelease
for f in Fed9USupervisor/{src/common/*.cc,include/*.hh,Makefile,obj/tar.obj,fed9uDefs.linux} ; do
  mkdir -p TempFed9URelease/$f
  rmdir TempFed9URelease/$f
  if [[ $f = *.hh || $f = *.cc || $f = *.cxx ]]; then
    (head -n 1 licence.txt ; echo "// # Fed9U Software version $DOTVERSION") | cat - licence.txt > TempFed9URelease/$f
    if [[ $f = Fed9U*/src/Fed9U*Version.cc ]]; then
      scripts/MakeVersion.bash >> TempFed9URelease/$f
      tail -n +2 $f >> TempFed9URelease/$f
    else
      cat $f >> TempFed9URelease/$f
    fi
  else
    cp $f TempFed9URelease/$f
  fi
done
cd TempFed9URelease
tar -cvzf ../Fed9USupervisor_$VERSION.tgz *
cd ..
fi

rm -rf TempFed9URelease

echo "#!/bin/bash

INSTALLVERSION=$VERSION
XERCES_VERSION=$XERCES_VERSION
`tail -n +2 scripts/FedInstall.bash`" >| FedInstall_$VERSION.bash



#Now we create the tarballs for the lib release.

tar -cvzf LibFed9UUtils_$VERSION.tgz Fed9UUtils/libFed9UUtils.so Fed9UUtils/include/Fed9UUtils.hh Fed9UDescriptionFiles
tar -cvzf LibFed9UDeviceFactory_$VERSION.tgz Fed9UDeviceFactory/libFed9UDeviceFactory.so Fed9UDeviceFactory/include/Fed9UDeviceFactoryLib.hh
tar -cvzf LibFed9ULib_$VERSION.tgz Fed9UDevice/libFed9ULib.so Fed9UDevice/include/Fed9ULib.hh Fed9UStandAlone/FedDebugSuite.exe Fed9UVmeBase/Fed9UAddressTable.dat Fed9UGuiConfigure/Fed9UDebug/Fed9UDebug  Fed9UGuiConfigure/Fed9UDescriptionInterface/libFed9UDescriptionInterface.so
if [[ "$usingSupervisor" = 1 ]]; then
tar -cvzf LibFed9USupervisor_$VERSION.tgz Fed9USupervisor/lib/linux/x86/libFed9USupervisor.so
fi


#move the release tarballs into the release directory and upload the lib tarballs to the website
mkdir release/Fed9URelease_$VERSION
mkdir release/Fed9URelease_$VERSION/xerces$2
mv LibFed9UUtils_$VERSION.tgz LibFed9UDeviceFactory_$VERSION.tgz LibFed9ULib_$VERSION.tgz Fed9UUtils_$VERSION.tgz Fed9UDeviceFactory_$VERSION.tgz Fed9ULib_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/
if [[ "$usingSupervisor" = 1 ]]; then
mv LibFed9USupervisor_$VERSION.tgz Fed9USupervisor_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/
fi

if yesno "Do you want to upload release to server ? " "n" ; then
    echo "Sending release version $DOTVERSION to web server..."
    scp release/Fed9URelease_$VERSION/xerces$2/LibFed9UUtils_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/LibFed9ULib_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/LibFed9USupervisor_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/libFed9UDeviceFactory_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/Fed9UUtils_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/Fed9UDeviceFactory_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/Fed9ULib_$VERSION.tgz release/Fed9URelease_$VERSION/xerces$2/Fed9USupervisor_$VERSION.tgz fulcher@lxplus.cern.ch:$1/xerces$2
    
fi

#rm -f LibFed9UUtils*.tgz LibFed9ULib*.tgz LibFed9USupervisor*.tgz
#rm -f FedInstall$VERSION.bash Fed9UUtils_$VERSION.tgz Fed9ULib_$VERSION.tgz Fed9USupervisor_$VERSION.tgz
