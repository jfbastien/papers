#! /bin/bash

set -e
set -u

# Deploy generated html pages to github.io.

BUILD=./build/
HTML=$BUILD/html
DIR=$BUILD/jfbastien.github.io
PAPERS=$DIR/papers
CLONE=git@github.com:jfbastien/jfbastien.github.io.git

HASH=$(git rev-parse HEAD)
SUBJECT=$(git log -n1 --pretty=format:%s)

# Hacky reuse of git's require_clean_work_tree.
OPTIONS_SPEC=
LONG_USAGE=
USAGE=
NONGIT_OK=
SUBDIRECTORY_OK=
source $(git --exec-path)/git-sh-setup ""
require_clean_work_tree deploy "Please commit or stash changes."

# Copy generated html files to the github.io repo.
rm -rf $DIR
mkdir $DIR
git clone $CLONE $DIR
find $HTML/*.html -maxdepth 1 -type f \
  \( -iname "*.html" ! -iname "genindex.html" ! -iname "search.html" \) | \
  xargs -I{} cp {} $PAPERS/

# Commit the changes, and deploy them.
pushd $PAPERS
git status
git add "*.html"
git commit -m "Update '$SUBJECT'

Hash: $HASH"
git push origin master
popd
