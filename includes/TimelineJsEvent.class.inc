<?php
/**
 * @file
 * Class TimelineEvent
 */

/**
 *
 */
class TimelineJsEvent {
  var $view;
  var $row;
  var $data;
  var $dateformat;

  /**
   * Constructor.
   *
   * @param $view
   */
  function __construct(&$view, &$row, $dateformat) {
    $this->view       = $view;
    $this->dateformat = $dateformat;
    $this->data       = $this->buildData($row);
  }

  /**
   *
   */
  function getData() {
    return $this->data;
  }

  /**
   *
   */
  function buildData($row) {

    if (!empty($this->view->result)) {
      $tmp = $this->getDefinedFieldUsage('startDate');

      if (isset($row->{$tmp['alias']})) {
        $data         = array();
        $tmp_headline = $this->getDefinedFieldUsage('headline');
        $tmp_text     = $this->getDefinedFieldUsage('text');


        $v_text = $this->view->field[$tmp_text['fieldname']]->get_value($row);
        $v_text = array_shift($v_text);

        $events_validated = $this->logic($row);
        $data['startDate'] = $events_validated['startDate']['formated'];
        if (isset($events_validated['endDate']['formated'])) {
          $data['endDate'] = $events_validated['endDate']['formated'];
        }
        $data['headline'] = $row->{$tmp_headline['alias']};
        $data['text']     = $v_text['value'];
        $data['asset']    = array();

        // get asset fields

        foreach (array('credit', 'caption') as $asset_field) {
          $field_name = $this->view->style_options['fields']['asset'][$asset_field];
          if (isset($this->view->field[$field_name])) {
            $value = $this->view->field[$field_name]->get_value($row);
            $value = array_shift($value);
            $data['asset'][$asset_field] = $value['value'];
          }
        }
        // get media field_alias
        $field_name = $this->view->style_options['fields']['asset']['media'];
        if (isset($this->view->field[$field_name])) {
          $value = $this->view->field[$field_name]->get_value($row);
          $value = array_shift($value);
          $data['asset']['media'] = file_create_url($value['uri']);
        }
        return $data;
      }
    }
    return FALSE;
  }

  /**
   * This function validates the dates of one event that have a relation. Like start and end date.
   * There is a logic change to timeline. This conversion is done here.
   *
   * @return
   *   $event: Array with the converted date and a unix timestamp on which some logic has been done
   */
  function logic($row) {

    $event = array();
    foreach (array('startDate', 'endDate') as $event_field) {
      $field = $this->getDefinedFieldUsage($event_field);
      if (isset($row->{$field['alias']})) {
        // Loading plugin to handle date format
        $value = $this->view->field[$field['fieldname']]->get_value($row);
        // If this is a date field, get_value() returns an array of values.
        if (is_array($value)) {
          // @TODO Need to confirm if this will retrieve the right value for multiple value fields.
          $value = array_shift($value);
          switch ($event_field) {
            case 'startDate':
              $value = $value['value'];
              break;

            case 'endDate':
              $value = $value['value2'];
              break;
          }
        }
        if ($conversion_callback = timelinejs_plugins_get_callback($field['handler'], 'date_sources')) {
          $event[$event_field]['formated'] = call_user_func($conversion_callback, $value, 'iso8601', array('field' => $field));
          $event[$event_field]['timestamp'] = call_user_func($conversion_callback, $value, 'timestamp', array('field' => $field));
        }
      }
    }

    if (isset($event['endDate']) && $event['endDate']['timestamp'] <= $event['startDate']['timestamp']) {
      unset($event['endDate']);
    }

    if (isset($event['end'])) {
      $event['durationEvent'] = TRUE;
    }
    else {
      $event['durationEvent'] = FALSE;
    }

    return $event;
  }

  /**
   * The function returns the field that was defined by the views timeline settings.
   *
   * @param
   *   String that is expected to be "title", "start", "end", "earliestStart", "latestEnd", "icon"
   *
   * @return
   *   array with the following attributes set:
   *
   *   alias - The alias of the field
   *   handler - A string with the name of the handler
   *   date_format _ If it is an supported date information the expected date format
   */
  function getDefinedFieldUsage($type) {

    if ($timeline_settings_fields = $this->view->style_options['fields']) {
      if (isset($timeline_settings_fields[$type])) {
        if (is_string($timeline_settings_fields[$type])) {
          $fieldname = $timeline_settings_fields[$type];
        }
      }
      elseif (isset($timeline_settings_fields['advanced'][$type])) {
        if (is_string($timeline_settings_fields['advanced'][$type])) {
          $fieldname = $timeline_settings_fields['advanced'][$type];
        }
      }
      if (isset($fieldname) && $fieldname != '') {
        $field_information = array(
          'alias' => $this->view->field[$fieldname]->field_alias,
          'handler' => $this->view->field[$fieldname]->definition['handler'],
          'fieldname' => $fieldname,
        );
        if (!empty($this->view->field[$fieldname]->field_info)) {
          $field_information['date_format'] = $this->view->field[$fieldname]->field_info['type'];
          $field_information['tz_handling'] = 'site';
        }
        else {
          $field_information['date_format'] = 'timestamp';
          $field_information['tz_handling'] = 'site';
        }
        return $field_information;
      }
    }
    else {
      return FALSE;
    }
  }
}

