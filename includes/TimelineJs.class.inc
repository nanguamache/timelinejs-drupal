<?php
/**
 * @file
 * Class TimelineJs
 */

/**
 *   id - the XHTML identifier of the timeline element
 *   timezone - the timezone represented by # of hours difference from GMT
 *   theme_name - for the use of several themes
 */
class TimelineJs {

  var $view;
  var $data;
  var $events;
  var $sets;
  var $dateformat;
  /**
   * Constructor.
   *
   * @param $view
   */
  function __construct(&$view, $sets) {
    $this->view          = $view;
    $this->sets          = $sets;
    $this->dateformat    = 'iso8601';
    $this->data          = $this->getEvents();
    $this->data['id']    = $this->view->name;
    $this->data['theme'] = $view->style_options['display']['theme'];
  }

  function timelinejs_to_json() {
    drupal_add_js(array('timelinejs' => array($this->data['id'] => $this->data)), 'setting');
  }

  function debug() {
    return $this->data;
  }

  /**
   *
   */
  function getEvents() {
    $events = array();
    if (!empty($this->view->result)) {
      $this->view->row_index = 0;
      foreach ($this->sets as $k => $set) {
        $cnt = 0;
        foreach ($set as $row_key => $row) {
          $this->view->row_index = $row_key;
          $event = new TimelineJsEvent($this->view, $row, $this->dateformat);
          // fetch array structure for timelinejs json
          if ($event->getData()) {
            if ($cnt == 0) {
              $events = $event->getData();
              $events['type'] = 'default';
            }
            else {
              $events['date'][] = $event->getData();
            }
            $cnt++;
          }
        }
      }
    }
    return array(
      'timeline' => $events,
      'dateTimeFormat' => $this->dateformat,
    );
  }

  /**
   * Get the actual field value based on the timeline views settings and the result set of events.
   *
   * @param
   *   $option - name of the selected option
   *   result - the result set of events
   *
   * @return
   *   formated date
   *
   * Note: -fd
   * First and last fields have to be improved:
   * If the view is not sorted on the start date this would lead wrong dates.
   * Last should also check if the event has an "end" date
   */

  // @TODO check logic of $options - not yet done for new timelinejs array structure
  function getDefinedDateField($option, $options_field = '') {
    $timezone = timeline_get_timezone();

    // Make sure that if there is no result that a valid date (today) is getting served.
    if ($option != 'today' && $option != 'custom' && count($this->data['events']['events']) == 0) {
      $option = 'today';
    }
    switch ($option) {
      case 'today':
        return timelinejs_convert_to_gregorian(REQUEST_TIME, 'timestamp', $timezone, $timezone);

      case 'first':
        return timelinejs_convert_to_gregorian($this->data['events']['events'][0]['start'], 'iso8601', $timezone, $timezone);

      case 'last':
        return timelinejs_convert_to_gregorian($this->data['events']['events'][count($this->data['events']['events']) - 1]['start'], 'iso8601', $timezone, $timezone);

      case 'middle':
        $end    = timelinejs_convert_to_timestamp($this->data['events']['events'][count($this->data['events']['events']) - 1]['start'], 'iso8601', $timezone, $timezone);
        $start  = timeline_convert_to_timestamp($this->data['events']['events'][0]['start'], 'iso8601', $timezone, $timezone);
        $middle = round(($start + $end) / 2, 0);
        return timelinejs_convert_to_gregorian($middle, 'timestamp', $timezone, $timezone);

      case 'median':
        foreach ($this->data['events']['events'] as $date) {
          $t[] = timelinejs_convert_to_timestamp($date['start'], 'iso8601', $timezone, $timezone);
        }
        sort($t);
        return timelinejs_convert_to_gregorian($t[round(count($t) / 2)], 'timestamp', $timezone, $timezone);

      case 'custom':
        if ($options_field != '') {
          return timelinejs_convert_to_gregorian(strtotime($options_field), 'timestamp', $timezone, $timezone);
        }
    }
    return FALSE;
  }
}

