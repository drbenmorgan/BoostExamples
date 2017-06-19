#!/bin/bash
set -ev

pwd
ls -larth
env
git status
git log -n2


if [[ "$TRAVIS_OS_NAME" == "linux" ]] ; then
  CONTAINER_CMD="docker exec docker-cmake"
fi

$(CONTAINER_CMD) ls -larth


