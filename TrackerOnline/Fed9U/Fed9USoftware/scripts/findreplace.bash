find . -name *.cc | awk -F, '{ print "cat " $1 " | sed s/VERIFY/ICUTILS_VERIFY/g >" $1 ".tmp"}' > replaceccV.bash
