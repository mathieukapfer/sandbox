#!/bin/bash

for i in "$@"; do
    awk '{print "/*" FILENAME ":" NR ":" "*/" $0}' $i;
done
