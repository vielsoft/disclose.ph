/**
 * @file
 * Javascript behaviors and helpers for modules/fb.
 */

FB_JS = function(){};
FB_JS.fbu = null; // Detect session changes
FB_JS.ignoreEvents = false; // Hack makes login status test possible
FB_JS.reloadParams = {}; // Pass data to drupal when reloading

FB_JS.fbu = null;

if (!window.Node) { // IE
  FB_JS.DOCUMENT_NODE = 9;
}
else {
  FB_JS.DOCUMENT_NODE = window.Node.DOCUMENT_NODE;
}

/**
 * Drupal behaviors hook.
 * Called when page is loaded, or content added via javascript.
 */
(function ($) {
  Drupal.behaviors.fb = {
    attach : function(context) {
      FB_JS.drupalBehaviors(context);
    }
  };
})(jQuery);

/**
 * Called when page is loaded, or content added via javascript.
 */
FB_JS.drupalBehaviors = function(context) {

  // Sanity check.
  if (!Drupal.settings.fb) {
    // Reach here when Drupal footer is not rendered in page template.
    Drupal.settings.fb = {};
  }

  // Respond to our jquery pseudo-events
  var events = jQuery(document).data('events');
  if (!events || !events.fb_session_change) {
    jQuery(document).bind('fb_session_change', FB_JS.sessionChangeHandler);
  }

  // If FB is not yet initialized, fbAsyncInit() will be called when it is.
  if (typeof(FB) != 'undefined') {
    // Render any XFBML markup that may have been added by AJAX.
    jQuery(context).each(function() {
      var elem = jQuery(this).get(0);
      if (elem.nodeType == FB_JS.DOCUMENT_NODE) { // Popups are entire document.
        // FB.XFBML.parse() fails if passed document.  Pass body element instead.
        elem = jQuery(context).find('body').get(0);
      }

      try {
        FB.XFBML.parse(elem);
      }
      catch(error) {
        jQuery.event.trigger('fb_devel', error);
      }
    });

    FB_JS.showConnectedMarkup(Drupal.settings.fb.fbu, context);
  }

  // Markup with class .fb_show should be visible if javascript is enabled.  .fb_hide should be hidden.
  jQuery('.fb_hide', context).hide();
  jQuery('.fb_show', context).show();

  if (Drupal.settings.fb.fb_reloading) {
    // The reloading flag helps us avoid infinite loops.  But will accidentally prevent a reload in some cases. We really want to prevent a reload for a few seconds.
    setTimeout(function() {
      Drupal.settings.fb.fb_reloading = false;
    }, 5000);
  }
};

if (typeof(window.fbAsyncInit) != 'undefined') {
  // There should be only one definition of fbAsyncInit!
  jQuery.event.trigger('fb_devel', {});
};

window.fbAsyncInit = function() {

  if (Drupal.settings.fb) {
    FB.init(Drupal.settings.fb.fb_init_settings);
  }

  if (Drupal.settings.fb.fb_init_settings.authResponse) {
    // Trust login status passed into us.  No getLoginStatus
    FB_JS.fbAsyncInitFinal();

  }
  else if (!Drupal.settings.fb.fb_init_settings ||
           !Drupal.settings.fb.fb_init_settings.appId) {
    // Once upon a time, we could test FB._apikey to learn whether FB was initialize with an appId.  Now, there is no way to do that.  So the test above uses the data we pass in.  Unfortunately if FB is not initialized by our code things may not work properly here.

    // Cannot call getLoginStatus when not hosting an app.
    FB_JS.fbAsyncInitFinal();
  }
  else {
    FB_JS.getLoginStatus(function(response) {
      if (Drupal.settings.fb.fbu && !response.authResponse) {
        if (Drupal.settings.fb.page_type) {
          // On canvas and tabs(?), this probably means third-party cookies not accepted.
          jQuery.event.trigger('fb_devel', {}); // debug
          FB_JS.reloadParams.fb_login_status = false;
        }
      }
      FB_JS.fbAsyncInitFinal(response);
    });
  }
}

FB_JS._fbAsyncInitFinalComplete = false; // semaphore
FB_JS.fbAsyncInitFinal = function(response) {

  if (FB_JS._fbAsyncInitFinalComplete && !response) {
    return; // execute this function only once.
  }
  FB_JS._fbAsyncInitFinalComplete = true;

  if (!response) {
    response = FB.getAuthResponse();
  }

  jQuery.event.trigger('fb_init');  // Trigger event for third-party modules.

  if (response) {
    FB_JS.authResponseChange(response); // This will act only if fbu changed.
  }

  FB_JS.eventSubscribe();  // Get notified when session changes

  FB_JS.showConnectedMarkup(FB.getUserID()); // Show/hide markup based on connect status

  if (typeof(FB.XFBML) != 'undefined') { // Soon to be deprecated?
    try {
      FB.XFBML.parse();
    }
    catch (error) {
      jQuery.event.trigger('fb_devel', error);
    }
  }
};

/**
 * Wrapper for FB.getLoginStatus().
 * Unlike the FB version, this function always calls its callback.
 */
FB_JS.getLoginStatus = function(callback, force) {
  var semaphore; // Avoid multiple calls to callback.
  semaphore = false;

  FB.getLoginStatus(function(response) {
    semaphore = true;
    callback(response);
  }, force);

  // Fallback for when getLoginStatus never calls us back.
  setTimeout(function() {
    if (!semaphore) {
      callback({'authResponse' : null});
    }
  }, 3000); // 3000 = 3 seconds
};

/**
 * Tell facebook to notify us of events we may need to act on.
 */
FB_JS.eventSubscribe = function() {
  // Use FB.Event to detect Connect login/logout.
  FB.Event.subscribe('auth.authResponseChange', FB_JS.authResponseChange);

  // Q: what the heck is "edge.create"? A: the like button was clicked.
  FB.Event.subscribe('edge.create', FB_JS.edgeCreate);
}

/**
 * Helper parses URL params.
 *
 * http://jquery-howto.blogspot.com/2009/09/get-url-parameters-values-with-jquery.html
 */
FB_JS.getUrlVars = function(href) {
  var vars = [], hash;
  var hashes = href.slice(href.indexOf('?') + 1).split('&');
  for(var i = 0; i < hashes.length; i++)
  {
    hash = hashes[i].split('=');
    vars[hash[0]] = hash[1];
    if (hash[0] != 'fbu')
      vars.push(hashes[i]); // i.e. "foo=bar"
  }
  return vars;
}

/**
 * Reload the current page, whether on canvas page or facebook connect.
 *
 */
FB_JS.reloading = false // semaphore prevents concurrent reloads.
FB_JS.reload = function(destination) {
  if (FB_JS.reloading) {
    // We are attempting to reload two times at once.
    return;
  }
  else {
    FB_JS.reloading = true;
  }
  // Avoid repeated reloads.  Esp on canvas pages when third-party cookies disabled.
  if (Drupal.settings.fb.fb_reloading) {
    // We are attempting a reload one after another.
    jQuery.event.trigger('fb_devel', destination); // Debug. JS and PHP SDKs are not in sync.
    return;
  }

  // Determine where to send user.
  if (typeof(destination) != 'undefined' && destination) {
    // Use destination passed in.
  }
  else if (typeof(Drupal.settings.fb.reload_url) != 'undefined') {
    destination = Drupal.settings.fb.reload_url;
  }
  else {
    destination = window.location.href;
  }

  // Split and parse destination
  var path;
  if (destination.indexOf('?') == -1) {
    vars = [];
    path = destination;
  }
  else {
    vars = FB_JS.getUrlVars(destination);
    path = destination.substr(0, destination.indexOf('?'));
  }

  // Passing this helps us avoid infinite loop.
  FB_JS.reloadParams.fb_reload = true;

  // Canvas pages will not get POST vars, so include them in the URL.
  if (Drupal.settings.fb.page_type == 'canvas') {
    for (var key in FB_JS.reloadParams) {
      vars.push(key + '=' + FB_JS.reloadParams[key]);
    }
  }

  destination = vars.length ? (path + '?' + vars.join('&')) : path;

  if (Drupal.settings.fb.reload_url_fragment) {
    destination = destination + "#" + Drupal.settings.fb.reload_url_fragment;
  }

  // Feedback that entire page may be reloading.
  if (typeof(Drupal.settings.fb.reload_progress) == 'undefined' || Drupal.settings.fb.reload_progress) {
    // This unweildy set of tags should turn facebook-specific markup into a progress indicator.
    var fbMarkup = jQuery('.fb_connected,.fb_not_connected').wrap('<div class="progress" />').wrap('<div class="bar" />').wrap('<div class="filled" />');
    if (fbMarkup.length) {
      fbMarkup.hide(); // Hides FBML, leaves progress bar.
    }


    // Spinning progress indicator
    jQuery('body').prepend('<div id="fb_reload"><div class="fb_spinner"></div></div>');
  }

  // Use POST to get past any caching on the server.
  // reloadParams includes signed_request.
  if (Drupal.settings.fb.fbu && Drupal.settings.fb.test_login_status) {
    // The login status test might break all future calls to FB.  So we do it only immediately before reload.
    FB_JS.testGetLoginStatus(function() {
      FB_JS.postToURL(destination, FB_JS.reloadParams);
    });
  }
  else if (!FB_JS.isEmpty(FB_JS.reloadParams)) {
    FB_JS.postToURL(destination, FB_JS.reloadParams);
  }
  else {
    window.top.location = destination; // Uses GET, returns cached pages.
  }

};

/**
 * Send the browser to a URL.
 * Similar to setting window.top.location, but via POST instead of GET.
 * POST will get through Drupal cache or external cache (i.e. Varnish)
 */
FB_JS.postToURL = function(path, params, method) {
  method = method || "post"; // Set method to post by default, if not specified.

  // The rest of this code assumes you are not using a library.
  // It can be made less wordy if you use one.
  var form = document.createElement("form");
  form.setAttribute("method", method);
  form.setAttribute("action", path);
  form.setAttribute("target", '_top'); // important for canvas pages

  for(var key in params) {
    if(params.hasOwnProperty(key)) {
      var hiddenField = document.createElement("input");
      hiddenField.setAttribute("type", "hidden");
      hiddenField.setAttribute("name", key);
      hiddenField.setAttribute("value", params[key]);

      form.appendChild(hiddenField);
    }
  }

  document.body.appendChild(form);
  form.submit();
}


// Facebook pseudo-event handlers.
FB_JS.authResponseChange = function(response) {
  if (FB_JS.ignoreEvents) {
    return;
  }

  if (response.authResponse && response.authResponse.signedRequest) {
    // If we end up reloading page, pass signed request.
    FB_JS.reloadParams.signed_request = response.authResponse.signedRequest;
  }
  else {
    delete FB_JS.reloadParams.signed_request;
  }

  var status = {
    'changed': false,
    'fbu': FB.getUserID(),
    'response' : response
  };

  if ((Drupal.settings.fb.fbu || status.fbu) &&
      Drupal.settings.fb.fbu != status.fbu) {
    // Drupal.settings.fb.fbu (from server) not the same as status.fbu (from javascript).
    status.changed = true;
  }
  else if (status.fbu && !response.authResponse) {
    // A bug in facebook's all.js, user has logged out yet FB.getUserID() still returns their id!
    // This is our workaround.
    status.changed = true;
    status.fbu = null;
  }


  if (status.changed) {
    // Remember the fbu.
    Drupal.settings.fb.fbu = status.fbu;

    // fbu has changed since server built the page.
    jQuery.event.trigger('fb_session_change', status);

    FB_JS.showConnectedMarkup(status.fbu);
  }
};

// edgeCreate is handler for Like button.
FB_JS.edgeCreate = function(href, widget) {
  var data = {'href': href};
  FB_JS.ajaxEvent('edge.create', data);
};

// JQuery pseudo-event handler.
FB_JS.sessionChangeHandler = function(context, status) {
  // Pass data to ajax event.
  var data = {
    'event_type': 'session_change',
    'is_anonymous': Drupal.settings.fb.is_anonymous
  };

  data.fbu = status.fbu;

  FB_JS.ajaxEvent(data.event_type, data);

  // Note that ajaxEvent might reload the page.
};


// Helper to pass events via AJAX.
// A list of javascript functions to be evaluated is returned.
FB_JS.ajaxEvent = function(event_type, request_data) {
  if (Drupal.settings.fb.ajax_event_url) {

    if (typeof(Drupal.settings.fb_page_type) != 'undefined') {
      request_data.fb_js_page_type = Drupal.settings.fb_page_type;
    }

    // Historically, we pass appId to ajax events.
    // This data no longer present in JS API, so may be removed soon.
    // In other words, deprecated!
    request_data.appId = Drupal.settings.fb.fb_init_settings.appId;

    // Other values to pass to ajax handler.
    if (Drupal.settings.fb.controls) {
      request_data.fb_controls = Drupal.settings.fb.controls;
    }

    // In case cookies are not accurate, always pass in signed request.
    if (typeof(FB.getAuthResponse) != 'undefined') {
      response = FB.getAuthResponse();
      if (response && response.signedRequest) {
        request_data.signed_request = response.signedRequest;
      }
    }
    else {
      session = FB.getSession();
      if (session) {
        //request_data.session = session;
        request_data.access_token = session.access_token;
      }
    }


    jQuery.ajax({
      url: Drupal.settings.fb.ajax_event_url + '/' + event_type,
      data : request_data,
      type: 'POST',
      dataType: 'json',
      success: function(js_array, textStatus, XMLHttpRequest) {
        if (js_array.length > 0) {
          for (var i = 0; i < js_array.length; i++) {
            // alert(js_array[i]);// debug
            eval(js_array[i]);
          }
        }
        else {
          if (event_type == 'session_change') {
            // No instructions from ajax.  Notify interested parties
            jQuery.event.trigger('fb_session_change_done');
          }
        }
      },
      error: function(jqXHR, textStatus, errorThrown) {
        // Unexpected error (i.e. ajax did not return json-encoded data).
        var headers = jqXHR.getAllResponseHeaders(); // debug info.
        var responseText = jqXHR.responseText; // debug info.
        // @TODO: handle error, but how?
        jQuery.event.trigger('fb_devel', jqXHR);
      }
    });
  }
};


/**
 * Called when we first learn the currently logged in user's Facebook ID.
 *
 * Responsible for showing/hiding markup not intended for the current
 * user.  Some sites will choose to render pages with fb_connected and
 * fb_not_connected classes, rather than reload pages when user's
 * connect/disconnect.
 */
FB_JS.showConnectedMarkup = function(fbu, context) {
  if (context || fbu != FB_JS.fbu) {
    if (fbu) {
      FB_JS.fbu = fbu;
      // Show markup intended only for connected users.
      jQuery('.fb_not_connected', context).hide();
      jQuery('.fb_connected', context).show();
    }
    else {
      FB_JS.fbu = null;
      // Show markup intended only for not connected users.
      jQuery('.fb_connected', context).hide();
      jQuery('.fb_not_connected', context).show();
    }
  }
};

/**
 * Tests whether FB.getLoginStatus() will work.
 * It tends to fail when user disables third-party cookies, and when apps are in sandbox mode, and probably more cases.
 * The danger of running this test is that if it fails, future calls to FB will break, because FB will forget the current user's credentials.
 */
FB_JS.testGetLoginStatus = function(callback) {
  // Attempt to learn whether third party cookies are enabled.
  FB_JS.ignoreEvents = true; // disregard events triggered by getLoginStatus.
  FB_JS.getLoginStatus(function(response) {
    FB_JS.ignoreEvents = false; // we can pay attention again
    if (!response.authResponse) {
      // Let fb.module know that test failed.
      FB_JS.reloadParams.fb_login_status = false;
    }
    callback(response.authResponse);
  }, true);
};


// Quick test whether object contains anything.
FB_JS.isEmpty = function(ob) {
  for (var i in ob) {
    return false;
  }
  return true;
}
