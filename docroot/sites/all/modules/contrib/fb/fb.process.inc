<?php

/**
 * @file
 * Helper functions for processing links.
 *
 * Both canvas pages and tab pages need to alter links, changing both URLs and
 * attributes such as target.  This cannot always be done via Drupal's url
 * rewriting.
 */


/**
 * This function uses regular expressions to convert links on canvas pages
 * to URLs that begin http://apps.facebook.com/...
 *
 * This is a relatively expensive operation.  In the past, this had to be run
 * on all FBML canvas pages.  Now with iframe canvas pages, this is not
 * strictly needed, but remains useful.  If processed, links on the page will
 * change the parent frame (the one with the URL shown in the browser).  If
 * not processed, the links will change only the iframe.
 *
 *
 * @param $output is the page (or iframe block) about to be returned.
 *
 * @param $options - 'add_target' will cause target=_top to be added
 * when producing an iframe. 'absolute_links' will change hrefs with absolute
 * URLs to refer to canvas pages.
 *
 */
function fb_process($output, $options = array()) {
  global $base_url;
  global $_fb, $_fb_app;

  // Default to canvas links.
  if (!isset($options['to_canvas'])) {
    $options['to_canvas'] = $_fb_app->canvas;
  }

  $patterns = array();
  $replacements = array();
  $base_path = base_path();
  $targets_added = FALSE;

  if ($_fb) {
    if (function_exists('fb_url_outbound_alter')) {
      $rewrite_options = array(
        FB_SETTINGS_CB_SESSION => FALSE,
        FB_SETTINGS_CB_TYPE => FALSE,
      );
      $base_rewrite = '';
      fb_url_outbound_alter($base_rewrite, $rewrite_options, '');
      $base = $base_path . $base_rewrite;  // short URL with rewrite applied.
    }
    else {
      // If no url_alter, use normal base_path.
      $base = $base_path;
    }

    $protocol = fb_protocol(); // http or https
    if (!isset($options['relative_links']) || $options['relative_links']) {
      if (isset($options['to_canvas']) && ($canvas = $options['to_canvas'])) {
        // Make relative links point to canvas pages.
        $patterns[] = "|<a([^>]*)href=\"{$base}|";
        $replacements[] = "<a $1 href=\"$protocol://apps.facebook.com/{$canvas}/";
      }
      elseif (!empty($options['to_page'])) {
        // Make relative links point to page tab.
        $to_page = $options['to_page'];

        // Note that url parameters will be lost on page tabs.  Facebook will not pass them on to our iframe.
        $patterns[] = "|<a([^>]*)href=\"{$base}([^\"\?]*)\?|";
        $replacements[] = "<a $1 href=\"$to_page" . "&app_data=$2&";

        // Pattern without URL params.
        $patterns[] = "|<a([^>]*)href=\"{$base}|";
        $replacements[] = "<a $1 href=\"$to_page" . "&app_data=";
      }
      else {
        // Make relative links point to website.
        $site_base = url('<front>', array(
                           'absolute' => TRUE,
                           'fb_url_alter' => FALSE,
                         ));
        $patterns[] = "|<a([^>]*)href=\"{$base}|";
        $replacements[] = "<a $1 href=\"{$site_base}";
      }

      // Add target=_top so that entire pages do not appear within an iframe.
      // TODO: make these pattern replacements more sophisticated, detect whether target is already set.
      if (isset($options['add_target']) && ($target = $options['add_target'])) {
        // Add target=_top to all links
        $patterns[] = "|<a ([^>]*)href=\"|";
        $replacements[] = "<a $1 target=\"{$target}\" href=\"";
        // Do not change local forms, but do change external ones
        $patterns[] = "|<form([^>]*)action=\"([^:\"]*):|";
        $replacements[] = "<form target=\"{$target}\" $1 action=\"$2:";
        $targets_added = TRUE;
      }
    }

    if (!$targets_added) {
      // Add target=_top to only external links
      $patterns[] = "|<a([^>]*)href=\"([^:\"]*):|";
      $replacements[] = "<a target=\"_top\" $1 href=\"$2:";
      $patterns[] = "|<form([^>]*)action=\"([^:\"]*):|";
      $replacements[] = "<form target=\"_top\" $1 action=\"$2:";
    }

    if (isset($options['absolute_links']) && $options['absolute_links']) {
      // Make absolute links point to canvas pages.
      $absolute_base = url('<front>', array('absolute' => TRUE));
      $patterns[] = "|<a([^>]*)href=\"{$absolute_base}|";
      $replacements[] = "<a $1 href=\"$protocol://apps.facebook.com/{$_fb_app->canvas}/";
    }
  }
  if (count($patterns)) {
    $count = 0;
    $return = preg_replace($patterns, $replacements, $output, -1, $count);
    //print ("fb_process replaced $count.\n\n"); // debug
    return $return;
  }
  else
    return $output;
}
