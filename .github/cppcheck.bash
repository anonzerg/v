#!/usr/bin/env bash

cppcheck --xml \
  --error-exitcode=1 \
  --check-level=exhaustive \
  --enable=all \
  --inconclusive \
  --suppress=missingIncludeSystem \
  --suppress=unusedFunction \
  --suppress=checkersReport \
  --suppress=unmatchedSuppression \
  -I src/v -I src/mol -I src/math -I src/sym \
  --inline-suppr \
  src/
