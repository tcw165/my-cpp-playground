#!/bin/sh

clear

git submodule update --init
if [ ! -f ./vcpkg/bootstrap-vcpkg.sh ]; then
    echo "Can't find vcpkg/bootstrap-vcpkg.sh"
    exit 1
fi

# Copy the vcpkg wrapper that uses the local vcpkg executable under vcpkg/ folder.
if [ ! -f /usr/local/bin/vcpkg ]; then
    cp -v bootstrap/vcpkg.sh /usr/local/bin/vcpkg
fi
if [ ! -x /usr/local/bin/vcpkg ]; then
    chmod -v +x /usr/local/bin/vcpkg
fi
vcpkg install catch2 rxcpp boost folly

# Show the instsall packages
vcpkg list
echo "---"
echo "You're all set!"