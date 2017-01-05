#!/bin/sh

rm -r build-coverage
mkdir -p build-coverage
pushd build-coverage

cmake -GNinja -DENABLE_COVERAGE=ON  ..

ninja && ninja test

if [ "x$?" = "x0" ]; then 

    lcov --base-directory . --directory . -c -o vistaio.info

    # remove system library files 
    lcov --remove vistaio.info "/usr*" -o vistaio.info
    lcov --remove vistaio.info "vistaio/test_*" -o vistaio.info

    # generate the html report, note that genhtml may have some problems with a few files
    # that will currently need to be removed manually
    rm -rf test-coverage

    genhtml -o test-coverage -t "VistaIO coverage" --num-spaces 2 vistaio.info
else
    echo build or test failed 
fi

popd


firefox build-coverage/test-coverage/index.html

