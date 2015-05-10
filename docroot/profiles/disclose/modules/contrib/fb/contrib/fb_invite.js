/**
 * @file
 * Javascript behaviors and helpers for fb_invite.module.
 */
FB_Invite = function(){};

/**
 * Drupal behaviors hook.
 * Called when page is loaded, or content added via javascript.
 */
(function ($) {
  Drupal.behaviors.fb_invite = {
    attach : function(context) {
      FB_Invite.drupalBehaviors(context);
    }
  };
})(jQuery);

FB_Invite.drupalBehaviors = function(context) {
  jQuery(document).bind('fb_init', FB_Invite.fbBehaviors);
};

FB_Invite.friends = [];
FB_Invite.users = [];
FB_Invite.friendsRendered = 0;
FB_Invite.usersRendered = 0;
FB_Invite.renderIncrement = 300; // How many users to display at a time.

/**
 * This callback is invoked after facebook API object FB is initialized.
 *
 * Here we query facebook for the current user's friends.  The list
 * may be long, so we render them only as the page is scrolled down
 * far enough to see the bottom of the list.
 */
FB_Invite.fbBehaviors = function() {

  jQuery('#fb_invite_friend_template:not(fb_invite-processed)').each(function() {
    jQuery(this).addClass('fb_invite-processed');
    jQuery(this).after('<div id=fb_invite_end_friends></div>'); // This tells us whether end of list is viewable.
    jQuery('#fb_invite_end_friends').attr('count', 0);
    jQuery(window).scroll(FB_Invite.renderFriends);
    FB.api('/fql', {q : 'SELECT uid, name, is_app_user, pic_square FROM user WHERE uid IN (SELECT uid2 FROM friend WHERE uid1 = me()) AND is_app_user = 0 ORDER BY name ASC'}, function(response) {
      FB_Invite.friends = response.data;
      FB_Invite.renderFriends();
    });
  });

  jQuery('#fb_invite_user_template:not(fb_invite-processed)').each(function() {
    jQuery(this).addClass('fb_invite-processed');
    jQuery(this).after('<div id=fb_invite_end_users></div>'); // This tells us whether end of list is viewable.
    jQuery('#fb_invite_end_users').attr('count', 0);
    jQuery(window).scroll(FB_Invite.renderFriends);
    FB.api('/fql', {q : 'SELECT uid, name, is_app_user, pic_square FROM user WHERE uid IN (SELECT uid2 FROM friend WHERE uid1 = me()) AND is_app_user = 1 ORDER BY name ASC'}, function(response) {
      FB_Invite.users = response.data;
      FB_Invite.renderUsers();
    });
  });

};

FB_Invite.renderFriends = function() {
  var endElement = jQuery('#fb_invite_end_friends');
  while (typeof(FB_Invite.friends) != 'undefined' && FB_Invite.isScrolledIntoView(endElement) && endElement.attr('count') < FB_Invite.friends.length) {
    // Render each friend not an app user.
    for(var i = endElement.attr('count'); i < Math.min(FB_Invite.friends.length, endElement.attr('count') + FB_Invite.renderIncrement); i++) {
      var row = jQuery('#fb_invite_friend_template').clone();
      var id = 'fb_invite_' + FB_Invite.friends[i].uid;
      row.attr('id', id);
      jQuery('.fb_invite_name', row).text(FB_Invite.friends[i].name);
      jQuery('img.fb_invite_img', row).attr('src', '//graph.facebook.com/' + FB_Invite.friends[i].uid + '/picture');
      jQuery('input', row).click(FB_Invite.friends[i], FB_Invite.sendInvite);

      row.insertBefore(jQuery('#fb_invite_friend_template')).show();
    }
    endElement.attr('count', i);
  }
};

FB_Invite.renderUsers = function() {
  var endElement = jQuery('#fb_invite_end_users');
  while (typeof(FB_Invite.users) != 'undefined' && FB_Invite.isScrolledIntoView(endElement) && endElement.attr('count') < FB_Invite.users.length) {
    // Render each friend who is an app user.
    for(var i = endElement.attr('count'); i < Math.min(FB_Invite.users.length, endElement.attr('count') + FB_Invite.renderIncrement); i++) {
      var row = jQuery('#fb_invite_user_template').clone();
      var id = 'fb_invite_' + FB_Invite.users[i].uid;
      row.attr('id', id);
      jQuery('.fb_invite_name', row).text(FB_Invite.users[i].name);
      jQuery('.fb_invite_user_link', row).attr('href', Drupal.settings.fb.base_url + '/fb_user/' + FB_Invite.users[i].uid);
      jQuery('img.fb_invite_img', row).attr('src', '//graph.facebook.com/' + FB_Invite.users[i].uid + '/picture');
      jQuery('input', row).click(FB_Invite.users[i], FB_Invite.sendInvite);

      row.insertBefore(jQuery('#fb_invite_user_template')).show();
    }
    endElement.attr('count', i);
  }
};

FB_Invite.sendInvite = function(e) {
  FB.ui({method: 'send',
         name: Drupal.settings.fb_invite.site_name,
         link: Drupal.settings.fb.base_url,
         to: e.data.uid
        }, FB_Invite.requestCallback);

  return false;
};

FB_Invite.sendInviteMFS = function() {
  FB.ui({method: 'send',
         name: Drupal.settings.fb_invite.site_name,
         link: Drupal.settings.fb.base_url,
        }, FB_Invite.requestCallback);
};

FB_Invite.requestCallback = function(response) {
  // TODO Might make sense to invoke ajax event here.
};

/**
 * http://stackoverflow.com/questions/487073/check-if-element-is-visible-after-scrolling
 */

FB_Invite.isScrolledIntoView = function(elem)
{
  var docViewTop = jQuery(window).scrollTop();
  var docViewBottom = docViewTop + jQuery(window).height();

  var elemTop = jQuery(elem).offset().top;
  var elemBottom = elemTop + jQuery(elem).height();

  return ((elemBottom <= docViewBottom) && (elemTop >= docViewTop));
}
