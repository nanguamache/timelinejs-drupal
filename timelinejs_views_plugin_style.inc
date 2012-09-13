<?php
// $Id$


/**
 * @file
 * Defines the View Style Plugins for Timeline module.
 */

/**
 * Extending the view_plugin_style class to provide a timeline view style.
 */
class timelinejs_views_plugin_style extends views_plugin_style {

  // Basic properties that let the row style follow relationships.
  var $base_table = 'node';
  var $base_field = 'nid';

  /**
   * Add settings for the particular timeline.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['display'] = array('#type' => 'fieldset', '#title' => t('Display settings'), '#collapsible' => TRUE, '#collapsed' => FALSE);
    // Appearance settings
    ctools_include('plugins');
    $timelinejs_themes = ctools_get_plugins('timelinejs', 'themes');
    $timelinejs_themes_select = array('DefaultTheme' => t('Default Theme'), 'DarkTheme' => t('Dark Theme'));
    foreach ($timelinejs_themes as $theme_name => $theme_info) {
      $timelinejs_themes_select[$theme_name] = $theme_info['name'];
    }
    $form['display']['theme'] = array(
      '#type' => 'select',
      '#title' => t('Theme'),
      '#default_value' => $this->options['display']['theme'],
      '#options' => $timelinejs_themes_select,
      '#description' => t('Select a theme.'),
    );

    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#value' => t('You need at least one field before you can configure your field settings'),
        '#prefix' => '<div class="error form-item description">',
        '#suffix' => '</div>',
      );
    }
    else {
      // initializing select arrays for field selection form elements
      $field_names = array('' => '--');
      $date_field_names = array('' => '--');
      $iconimage_field_names = array('' => '--');

      // load all plugins for date and image field supports. provided by this or other modules
      ctools_include('plugins');
      $date_sources = ctools_get_plugins('timelinejs', 'date_sources');
      $image_sources = ctools_get_plugins('timelinejs', 'image_sources');

      // go through all available fields' handlers to check support for certain functionality
      foreach ($handlers as $field => $handler) {
        // obtain nice name of field for the ui
        $field_names[$field] = $handler->ui_name();
        if ($label = $handler->label()) {
          $field_names[$field] .= " (\"$label\")";
        }

        if (isset($handler->definition['field_name'])) {
          $field_name = $handler->definition['field_name'];
          $field_info = field_info_field($field_name);
        }

        // check if this field is supported as a date source
        foreach ($date_sources as $date_source) {
          if (get_class($handler) == $date_source['handler_name']) {
            if ($field_info['type'] == $date_source['field_type'] ||
              $field == 'created' || $field == 'changed'
            ) {
              $date_field_names[$field] = $field_names[$field];
            }
          }
        }
      }

      $form['fields'] = array(
        '#type' => 'fieldset',
        '#title' => 'Field usage',
        '#description' => t('Select the fields that contain the title, start time and end time of each item.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      $form['fields']['headline'] = array(
        '#type' => 'select',
        '#title' => 'Title',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['headline'],
        '#required' => TRUE,
      );

      $form['fields']['link'] = array(
        '#type' => 'checkbox',
        '#title' => 'Link to node',
        '#default_value' => $this->options['fields']['link'],
        '#required' => FALSE,
      );

      $form['fields']['text'] = array(
        '#type' => 'select',
        '#title' => 'Text',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['text'],
        '#required' => TRUE,
      );

      $form['fields']['startDate'] = array(
        '#type' => 'select',
        '#title' => 'Start',
        '#options' => $date_field_names,
        '#default_value' => $this->options['fields']['startDate'],
        '#required' => TRUE,
      );

      $form['fields']['endDate'] = array(
        '#type' => 'select',
        '#title' => 'End',
        '#options' => $date_field_names,
        '#default_value' => $this->options['fields']['endDate'],
      );

      $form['fields']['asset'] = array(
        '#type' => 'fieldset',
        '#title' => 'Asset',
        '#description' => t('Select fields for multimedia asset.'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );

      $form['fields']['asset']['media'] = array(
        '#type' => 'select',
        '#title' => 'Media',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['asset']['media'],
        '#required' => FALSE,
      );

      $form['fields']['asset']['credit'] = array(
        '#type' => 'select',
        '#title' => 'Credit',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['asset']['credit'],
        '#required' => FALSE,
      );
      $form['fields']['asset']['caption'] = array(
        '#type' => 'select',
        '#title' => 'Caption',
        '#options' => $field_names,
        '#default_value' => $this->options['fields']['asset']['caption'],
        '#required' => FALSE,
      );
    }
  }

  // Copied from views_plugin_row because not implemented in views_plugin_style to suport $base_table and $_base_field with relationships. -fd
  function query() {
    if (isset($this->base_table)) {
      if (isset($this->options['relationship']) && isset($this->view->relationship[$this->options['relationship']])) {
        $relationship = $this->view->relationship[$this->options['relationship']];
        $this->field_alias = $this->view->query->add_field($relationship->alias, $this->base_field);
      }
      else {
        $this->field_alias = $this->view->query->add_field($this->base_table, $this->base_field);
      }
    }
  }

  /**
   * Render the timeline style.
   * Data for the timelinejs is getting collected within the view
   */
  function render() {
    static $delta = 0;

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($this->view->result, $this->options['grouping']);

    // Build data for timeline
    ctools_include('TimelineJs.class', 'timelinejs', 'includes');
    ctools_include('TimelineJsEvent.class', 'timelinejs', 'includes');
    $timelinejs = new TimelineJs($this->view, $sets);
    // Allow other modules to alter the data array.
    drupal_alter('timelinejs_data', $timelinejs->data);
    // Convert data to json and push them to drupal's js behaviors
    $data = array();
    $data['timelinejs'][$delta] = array($timelinejs->data['id'] . '_' . $delta => $timelinejs->data);

    drupal_add_js($data, 'setting');

    // options that are used for template preprocessor
    $this->options['timelinejs'] = array(
      'class' => $this->view->name . '_' . $delta,
      'align' => '',
      'data_count' => count($timelinejs->data['timeline']),
      'theme' => $this->options['display']['theme'],
    );
    $delta++;
    return theme($this->theme_functions(), array(
        'view' => $this->view,
        'options' => $this->options,
      ));
  }
}

