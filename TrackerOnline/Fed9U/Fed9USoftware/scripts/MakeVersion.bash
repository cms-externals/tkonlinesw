#!/bin/bash

VERSION=`tail -n 1 .Fed9UReleaseVersion`
MAJORVERSION=${VERSION%*_*}
MINORVERSION=${VERSION#*_*}

echo "static const unsigned Fed9UVersion[2] = { $MAJORVERSION, $MINORVERSION };"
