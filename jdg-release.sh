#!/bin/bash

JDG_REMOTE=jdg
REDHAT_REMOTE=redhat
JDG_BRANCH=jdg-7.1.x
TEMP_BRANCH=__tmp
VERSION=8.4.0

# dist-git setup
DIST_GIT_REPO=infinispan-cpp-client
DIST_GIT_RHEL7_BRANCH=jb-dg-7-rhel-7
BRANCHES="$DIST_GIT_RHEL7_BRANCH"

USERNAME=`whoami`
FULLNAME=`getent passwd "$USERNAME" | cut -d ':' -f 5`
DATE=`date +"%a %b %d %Y"`


function prepare-src() {
	echo "Preparing release for milestone $1"
	rm -fr build
	mkdir build
	pushd build 2> /dev/null
	cmake ..
	cpack --config CPackSourceConfig.cmake
	popd
}

function prepare-dist-git() {
    rhpkg clone $DIST_GIT_REPO
    pushd $DIST_GIT_REPO
    for i in $BRANCHES;
    do
        if [ -f ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip ]; then
            git checkout $i

            # Update .spec file
            PREV_VERSION=`grep "Version:" infinispan-cpp-client.spec | sed -e s/"Version:\s*"//`
 
            if [ $PREV_VERSION != $VERSION.$1 ]; then
                sed -i "s/Version:\s*$PREV_VERSION/Version:\t$VERSION.$1/" infinispan-cpp-client.spec
            fi
             
            # Update sources
            cp ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip .
            rhpkg new-sources infinispan-hotrod-cpp-$VERSION.$1-Source.zip

            # Commit and push
            git rm --cached infinispan-cpp-client.spec && git add infinispan-cpp-client.spec
            git commit -a -m "$VERSION.$1-$RELEASE" && rhpkg push && git rev-parse HEAD
        else
            echo "Cannot find ../build/infinispan-hotrod-cpp-$VERSION.$1-Source.zip"
            exit 1
        fi
    done
    popd
}

function usage() {
	echo "Usage: $0 [--prepare MILESTONE|--perform]"
	exit 1
}

case "$1" in
    "--prepare" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-src "$2"
        prepare-dist-git "$2" "$3"
        ;;
    "--prepare-src" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-src "$2"
        ;;
    "--prepare-dist-git" )
    	if [ "$#" -lt 2 ]; then
	    usage
	fi
        prepare-dist-git "$2" "$3"
        ;;
    *)
        usage
        ;;
esac

