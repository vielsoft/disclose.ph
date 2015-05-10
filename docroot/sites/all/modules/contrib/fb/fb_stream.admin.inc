<?php


/**
 * Form callback for general settings.
 */
function fb_stream_admin_settings($form, &$form_state) {
  $form = array();

  $token = variable_get(FB_STREAM_VAR_TOKEN, '');
  $options = array();
  $default = NULL;

  if ($token && empty($_POST)) {
    // Show details about the currently saved token.
    try {
      // TODO: consolodate graph api, use batch.
      $from = fb_graph('me', array('access_token' => $token));
      $via = fb_graph('app', array('access_token' => $token));
      // Show more verbose token description.
      $args = array(
        '%app_name' => _fb_get_name($via),
        '%user_name' => _fb_get_name($from),
        '%token' => $token,
      );

      $options[$token] = t('Post as %user_name via the %app_name application (leave token unchanged)', $args);
      $default = $token;

      $form['fb_stream_current_token'] = array(
        '#type' => 'fieldset',
        '#title' => t('Current token'),
        '#description' => t('The current token allows posting by %user_name via the %app_name application.', $args),
      );

      $form['fb_stream_current_token']['token'] = array(
        '#type' => 'fieldset',
        '#title' => t("Show current token"),
        '#description' => $token,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
    }
    catch (Exception $e) {
      $options[$token] = t('Do not change token (possibly expired)');
    }
  }

  if (!empty($_REQUEST['code']) && empty($_POST)) {
    // Send user to this URL after token is generated.
    $redirect_uri = url(request_path(), array(
                          'absolute' => TRUE,
                          'query' => array(
                            'client_id' => $_REQUEST['client_id'],
                          ),
                        ));

    $token = fb_stream_admin_code_to_token($_REQUEST['code'], $_REQUEST['client_id'], $redirect_uri);
    if ($token) {
      drupal_set_message(t('Generated a new access token, but not yet saved.  Remember to press the save button below!', array('%token' => $token)), 'warning');
      try {
        // TODO: consolodate graph api, use batch.
        $from = fb_graph('me', array('access_token' => $token));
        $via = fb_graph('app', array('access_token' => $token));
        // Show more verbose token description.
        $args = array(
          '%app_name' => _fb_get_name($via),
          '%user_name' => _fb_get_name($from),
          '%token' => $token,
        );

        $options[$token] = t('Post as %user_name via the %app_name application (use new token)', $args);
        $default = $token;
      }
      catch (Exception $e) {
        $options[$token] = t('Use new token (may not be a valid token)');

        fb_log_exception($e, t('Unable to query graph with new fb_stream token.'));
        drupal_set_message(t('There was an error using the current access token, %token.  Consider generating a new token.', array(
                               '%token' => $token,
                             )), 'error');
      }
    }
    else {
      drupal_set_message(t('Failed to generate a token from code returned by facebook.'), 'warning');
    }
  }

  if ($token) {
    try {
      $accounts = fb_graph('me/accounts', array('access_token' => $token));
      if (!empty($accounts) && !empty($accounts['data'])) {
        // TODO: support pagination if not all accounts returned.
        foreach ($accounts['data'] as $account_data) {
          if (!empty($account_data['access_token'])) {
            $options[$account_data['access_token']] = t('Post as %account_name (%account_type) via %app_name', array(
                                                          '%account_name' => _fb_get_name($account_data),
                                                          '%account_type' => $account_data['category'],
                                                          '%app_name' => !empty($via) ? _fb_get_name($via) : t('(could not determine application)'),
                                                        ));
          }
        }
      }
    }
    catch (Exception $e) {
      // If me/accounts fails, it probably just means the token is already an account token and not a user token.  Nothing to do here.
    }
  }

  // Because drupal builds the form over and over again, and because we don't want to generate all the options each time, we trust what gets passed in.
  if (!empty($form_state['input']) && !empty($form_state['input']['fb_stream_token_select'])) {
    $form['fb_stream_token_select'] = array(
      '#type' => 'value',
      '#value' => $form_state['input']['fb_stream_token_select'],
      '#validated' => TRUE,
    );
  }
  elseif (!empty($options)) {
    // Nothing passed in, show user the options.
    $form['fb_stream_token_select'] = array(
      '#type' => 'radios',
      '#title' => t('Select Access Token'),
      '#options' => $options,
      '#default_value' => $default,
      // Disable core validation, because options are not regenerated during submit.
      '#validated' => TRUE,
    );
  }

  // Use textarea not textfield for long tokens.  https://developers.facebook.com/docs/facebook-login/access-tokens/#sizes
  $form['fb_stream_token'] = array(
    '#type' => 'textarea',
    '#title' => t('Paste Access Token'),
    '#description' => t('Use <a href=!url target=_blank>Facebook\'s Graph Explorer</a> to generate a token you can copy and paste into this form. <br/><em>For best results your token should include the publish_stream and manage_pages extended permissions</em>.', array(
                          '!url' => 'https://developers.facebook.com/tools/explorer',
                        )),
    '#rows' => 1,
  );

  $form['fb_stream_token_prefer_long'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prefer longer-lived tokens.'),
    '#description' => t('Some Facebook access tokens expire in minutes, or when user logs out of facebook.com. Longer lived tokens can last weeks before expiring.  Facebook no longer offers access that never expire.'),
    '#default_value' => variable_get('fb_stream_token_prefer_long', TRUE),
  );


  // TODO: show user what permissions and pages their token can access.

  $form['fb_stream_apps'] = array(
    '#type' => 'fieldset',
    '#title' => t('Access Token Generator'),
    '#description' => t('Generate a token for a local application.  The new token will be placed in the form field above.'),
  );

  foreach (fb_get_all_apps() as $fb_app) {
    // Send user to this URL after token is generated.
    $redirect_uri = url(request_path(), array(
                          'absolute' => TRUE,
                          'query' => array(
                            'client_id' => $fb_app->id,
                          ),
                        ));

    $form['fb_stream_apps'][$fb_app->id] = array(
      '#type' => 'markup',
      '#markup' => l(t('post via the %title application', array(
                        '%title' => $fb_app->title,
                      )),
                    url("https://www.facebook.com/dialog/oauth", array(
                          'query' => array(
                            // Important to get the scope right.
                            'scope' => 'publish_stream,manage_pages',
                            'client_id' => $fb_app->id,
                            'redirect_uri' => $redirect_uri,
                          ),
                        )), array('html' => TRUE)),
      '#prefix' => '<p>', '#suffix' => '</p>',
    );
  }

  // Our validate hook gives us a way to swap long-lived tokens for shorter ones.
  $form['#validate'][] = '_fb_stream_validate_token';

  // We must redirect to remove facebook's code from url params.
  $form_state['redirect'] = implode('/', arg());
  unset($_GET['code']);
  unset($_GET['client_id']);

  return system_settings_form($form);
}

function _fb_stream_validate_token($form, &$form_state) {
  $values = $form_state['values'];
  $token = $values['fb_stream_token'];
  if (empty($token)) {
    $token = $values['fb_stream_token_select'];
    form_set_value($form['fb_stream_token'], $token, $form_state);
    form_set_value($form['fb_stream_token_select'], NULL, $form_state);
  }

  if (empty($token)) {
    form_set_error('fb_stream_token', t('You must select or paste an access token.'));
    return;
  }

  try {
    $via = fb_graph('app', array('access_token' => $token));

    if ($values['fb_stream_token_prefer_long']) {
      $app = fb_get_app(array('id' => $via['id']));

      if ($app && $app->secret) {
        try {
          $result = fb_graph('oauth/access_token', array(
                               'client_id' => $app->id,
                               'client_secret' => $app->secret,
                               'grant_type' => 'fb_exchange_token',
                               'fb_exchange_token' => $token,
                             ));
          if (!empty($result['access_token'])) {
            drupal_set_message(t('Using long-lived token, which is set to expire in %duration.', array(
                                   '%token' => $result['access_token'],
                                   '%duration' => !empty($result['expires']) ? format_interval($result['expires']) : t('(could not determine expiration)'),
                                 )));

            if ($result['access_token'] != $token) {
              // Change submitted value to the longer-lived version.
              form_set_value($form['fb_stream_token'], $result['access_token'], $form_state);
            }
          }
        }
        catch (Exception $e) {
          // This is reached whenever token belongs to an account instead of a user.  So we don't need to be verbose about it.
          drupal_set_message(t('Could not convert the token into a longer-lived token.  This is expected when token belongs to a page rather than a user.'));
          //fb_log_exception($e, t('Failed to convert token to longer-lived token.'));
        }
      }
    }
  }
  catch (Exception $e) {
    fb_log_exception($e, t('Unable to validate fb_stream token (%token).',array('%token' => $token)));
    form_set_error('fb_stream_token', $e->getMessage());
  }
}


function fb_stream_admin_code_to_token($code, $app_id, $redirect_uri) {
  $fb_app = fb_get_app(array('id' => $app_id));
  $path = url("https://graph.facebook.com/oauth/access_token", array(
                'query' => array(
                  'client_id' => $app_id,
                  'client_secret' => $fb_app->secret,
                  'code' => $code,
                  'redirect_uri' =>$redirect_uri,
                ),
              ));
  $http = drupal_http_request($path);
  if ($http->code == 200 && isset($http->data)) {
    $data = array();
    parse_str($http->data, $data);
    return $data['access_token'];
  }
}
