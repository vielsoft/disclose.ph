Disclose
========
An opensource community driven reporting web application based on Drupal.

# To install Drupal
* Import the database from assets folder.
* Copy the `default.local.settings.php` to `local.settings.php` and update the
`$database` with correct credentials.

# Requirements
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

To install Gulp:
```
$ sudo npm install
$ sudo npm install gulp-cli -g
```

To compile SASS you need Compass:
```
$ sudo gem install compass
```

If you dont have Gem installed you need to install [Ruby](https://www.ruby-lang.org/en/installation/).

# Working with Gulp

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
