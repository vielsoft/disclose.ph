#!/bin/sh
#
# Cloud Hook: post-code-update
#
# The post-code-update hook runs in response to code commits.

site="$1"
target_env="$2"
source_branch="$3"
deployed_tag="$4"
repo_url="$5"
repo_type="$6"

drush -vdy @discloseph.test cc all
drush -vdy @discloseph.test updb
drush -vdy @discloseph.test fra
drush -vdy @discloseph.test cc all

if [ "$source_branch" != "$deployed_tag" ]; then
    echo "$site.$target_env: Updated branch $source_branch as $deployed_tag."
else
    echo "$site.$target_env: Updated $deployed_tag."
fi
