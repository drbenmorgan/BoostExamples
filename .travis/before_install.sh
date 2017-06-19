#!/bin/bash
set -ex

# This sets up script-time dependencies
# - Docker on Linux OS
# - macOS homebrew/cadfaelbrew
#
# Depends on Travis CI Environment:
# - TRAVIS_OS_NAME
# - (TODO) docker image id

if [[ "$TRAVIS_OS_NAME" == "linux" ]] ; then
  # May also use env var here to choose image
  docker pull benmorgan/supernemo-ubuntu:3.0
  # ... run detached, mounting volumes...
elif [[ "$TRAVIS_OS_NAME" == "osx" ]] ; then
  # Tap, pin and install...
  brew ls --versions
fi


