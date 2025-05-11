#!/bin/bash
make distclean 2>/dev/null
rm -rf autom4te.cache libs/cjson/.deps
rm -f aclocal.m4 configure config.h config.h.in config.log config.status
rm -f Makefile Makefile.in src/Makefile src/Makefile.in
rm -f install-sh missing depcomp compile
rm -f *.o *.lo *.la */*.o */*.lo */*.la
rm -f skareader src/skareader
rm -f bin/skareader