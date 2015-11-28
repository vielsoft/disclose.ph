Disclose
========
An opensource community driven reporting platform based on Drupal.

* DisclosePH is a centralize repository of events (e.g crimes, weather report, natural disaster, accidents, politics, economy, opinion, etc) contributed by the public.
* DisclosePH is free of use by public.

## What this platform trying to solve?
* This will allow everyone to participate in bringing the news to the rest of the community.
* This will serve as a platform to fight crimes.
* Promotes transparency and bayanihan (support to fellow citizen).
* It will provide data for statistical purposes.
* Centralize source of information.

## To install Drupal
* Import the database from assets folder.
* Copy the `default.local.settings.php` to `local.settings.php` and update the
`$database` with the correct credentials.
* CMS login: `admin/admin`

Local settings:
```
<?php
/**
 * @file
 * Local settings for local environment.
 *
 * Enable aggresive error reporting.
 */
error_reporting(E_ALL);
ini_set('display_errors', TRUE);
ini_set('display_startup_errors', TRUE);

/**
 * Use base url for your specific development url.
 */
$base_url = 'http://' . $_SERVER['HTTP_HOST'];

/**
 * Database settings.
 */
$databases = array(
  'default' => array(
    'default' => array(
      'database' => 'YOUR_DATABASE_NAME',
      'username' => 'YOUR_MYSQL_USERNAME',
      'password' => 'YOUR_MYSQL_PASSWORD_IF_YOU_SET_TO_HAVE_PASSWORD',
      'host' => 'localhost',
      'port' => '3306',
      'driver' => 'mysql',
      'prefix' => '',
    ),
  ),
);

/**
 * Disabling cache on local development. This is handy when database
 * import is done, then disable cache altogether on local dev.
 */
$conf['cache'] = 0;
$conf['preprocess_css'] = 0;
$conf['preprocess_js'] = 0;
$conf['block_cache'] = 0;
$conf['theme_debug'] = 1;
```

## Requirements
* Drush
* Gulp
* PHP CodeSniffer
* Compass

To install Drush via Pear (on Ubuntu):
```
$ sudo apt-get install php-pear
$ sudo pear channel-discover pear.drush.org
$ sudo pear install drush/drush
$ sudo drush
$ sudo chown -R $USER:$USER ~/.drush
```

If you're on Ubuntu you can try this alternative [this Drush installer](https://github.com/geraldvillorente/drush-installer)

## PHP Code Linting
To check your PHP for code standards and syntax, run these commands.

1. `composer install` # One time only
1. `composer run-script phpcs`
1. `composer run-script check-syntax`

Check the coding standard here: [Drupal coding standard](https://github.com/VielSoft/disclose.ph/blob/develop/coding_standard.md)

## Theme development
The Insomniac Theme is located here:

* `docroot/sites/all/themes/disclose/`

All the JavaScript and CSS sources are placed under the following:

* `docroot/themes/disclose/js` - JS files
* `docroot/themes/disclose/sass` - CSS sources written in SASS using Compass framework.

**To install Gulp:**
```
$ sudo npm install
$ sudo npm install gulp-cli -g
```

**To compile SASS you need Compass:**
```
$ sudo gem install compass
$ sudo gem install sass
```

If you dont have Gem installed you need to install [Ruby](https://www.ruby-lang.org/en/installation/).

If you're using Ubuntu 14.04 you can install it via `apt-get`:
```
$ sudo apt-get install rubygems-integration
```

**Working with Gulp**

Each time you change files under `sites/all/themes/disclose/` you need to run the following:
```
$ gulp build
```

To compile without JS and CSS compression (local development only) run:
```
$ gulp dev
```

which is the same as the following single command:
```
$ gulp
```

To compile CSS source only run:
```
$ gulp styles
```

And to compile JS source only:
```
$ gulp scripts
```

To optimize all available images:
```
$ gulp images
```

To make your development easier you can run a task which will watch your changes and automatically recompile the sources:
```
$ gulp watch
```

## Working with Github

**Branching**

The current development branch is `develop`.
The release branch is `release/version-[number]`.

**Branch Conventions**

* `master` - For prod releases.
* `develop` - For development. All feature branches are fork from develop. (e.g. `feature/issue-1`)
* `release/version-[number]` - For stable releases while the subsequent release is being merged to develop.
* `feature/issue-[number]` - Naming convention for feature branchs. The `[number]` is refer to the Github issue number.
* `hotfix/version-[number]-hotfix-[number]` - Naming convention for hotfixes. The `version-[number]` is the release version while the `hotfix-[number]` is the number of hotfix attempt.

**Commiting**

To close a ticket automatically via your commit:
[Read this documentation](https://help.github.com/articles/closing-issues-via-commit-messages/)

**Merging**

We use PR to merge the commits. This ensure the quality and consistency of our code. See the doc [here](https://help.github.com/articles/creating-a-pull-request/).
Another doc [here](https://help.github.com/articles/using-pull-requests/).

**Creating a New Issue**

To file a new issue:
Be sure to file your issue under the correct milestone.

## Environments

To access our dev and staging environment you need first to have an account on Acquia cloud. Once you have an account create an issue [here](https://github.com/VielSoft/disclose.ph/milestones/Contributing) that you are asking for invitation.

* Prod - Currently we dont have production server. Looking for a sponsor to provide us a free server.
* Test - [http://disclosephhogcjmdqel.devcloud.acquia-sites.com](http://disclosephhogcjmdqel.devcloud.acquia-sites.com)
* Dev  - [http://disclosephwzybfnbsu8.devcloud.acquia-sites.com](http://disclosephwzybfnbsu8.devcloud.acquia-sites.com)

## Communication Platform

We are using [Slack](https://disclose.slack.com) to communicate. Plase create a ticket on [here](https://github.com/VielSoft/disclose.ph/issues) that you are asking for invitation.
