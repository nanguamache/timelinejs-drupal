<?php
/**
 * @file
 * Defines the View Style Plugins for Timeline module.
 */

/**
 * Implements of hook_views_style_plugins(). Adds view types to views UI interface.
 */
function timelinejs_views_plugins() {
  return array(
    'module' => 'timelinejs',
    'style' => array(
      'timelinejs' => array(
        'title' => t('TimelineJS'),
        'help' => t('Displays content on a verite.co Timeline.'),
        'handler' => 'timelinejs_views_plugin_style',
        'uses options' => TRUE,
        'uses row plugin' => TRUE,
        'uses grouping' => TRUE,
        'uses fields' => TRUE,
        'type' => 'normal',
        'parent' => 'list',
        'path' => drupal_get_path('module', 'timelinejs'),
        'theme' => 'views_view_timelinejs',
        'theme file' => 'timelinejs.theme.inc',
        'even empty' => TRUE,
      ),
    ),
  );
}

