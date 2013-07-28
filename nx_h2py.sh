#!/bin/bash

h2xml "$DEVEL_DIR"/virg/virg_nexus/include/virg/nexus/"$@".h -I"$DEVEL_DIR"/virg/virg_nexus/include -o /tmp/"$@".xml -q -c
xml2py /tmp/"$@".xml -r "NX.*|nx.*" -l "$DEVEL_DIR"/virg/virg_nexus/lib/libvirg-nexus.so
