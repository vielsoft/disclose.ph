/**
 * @file
 *
 * Javascript specific to iframe tabs.
 */

/**
 * Enable canvas page specific javascript on this page.
 */
Drupal.behaviors.fb_tab = {
  attach: function(context, settings) {
    // Resize if body class includes fb_canvas-resizable.
    jQuery('body.fb_tab-resizable:not(.fb_tab-processed)').each(function () {
      jQuery(this).addClass('fb_tab-processed');
      if (typeof(FB) == 'undefined') {
        // FB not yet initialized.
        jQuery(document).bind('fb_init', FB_Tab.setAutoResize);
      }
      else {
        // FB already initialized.
        FB_Tab.setAutoResize();
      }
      jQuery(document).bind('fb_init', FB_Tab.setAutoResize);
    });
  }
};

FB_Tab = function(){};

/**
 * Called after Facebook javascript has initialized.  Global FB will be set.
 */
FB_Tab.setAutoResize = function() {
  FB.Canvas.setAutoGrow();
};
