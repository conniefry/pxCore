#!/bin/sh
if [ "$TRAVIS_EVENT_TYPE" = "cron" ] ; then exit 1; fi


# cov-configure --template --comptype g++ --compiler gcc-6 
# or 
cov-configure --template --comptype gcc --compiler gcc-5 
cov-configure --template --comptype g++ --compiler gcc-5 
cov-configure --template --comptype javascript --compiler javascript
# cov-build --dir cov-int --fs-capture-search src --fs-capture-search examples/pxScene2d/src --no-command --fs-capture-search-exclude-regex examples/pxScene2d/external --fs-capture-search-exclude-regex examples/pxScene2d/src/duk_modules --fs-capture-search-exclude-regex examples/pxScene2d/src/node_modules --fs-capture-search-exclude-regex examples/pxScene2d/src/v8_modules --fs-capture-search-exclude-regex examples/pxScene2d/src/tests --fs-capture-search-exclude-regex examples/pxScene2d/src/optimus --fs-capture-search-exclude-regex examples/pxScene2d/src/treeControl

