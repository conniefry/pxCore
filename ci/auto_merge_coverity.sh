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

create_all_branches()
{
    # Keep track of where Travis put us.
    # We are on a detached head, and we need to be able to go back to it.
    local build_head=$(git rev-parse HEAD)

    # Fetch all the remote branches. Travis clones with `--depth`, which
    # implies `--single-branch`, so we need to overwrite remote.origin.fetch to
    # do that.
    git config --replace-all remote.origin.fetch +refs/heads/*:refs/remotes/origin/*
    git fetch
    # optionally, we can also fetch the tags
    # git fetch --tags

    # reate the tracking branchescreate the tracking branches
    #for branch in $(git branch -r|grep -v HEAD) ; do
    #    git checkout -qf ${branch#origin/}
    #done

    # finally, go back to where we were at the beginning
    git checkout ${build_head}
}

echo Automatically merge master to coverity_scan branch

create_all_branches 

#git checkout jr_master
git checkout origin/coverity_scan && git merge origin/jr_master

git push --repo="https://$REPO_USER_NAME:$GH_TOKEN@github.com/$REPO_USER_NAME/$REPO_NAME.git"
checkError $? "unable to commit data to repo" "" "check the credentials"
