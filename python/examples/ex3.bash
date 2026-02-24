#!/usr/bin/env bash

echo '.' | python -c "import vmol; print(vmol.main.run(['../../mol/rotation0.in', 'gui:0'])[1], end='')"
