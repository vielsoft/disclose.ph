<?php

/**
 * @file fb_rules.rules.inc
 * Rules integration for the Drupal for Facebook module.
 */

/**
 * Implementation of hook_rules_event_info().
 * @ingroup rules
 */
function fb_rules_rules_event_info() {
  return array(
    'fb_user_connected' => array(
      'label' => t('User has connected his Facebook account'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User allowed Facebook App access')
        ),
      ),
    ),
    'fb_user_disconnected' => array(
      'label' => t('User has disconnected his Facebook account.'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User revoked Facebook App access.')
        ),
      ),
    ),
    'fb_user_login' => array(
      'label' => t('User has logged in via Facebook'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User who has logged in via Facebook.')
        ),
      ),
    ),
    'fb_invite_submitted' => array(
      'label' => t('User submitted Facebook invitation'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User who submitted invitation.')
        ),
      ),
    ),
    'fb_invite_accepted' => array(
      'label' => t('UserA accepted UserB Facebook invitation'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('UserB (inviter).')
        ),
      ),
    ),
    'fb_user_created' => array(
      'label' => t('User account created automatically with connected Facebook account'),
      'group' => t('Drupal for Facebook'),
      'module' => 'fb',
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('Newly created user account')
        ),
      ),
    ),
  );
}

/**
 * Implementation of hook_rules_condition_info().
 * @ingroup rules
 */
function fb_rules_rules_condition_info() {
  $defaults = array(
    'parameter' => array(),
    'group' => t('Facebook'),
  );
  return array(
    'fb_is_tab' => array(
      'label' => t('Viewing Facebook page tab'),
      'base' => 'fb_is_tab',
    ) + $defaults,
    'fb_is_page_liked' => array(
      'label' => t('Viewing Facebook page tab and user has liked page'),
      'base' => 'fb_is_page_liked',
    ) + $defaults,

    'fb_facebook_user' => array(
      'label' => t('User is currently connected to Facebook'),
      'base' => 'fb_facebook_user',
    ) + $defaults,
  );
}
