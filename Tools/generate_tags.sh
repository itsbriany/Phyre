#!/bin/sh

# You will need cscope and ctags to run this script
if [ -z $PHYRE_ROOT ]; then
    echo "Please point the PHYRE_ROOT environment variable to the Phyre root directory"
    exit 1
fi
rm cscope.out
find . -path ./ThirdParty -prune -o -name "*.cpp" | grep -v ThirdParty > $PHYRE_ROOT/cscope.files
find . -path ./ThirdParty -prune -o -name "*.h"  | grep -v ThirdParty >> $PHYRE_ROOT/cscope.files
cscope -b
ctags -R .
