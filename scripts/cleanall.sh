#!/bin/bash

NAMES=(
  ".deps"
  "Makefile.in"
)
for NAME in "${NAMES[@]}"; do
  find . \( -type d -o -type f \) -name "$NAME" -exec rm -rf {} +
done


rm -rf autom4te.cache install result_conversations

rm -f aclocal.m4 configure config.h config.h.in config.log config.status

rm -f install-sh missing depcomp compile
