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
      'database' => 'YOUR_DRUPAL_DATABASE_NAME',
      'username' => 'YOUR_MYSQL_USERNAME',
      'password' => 'YOUR_MYSQL_PASSWORD_IF_YOU_SET_IT_OTHERWISE_BLANK',
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
