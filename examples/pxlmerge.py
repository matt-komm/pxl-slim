#! /usr/bin/env python

import sys

from pxl.io import *
from pxl.base import *
from pxl.hep import *

if __name__ == "__main__":
    print "pxlmerge.py <outputfile> <inputfile1> <inputfile2> ..."
    outputfile=sys.argv[1]
    inputfiles=sys.argv[2:]
    
    output=OutputFile(outputfile)
    for inputfile in inputfiles:
        print "reading ",inputfile
        input=InputFile(inputfile)
        while input.good():
            object=input.readNextObject()
            if object!=None:
                output.streamObject(object)
        input.close()
    print "writing ",outputfile
    output.close()
