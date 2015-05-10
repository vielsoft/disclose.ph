<?php

/**
 * @file
 * Drupal administration of fb_tab.module.
 */

/**
 * Implements hook_fb_admin().
 *
 */
function fb_tab_fb_admin($op, $data, &$return) {
  $fb = isset($data['fb']) ? $data['fb'] : NULL;
  $fb_app = isset($data['fb_app']) ? $data['fb_app'] : NULL;
  $config = _fb_tab_get_app_config($fb_app);
  if ($op == FB_ADMIN_OP_SET_PROPERTIES) {
    $return['tab_default_name'] = $config['tab_default_name'];

    if ($config['tab_default_name']) {
      if (function_exists('fb_url_inbound_alter')) {
        $callback_url = url('', array('absolute' => TRUE)) . FB_SETTINGS_CB . '/' . $fb_app->id . '/';
      }
      else {
        // Paving the way to make URL alters optional.
        $callback_url = url('', array('absolute' => TRUE));
      }
      $return['page_tab_url'] = $callback_url . FB_TAB_PATH_VIEW;

      if (variable_get(FB_VAR_SECURE_URLS, FB_SECURE_URLS_SOMETIMES) >= FB_SECURE_URLS_SOMETIMES) {
        $return['secure_page_tab_url'] = str_replace('http://', 'https://', $return['page_tab_url']) . '/'; // Must end with '/' ???
      }
      if (variable_get(FB_VAR_SECURE_URLS, FB_SECURE_URLS_SOMETIMES) <= FB_SECURE_URLS_SOMETIMES) {
        $return['page_tab_url'] = str_replace('https://', 'http://', $return['page_tab_url']);
      }
    }
    else {
      $return['page_tab_url'] = '';
    }

  }
  elseif ($op == FB_ADMIN_OP_LIST_PROPERTIES) {
    $return[t('Page Tab Name')] = 'page_tab_default_name';
    $return[t('Page Tab URL')] = 'page_tab_url';
    $return[t('Secure Page Tab URL')] = 'secure_page_tab_url';
  }
}


/**
 * Form builder; Configure settings for this site.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function fb_tab_admin_settings() {

  $form['process_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tab URL processing'),
    '#description' => t('This option alters links, so that instead of changing the iframe\'s URL, they change the top frame.  This adds some overhead to each tab served. Still, most sites will want this enabled.'),
  );

  $process_options = array(
    FB_TAB_PROCESS_IFRAME_NONE => t('No processing'),
    FB_TAB_PROCESS_IFRAME_TO_CANVAS => t('Link to canvas pages.  (Your app must support canvas pages.)'),
    FB_TAB_PROCESS_IFRAME_TO_TAB_VIA_APP_DATA => t('Stay on page tab. (Uses the special "app_data" url parameter to express local path.)'),
  );
  $form['process_settings'][FB_TAB_VAR_PROCESS_IFRAME] = array(
    '#type' => 'radios',
    '#title' => t('Link processing on iframe tabs.'),
    '#options' => $process_options,
    '#default_value' => variable_get(FB_TAB_VAR_PROCESS_IFRAME, FB_TAB_PROCESS_IFRAME_TO_CANVAS),
    '#description' => t('When a link appears on a page tab, should the linked-to content appear in the page tab, a canvas page, or some other way.  Note that without any processing, links may have unexpected behavior.'),
  );
  $form['process_settings'][FB_TAB_VAR_PROCESS_ABSOLUTE] = array(
    '#type' => 'checkbox',
    '#title' => t('Process absolute hrefs, not just relative links.  Adds target=_top attribute.'),
    '#default_value' => variable_get(FB_TAB_VAR_PROCESS_ABSOLUTE, TRUE),
  );

  return system_settings_form($form);
}


/**
 * See fb_tab_form_alter.
 */
function fb_tab_admin_form_alter(&$form, &$form_state, $form_id) {
  // Add our settings to the fb_app edit form.
  if (isset($form['fb_app_data']) && is_array($form['fb_app_data'])) {
    $fb_app = $form['#fb_app'];
    $config = _fb_tab_get_app_config($fb_app);

    $form['fb_app_data']['fb_tab'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => isset($fb_app->label),
      '#title' => t('Facebook profile tabs'),
      '#description' => t('Settings which apply to <a href=!url target=_blank>profile tabs</a>.',
                          array('!url' => 'http://developers.facebook.com/docs/guides/canvas/#tabs')),
    );

    // Override themes
    $themes = system_get_info('theme');
    ksort($themes);
    $theme_options[0] = t('System default');
    foreach ($themes as $key => $theme) {
      $theme_options[$key] = $theme['name'];
    }
    $form['fb_app_data']['fb_tab']['custom_theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme for profile tabs'),
      '#description' => t('Choose a theme designed to return FBML specifically for the 520 pixel wide space allocated to tabs.  <br/>Note that if your tab path is a menu callback which returns FBML, this setting is ignored.'),
      '#options' => $theme_options,
      '#required' => TRUE,
      '#default_value' => $config['custom_theme'],
    );

    // Properties: http://developers.facebook.com/docs/appproperties
    $form['fb_app_data']['fb_tab']['tab_default_name'] = array(
      '#type' => 'textfield',
      '#title' => 'Tab name',
      '#default_value' => $config['tab_default_name'],
      '#description' => t('A very short title.'),
    );

    $form['fb_app_data']['fb_tab']['profile_tab_url'] = array(
      '#type' => 'textfield',
      '#title' => 'View path',
      '#default_value' => $config['profile_tab_url'],
      '#description' => t('Specify a Drupal page (i.e. "node/123") to render specific content in your page tab.  Or leave blank and implement <em>hook_fb_tab()</em> in your custom module.'),
    );

    $form['fb_app_data']['fb_tab']['profile_tab_url_liked'] = array(
      '#type' => 'textfield',
      '#title' => 'View path when page is liked',
      '#default_value' => $config['profile_tab_url_liked'],
      '#description' => t('Optionally specify another path when the user has liked the page where the tab is shown.  Leave blank to use <em>view path</em>.'),
    );

    $form['fb_app_data']['fb_tab']['app_data_is_path'] = array(
      '#type' => 'checkbox',
      '#title' => t('App_data overrides view path'),
      '#default_value' => $config['app_data_is_path'],
      '#description' => t('The special url parameter, app_data, is the view path.  This makes it possible to show any content in your page tab.'),
    );

    $form['fb_app_data']['fb_tab']['edit_url'] = array(
      '#type' => 'textfield',
      '#title' => 'Edit path',
      '#default_value' => $config['edit_url'],
      '#description' => t('Recommended value is %edit_url.  Implement <em>hook_fb_tab()</em> to customize the configuration form.', array(
                            '%edit_url' => FB_TAB_PATH_FORM,
                          )),
    );
  }
}
