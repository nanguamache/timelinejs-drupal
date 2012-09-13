<?php
/**
 * @file
 * Theme functions for timeline.
 */

/**
 * Implementation of template_process for views-view-timeline.
 */
function template_preprocess_views_view_timelinejs(&$vars) {
  if (!empty($vars['view']->live_preview)) {
    drupal_set_message(t('The timeline cannot be viewed in the preview.'), 'warning');
    return;
  }
  // check if data is available
  if ($vars['options']['timelinejs']['data_count'] > 0) {
    // create actual timeline
    if (_timelinejs_include($vars['options']['timelinejs']['theme'])) {
      // preprocess css information for the template
      $vars['id'] = $vars['options']['timelinejs']['class'];
      $vars['timelinejs_theme'] = $vars['options']['timelinejs']['theme'];
    }
  }
  else {
    theme('timelinejs_nodata', array());
  }
}

/**
 * Overridable theme function.
 * This functions prints a warning when no data is available.
 */
function theme_timelinejs_nodata($vars) {
  drupal_set_message(t('There is no data available to show on the timeline'), 'warning');
}

/**
 * Inlude all js and css files that needs the timeline.
 *
 * @return: TRUE if everything could be included properly
 *          FALSE if Simile Exhibit Libraries couldn't be included properly
 */
function _timelinejs_include($timelinejs_theme) {
  static $loaded = FALSE;
  if (!$loaded) {
    // Check correct inclusion of local timeline library
    if ($timelinejs_library_status = timelinejs_validate_library()) {
      // some error messages are available, because of detected errors during validation
      if (is_string($timelinejs_library_status)) {
        drupal_set_message(check_plain($timelinejs_library_status));
        return FALSE;
      }
      // timeline library has been validated and will be added
      elseif ($timelinejs_library_status == TRUE) {
        global $base_path;
        $lib_path = libraries_get_path('timelinejs');
        // add Timeline Theme
        ctools_include('plugins');
        $timelinejs_themes = ctools_get_plugins('timelinejs', 'themes');
        if (isset($timelinejs_themes[$timelinejs_theme])) {
          drupal_add_css($timelinejs_themes[$timelinejs_theme]['files']['css']);
        }
        drupal_add_js($lib_path . '/js/timeline-min.js');
        drupal_add_js(drupal_get_path('module', 'timelinejs') . '/js/timelinejs.js', array('weight' => 13));
        return TRUE;
      }
    }
    drupal_set_message(t('TimelineJs Libraries not found'), 'error');
    return FALSE;
  }
}

