<?php
/**
 * @file
 * Admin pages and forms for facebook applications.
 *
 */

include drupal_get_path('module', 'fb') . '/fb.admin.inc';

/**
 * Implements hook_fb_admin().
 */
function fb_app_fb_admin($op, $data, &$return) {
  $fb = isset($data['fb']) ? $data['fb'] : NULL;
  $fb_app = isset($data['fb_app']) ? $data['fb_app'] : NULL;

  if ($op == FB_ADMIN_OP_SET_PROPERTIES) {
    // URLs for Facebook events we support.
    $return['uninstall_url'] = url(FB_APP_PATH_EVENT . '/' . $fb_app->label . "/" . FB_APP_EVENT_POST_REMOVE . '/', array(
                                     'absolute' => TRUE,
                                     'language' => NULL,
                                   ));
    $return['authorize_url'] = url(FB_APP_PATH_EVENT . '/' . $fb_app->label . "/" . FB_APP_EVENT_POST_AUTHORIZE . '/', array(
                                     'absolute' => TRUE,
                                     'language' => NULL,
                                   ));
  }
  elseif ($op == FB_ADMIN_OP_POST_SET_PROPERTIES) {
    // Application properties have been set or synced on facebook.  Ensure we store the application's proper namespace.
    try {
      $graph = fb_graph($data['fb_app']->id, array(
                          'access_token' => fb_get_token($data['fb']),
                        ));

      if (empty($graph['namespace'])) {
        // Avoid PHP notice when optional namespace is missing.
        $graph['namespace'] = '';
      }

      db_update('fb_app')
        ->fields(array('canvas' => $graph['namespace']))
        ->condition('fba_id', $graph['id'], '=')
        ->execute();
    }
    catch (Exception $e) {
      fb_log_exception($e, t('Failed to learn application namespace from Facebook.'));
    }
  }
  elseif ($op == FB_ADMIN_OP_LIST_PROPERTIES) {
    $return[t('Application Name')] = 'name';
    $return[t('De-Authorize Callback URL')] = 'deauth_callback_url';
  }
  elseif (($op == FB_ADMIN_OP_LOCAL_LINKS) && isset($fb_app->fba_id)) {
    // Path to edit this app.
    $return[t('edit')] = FB_PATH_ADMIN_APPS . '/' . $fb_app->label . '/fb_app';
  }
}

/**
 * Builds the form used to edit an application.
 *
 * This form supports both create and edit.
 */
function fb_app_edit_form($form, $form_state, $fb_app = NULL) {
  // If app is managed by this module, it has fba_id.
  if (isset($fb_app) && !$fb_app->fba_id) {
    drupal_set_message(t('Application %label not found.', array(
                           '%label' => $fb_app->label)),
                       'warning');
    drupal_not_found();
    exit();
  }

  if (!isset($fb_app)) {
    // Defaults for new app.
    $fb_app = (object) array(
      'label' => NULL,
      'apikey' => NULL, // deprecated.
      'canvas' => NULL,
      'fba_id' => NULL,
      'id' => NULL,
      'status' => 1,
      'data' => serialize(array('fb_app' => array('set_app_props' => TRUE))),
    );
  }

  $form['#fb_app'] = $fb_app; // Similar to #node

  if (!$fb_app->label) {
    $helptext = '<ol>
<li>Visit the Facebook application page, <a target="_blank" href="https://developers.facebook.com/apps">https://developers.facebook.com/apps</a>, and click "Create New App".</li>
<li>Enter a descriptive name in the Application Name field. Users will see this when signing up for your site.</li>
<li>Accept the Facebook Terms of Service.</li>
<li>If building a Canvas Page App, specify a Canvas Path.</li>
<li>Upload icon and logo images. The icon appears in News Feed stories and the logo appears in the Connect dialog when the user connects with your site.</li>
<li>Click Submit.</li>
<li>Copy the displayed App ID and Application Secret into this form.</li>
</ol>';

    $form['helptext'] = array(
      '#markup' => t($helptext),
      '#weight' => -10,
    );
    $form['helptext2'] = array(
      '#markup' => t('It is recommended to administer drupal in one browser (this one) and log into facebook in another browser, so no cookies are shared.  So for example if using Chrome, follow the create app link above in an incognito window.'),
      '#prefix' => '<p><em>', '#suffix' => '</em></p>',
    );

  }

  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#required' => TRUE,
    '#maxlength' => 20,
    '#default_value' => $fb_app->label,
    '#maxlength' => 20,
    '#description' => t('A short machine-friendly name for this application.  Use letters and numerals only (no spaces, etc). <br/>Module code may refer to this label, in order to customize the behavior of this app.<br/>When working with multiple copies of an application (i.e. development, staging, production),  use the <strong>same label</strong> on all servers.  Apikey, secret and ID will change from server to server, but <strong>the label remains the same</strong>.'),
  );
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => $fb_app->status,
    '#description' => t('Uncheck if this server no longer hosts this application, but you prefer not to delete the settings.'),
  );

  // ID, apikey and secret are shown on facebook.  User copies and pastes values.
  $form['id'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook App ID'),
    '#required' => TRUE,
    '#default_value' => $fb_app->id,
    '#description' => t('Facebook will generate this value when you create the application.'),
  );
  $form['secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret'),
    '#required' => TRUE,
    '#default_value' => isset($fb_app->secret) ? $fb_app->secret : NULL,
    '#description' => t('Facebook will generate this value when you create the application.'),
  );

  // fb_app_data is a placeholder where other modules can attach settings.
  $form['fb_app_data'] = array('#tree' => TRUE);

  // Add our own fields to fb_app_data.  Other modules use hook_form_alter to do this.

  $data = fb_get_app_data($fb_app);
  $form['fb_app_data']['fb_app']['set_app_props'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set application properties automatically'),
    '#default_value' => isset($data['fb_app']) ? $data['fb_app']['set_app_props'] : NULL,
    '#description' => t('Synchronize Facebook settings for this application when you save this form.  Disable this if you have customized your callback URL, or other settings on facebook.com.  Also disable if another Drupal instance hosts the same application (i.e. with shared subdomain).'),
  );

  $form['buttons'] = array();
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 5,
    '#submit' => array('fb_app_admin_form_submit'),
  );

  if ($fb_app->fba_id) {
    $form['buttons']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#weight' => 15,
      '#submit' => array('fb_app_admin_form_delete_submit'),
    );
  }

  return $form;
}

/**
 * Form validation.
 */
function fb_app_edit_form_validate($form, &$form_state) {
  $fb_app = (object) $form_state['values'];
  $fb_app->data = serialize($fb_app->fb_app_data);

  if ($form_state['values']['op'] != t('Delete')) {
    // Labels must be alphanumeric.
    if (preg_match('/[^a-z0-9_]/', $fb_app->label)) {
      form_error($form['label'], t('Label must be lower-case alphanumeric or underscores only.'));
    }

    // Labels must be unique.
    $apps = fb_get_all_apps();
    foreach ($apps as $app) {
      if ($app->label == $fb_app->label &&
          (!isset($form['#fb_app']) || $form['#fb_app']->label != $fb_app->label)) {
        form_set_error('fb_app][label', t('The label %label is in use by another application.', array(
                                            '%label' => $fb_app->label,
                                          )));
      }
    }
    // Getting properties confirms apikey and secret.
    fb_admin_get_app_info($fb_app);
    if (!$fb_app->name) {
      // Don't use form_set_error(), as that will prevent the user from saving any data.
      drupal_set_message(t("Unable to get application properties.  Possibly, you've given the wrong id or secret.  Possibly, this server is unable to reach facebook's servers.  Your application will not work properly!"), 'error');
      $fb_app->name = 'UNKOWN';
    }
  }
}

function fb_app_admin_form_submit($form, &$form_state) {
  $fb_app = (object)$form_state['values'];
  $fb_app->data = serialize($fb_app->fb_app_data);

  // Get namespace, name from facebook.
  fb_admin_get_app_info($fb_app);

  $orig_app = $form['#fb_app'];

  if ($orig_app->fba_id) {
    // Updating.
    try {
      db_update('fb_app')
        ->fields(array(
          'label' => $fb_app->label,
          'status' => $fb_app->status,
          'apikey' => $fb_app->id, // Note, apikey deprecated.  Using ID.
          'secret' => $fb_app->secret,
          'id' => $fb_app->id,
          // Canvas and title are learned from facebook, not the form.
          'canvas' => !empty($fb_app->namespace) ? $fb_app->namespace : '',
          'title' => !empty($fb_app->name) ? $fb_app->name : $fb_app->label,
          'data' => $fb_app->data,
                 ))
        ->condition('fba_id', $orig_app->fba_id)
        ->execute();

      watchdog('fb_app', 'Updated Facebook Application %label.', array(
                 '%label' => $fb_app->label,
               ),
               WATCHDOG_NOTICE,
               l(t('view apps'), FB_PATH_ADMIN_APPS));

    }
    catch (Exception $e) {
      // Log the exception to watchdog.
      watchdog_exception('fb_app', $e);

      drupal_set_message(t('Failed to save changes to facebook application %title (%label). Check the log.',
                           array('%label' => $fb_app->label,
                                 '%title' => $fb_app->name)));
    }
    drupal_set_message(t('Saved changes to Facebook application %title (%label).',
                         array('%title' => $fb_app->name,
                               '%label' => $fb_app->label)));

    // Should we just go ahead and flush the cache here automatically?
    drupal_set_message(t('Recommended: <a href=!cache_url>clear cached data</a> after changing Facebook app settings.', array(
                           '!cache_url'=> url('admin/config/development/performance'),
                         )), 'warning');
  }
  else {
    // Inserting.
    try {
      db_insert('fb_app')
        ->fields(array(
                   'label' => $fb_app->label,
                   'status' => $fb_app->status,
                   'apikey' => $fb_app->id, // Note, apikey deprecated.  Using ID.
                   'secret' => $fb_app->secret,
                   'id' => $fb_app->id,
                   // Canvas and title are learned from facebook, not the form.
                   'canvas' => !empty($fb_app->namespace) ? $fb_app->namespace : '',
                   'title' => !empty($fb_app->name) ? $fb_app->name : $fb_app->label,
                   'data' => $fb_app->data,
                 ))
        ->execute();

      watchdog('fb_app', 'Created Facebook Application %label.', array(
                 '%label' => $fb_app->label,
               ),
               WATCHDOG_NOTICE,
               l(t('view apps'), FB_PATH_ADMIN_APPS));

      drupal_set_message(t('Created facebook application %title (%label).', array(
                             '%label' => $fb_app->label,
                             '%title' => $fb_app->name,
                           )));
    }
    catch (Exception $e) {
      // Log the exception to watchdog.
      watchdog_exception('fb_app', $e);

      drupal_set_message(t('Failed to create facebook application %title (%label). Check the log.',
                           array('%label' => $fb_app->label,
                                 '%title' => $fb_app->name)));
    }
  }

  if ($fb_app->status) {
    fb_app_set_app_properties($fb_app); // Set callback URL, etc.
  }

  $form_state['redirect'] = FB_PATH_ADMIN;

}

/**
 * Button submit function.  Use has clicked delete, send them to confirm page.
 */
function fb_app_admin_form_delete_submit($form, &$form_state) {
  $destination = '';
  if (isset($_REQUEST['destination'])) {
    $destination = drupal_get_destination();
    unset($_REQUEST['destination']);
  }
  $fb_app = $form['#fb_app'];
  $form_state['redirect'] = array(FB_PATH_ADMIN_APPS . '/' . $fb_app->label . '/fb_app/delete', array('query' => $destination));
}


/**
 * Form creator -- ask for confirmation of deletion
 */
function fb_app_admin_delete_confirm_form($form, &$form_state, $fb_app) {
  $form['fba_id'] = array(
    '#type' => 'value',
    '#value' => $fb_app->fba_id,
  );

  return confirm_form($form,
                      t('Are you sure you want to delete %title?', array('%title' => $fb_app->title)),
                      isset($_GET['destination']) ? $_GET['destination'] : FB_PATH_ADMIN_APPS . '/' . $fb_app->label,
                      t('This action cannot be undone.'),
                      t('Delete'),
                      t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function fb_app_admin_delete_confirm_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $fba_id = $form_state['values']['fba_id'];

    // @TODO: invoke hooks so that third-party modules may act.

    db_delete('fb_app')
      ->condition('fba_id', $fba_id)
      ->execute();
  }

  $form_state['redirect'] = FB_PATH_ADMIN_APPS;
}



/**
 * Sets callback URLs and other properties of a facebook app.  Calls the facebook
 */
function fb_app_set_app_properties($fb_app) {
  $data = fb_get_app_data($fb_app);
  $fb_app_data = $data['fb_app'];
  // Collect properties from all modules.
  $props = fb_invoke(FB_ADMIN_OP_SET_PROPERTIES, array('fb_app' => $fb_app), array(), FB_ADMIN_HOOK);
  if (count($props)) {
    if ($fb_app_data['set_app_props']) {
      if ($fb = fb_api_init($fb_app)) {
        try {
          $result = fb_graph($fb_app->id, $props + array('access_token' => fb_get_token($fb)), 'POST', $fb);
          // @todo handle $result != true.

          drupal_set_message(t('Note that it may take several minutes for property changes to propagate to all facebook servers.'));
          if (fb_verbose()) {
            drupal_set_message(t('Set the following properties for %label application:<br/><pre>!props</pre>', array('%label' => $fb_app->label, '!props' => print_r($props, 1))));
            watchdog('fb_app', 'Set facebook app properties for %label.', array(
                       '%label' => $fb_app->label,
                     ),
                     WATCHDOG_NOTICE,
                     l(t('view apps'), FB_PATH_ADMIN));
          }

          // Allow third-parties to set additional "properties" such as restrictions.
          fb_invoke(FB_ADMIN_OP_POST_SET_PROPERTIES, array(
                      'fb_app' => $fb_app,
                      'fb' => $fb,
                      'properties' => $props,
                    ), array(), FB_ADMIN_HOOK);
        }
        catch (Exception $e) {
          drupal_set_message(t('Failed to set the following properties for %label application.  You may need to manually editing remote settings!<br/><pre>!props</pre>', array('%label' => $fb_app->label, '!props' => print_r($props, 1))), 'error');
          fb_log_exception($e, t('Failed to set application properties on Facebook. %props', array('%props' => json_encode($props))));
        }
      }
    }
    elseif (fb_verbose()) {
      drupal_set_message(t('The following recommended properties for %label application have <strong>not been set automatically</strong>, consider editing remote settings manually:<br/><pre>!props</pre>', array('%label' => $fb_app->label, '!props' => print_r($props, 1))), 'warning');
    }
  }
}
