#!/bin/bash

if [ ! -f $PWD/vcpkg/vcpkg ]; then
    echo "Error: There's no vcpkg submodule or vcpkg/vcpkg executable in $PWD"
    if [ -d $PWD/vcpkg ]; then
        echo "Cause: You do have vcpkg clone but no executable. So you may want to run $PWD/vcpkg/bootstrap-vcpkg.sh"
    else
        echo "Cause: You do not have vcpkg clone. Please consider checking out https://github.com/microsoft/vcpkg.git as vcpkg locally here."
    fi
    exit 1
fi

# Set the root so that vcpkg could match the right triplet.
if [ "$VCPKG_ROOT" != "$PWD/vcpkg" ]; then
    export VCPKG_ROOT=$PWD/vcpkg
fi

$PWD/vcpkg/vcpkg ${@:1}