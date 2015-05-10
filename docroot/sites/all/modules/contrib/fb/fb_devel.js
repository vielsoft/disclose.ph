/**
 * Devel helpers and sanity checks.
 *
 * This file will be included only when fb_devel.module is enabled and user
 * has 'access devel information' permission.
 */

FB_Devel = function(){};
FB_Devel.sanity = null;

FB_Devel.sanityCheck = function(extreme) {
  FB_Devel.sanity = true; // assume sane.

  var root = jQuery('#fb-root');
  if (root.length != 1 || !root.hasClass("fb_module")) {
    FB_Devel.sanity = false;
    debugger; // not verbose.
    if (Drupal.settings.fb_devel.verbose) {
      alert("fb_devel.js: facebook javascript not properly configured!"); // verbose
    }
  }

  if (FB_Devel.sanity && Drupal.settings.fb.fb_init_settings.appId) {
    if (typeof(FB) == 'undefined') {
      // Global FB should have been configured but isn't.
      FB_Devel.sanity = false;
      debugger;
    }
    else if ((extreme || Drupal.settings.fb_devel.verbose == 'extreme') &&
             FB.getAccessToken()) { // Unfortunately, we can only check when access token known (user logged in.){
        // Does global FB agree with our app id setting?
        FB.api('/app', function(response) {
          if (response.id && response.id != Drupal.settings.fb.fb_init_settings.appId) {
            FB_Devel.sanity = false;
            debugger;
            // If you're here, you probably have multiple facebook modules installed, and they are competing to initialize facebook's javascript API.  In a quality move, Facebook made their FB object a global.  So your server had better initialize it only once.
          }
          else if (!response.id) {
            FB_Devel.sanity = false;
            debugger;
          }

        });
    }
  }

  if (FB_Devel.sanity && typeof(FB) != 'undefined' && Drupal.settings.fb.fbu &&
      (Drupal.settings.fb.fbu != FB.getUserID())) {
    // This could be reached if we happen to be in the middle of a session change event. However far more likely is that facebook javascript api was not initialized properly.
    FB_Devel.sanity = false;
    debugger;
  }

  if (Drupal.settings.fb.verbose && !FB_Devel.sanity) {
    alert("fb_devel.js: Facebook javascript not configured properly!");
  }



  return FB_Devel.sanity;
};


/**
 * Called when fb.js triggers the 'fb_init' event.
 */
FB_Devel.initHandler = function() {
  FB_Devel.sanityCheck(false);

  // Facebook events that may be of interest...
  //FB.Event.subscribe('auth.login', FB_Devel.debugHandler);
  //FB.Event.subscribe('auth.logout', FB_Devel.debugHandler);
  //FB.Event.subscribe('auth.statusChange', FB_Devel.debugHandler);
  //FB.Event.subscribe('auth.sessionChange', FB_Devel.debugHandler);
};

// Helper, for debugging javascript.
FB_Devel.debugHandler = function(data) {
  debugger; // Check the call stack to see what triggered event.
};


/**
 * Drupal behaviors hook.
 * Called when page is loaded, or content added via javascript.
 */
(function ($) {
  Drupal.behaviors.fb_devel = {
    attach : function(context) {
      // Respond to fb.js events.
      jQuery(document).bind('fb_init', FB_Devel.initHandler);
      jQuery(document).bind('fb_devel', FB_Devel.debugHandler);


      // Crappy third-party modules will blow away our carefully initialized FB object.  It's hard to tell when they will do it.  So when verbose, let's check for problems again, later.
      setTimeout(function() {
        if (FB_Devel.sanity == null) {
          // A sanityCheck was expected during fb_init.  If it hasn't happened, probably some other module is changing our app settings.
          debugger;
        }
        FB_Devel.sanityCheck(false);
      }, 30000); // Long enough for fb.js to initialize, as well as other modules that are out to wreak havoc.
    }
  };
})(jQuery);
