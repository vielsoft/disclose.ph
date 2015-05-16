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
$base_url = 'http://disclose.ph.local';

/**
 * Database settings.
 */
$databases = array(
  'default' => array(
    'default' => array(
      'database' => '',
      'username' => '',
      'password' => '',
      'host' => '',
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
