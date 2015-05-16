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
 * Use base url for your specific development url
 */
$base_url = 'http://disclose.ph.local';

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


/*
 * Disabling cache on local development
 *
 * This is handy when database import is done, then disable cache altogther on local dev
 */

$conf['cache'] = FALSE;
$conf['preprocess_css'] = FALSE;
$conf['preprocess_js'] = FALSE;
$conf['block_cache'] = FALSE;


/*
 * Amazon S3 bucket name
 *
 * Making sure that all local development environments have bucket name preset. Handy when forgetting to update bucket name.
 */
$conf['amazons3_bucket'] = 'dev-insomniac';
