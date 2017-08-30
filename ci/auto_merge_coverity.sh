#!/bin/sh
# Automatically merge master to coverity_scan branch
checkError()
{
  if [ "$1" -ne 0 ]
  then
    echo "auto_merge_coverity failed with errors !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
    echo "failure reason: $2"
    echo "Cause: $3"
    echo "Reproduction/How to fix: $4"
    exit 1
  fi
}


echo Automatically merge master to coverity_scan branch

git fetch origin
git checkout jr_master
git checkout coverity_scan && git merge jr_master

git push --repo="https://$REPO_USER_NAME:$GH_TOKEN@github.com/$REPO_USER_NAME/$REPO_NAME.git"
checkError $? "unable to commit data to repo" "" "check the credentials"

