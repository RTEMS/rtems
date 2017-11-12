#! /bin/sh

git=$(command -v git)

#
# Git command not found or not a valid git repo is a release.
#
vc_ident="release"

if test $# -ge 1; then
 repo=$1
 shift
 if test -d $repo; then
  cwd=$(pwd)
  cd $repo
  if test -n ${git}; then
   git rev-parse --git-dir > /dev/null 2>&1
   if test $? = 0; then
    git status > /dev/null 2>&1
    if git diff-index --quiet HEAD --; then
     modified=""
    else
     modified="-modified"
    fi
    vc_ident="$(git rev-parse --verify HEAD)${modified}"
    if test $# -ge 1; then
     if test "${vc_ident}" = "$1"; then
      vc_ident="matches"
     fi
    fi
   fi
  fi
  cd $cwd
 fi
fi

echo ${vc_ident}

exit 0
