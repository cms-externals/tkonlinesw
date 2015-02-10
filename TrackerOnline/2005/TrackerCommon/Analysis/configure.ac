#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(TrackerCommon, development, mirabito@cern.ch)

echo ""
echo "TrackerCommon/Analysis configuration: " 
echo "-------------------------------------"

# -------------------------------------------------------------------------------------------
#
#    Required Settings 
#
# -------------------------------------------------------------------------------------------

AC_ARG_WITH(root-path, 
            [AC_HELP_STRING([--with-root-path],[specify the path to Root])],  
	    [root_root=$withval], 
            []) 
AS_IF([test "x$rootsys" == x], 
      AS_IF([test "x$ROOTSYS" != x], 
             [rootsys=$ROOTSYS; echo "ROOTSYS: $ROOTSYS"], 
	     [echo " "; 
              echo "************************************************************************************";
	      echo "Root libraries are required. Please set the root path using --with-root-path=<path>"; 
	      echo "or using the ROOTSYS environment variable";
	      echo "************************************************************************************"; 
              echo " "; exit; 
            ])
     )


# -------------------------------------------------------------------------------------------
#
#    Now, substitute
#
# -------------------------------------------------------------------------------------------

AC_SUBST(rootsys, $rootsys)

AC_CONFIG_FILES([makefile:makefile.in])
AC_OUTPUT

echo ""
echo ""
