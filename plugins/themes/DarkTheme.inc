<?php

/**
 * @file
 * Implementation of the theme plugin for the classic theme
 */
$lib_path = libraries_get_path('timelinejs');
$plugin = array(
  'name' => t('Dark Theme'),
  'files' => array(
    'css' => $lib_path . '/css/themes/dark.css',
  ),
);
