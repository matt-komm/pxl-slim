#!/bin/sh

# script_functions.sh
# pxl
#
# Created by Robert Fischer on 12.08.09.
# Copyright 2009. All rights reserved.


PATH=/usr/local/bin:$PATH:/opt/local/bin
export VERSIONER_PYTHON_VERSION=$PYTHON_VERSION

run_configure_script (){

	alias libtoolize=glibtoolize
	pxl_root_dir="${SRCROOT}/.."

	cd ${pxl_root_dir}
	./configure $@
	if [ ! $? -eq 0 ]; then
		echo "Could not run configure. Try running autogen.sh"
		./autogen.sh
		./configure $@
	fi

}