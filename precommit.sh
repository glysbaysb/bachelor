#!/bin/bash
# install: ln -s pre-commit.sh .git/hooks/pre-commit
STASH_NAME="pre-commit-$(date +%s)"
git stash save -q --keep-index $STASH_NAME

./run-tests.sh

STASHES=$(git stash list)
if [[ $STASHES == "$STASH_NAME" ]]; then
  git stash pop -q
fi
