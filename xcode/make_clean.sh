#!/bin/sh

# make_clean.sh
# pxl
#
# Created by Robert Fischer on 22.11.09.

cd "${SRCROOT}/.."

if [ "x$ACTION" == "xclean" ]; then
	echo "Running make clean..."
	make distclean
fi

exit 0