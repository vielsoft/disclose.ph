#!/bin/sh
#
# Cloud Hook: post-code-update
#
# The post-code-update hook runs in response to code commits. When you push commits to a Git branch, the
# post-code-update hooks runs for each environment that is currently running that branch.
#
# Usage: post-code-update site target-env source-branch deployed-tag repo-url
#                         repo-type

site="$1"
target_env="$2"
source_branch="$3"
deployed_tag="$4"
repo_url="$5"
repo_type="$6"

drush -vdy @discloseph.dev cc all
drush -vdy @discloseph.dev updb
drush -vdy @discloseph.dev fra
drush -vdy @discloseph.dev cc all

if [ "$source_branch" != "$deployed_tag" ]; then
    echo "$site.$target_env: Updated branch $source_branch as $deployed_tag."
else
    echo "$site.$target_env: Updated $deployed_tag."
fi
