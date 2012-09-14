Drupal.timelinejs = {
  timerID: null,
  getTheme: function(theme, timeline_start, timeline_end) {
    var theme = TimelineJs[theme].create();
    return theme;
  },
  createWidget: function(id,args) {
   args = eval(args);
   var timeline = new VMM.Timeline(id);
   timeline.init({source:args} );
  }
};

Drupal.behaviors.timelinejs = {
  attach:  function(context) {
    if(VMM.Timeline && Drupal.settings.timelinejs) {
      jQuery.each(Drupal.settings.timelinejs, function(i, v) {
        jQuery.each(v, function(id, data) {
           Drupal.timelinejs.createWidget(id,data);
        });

      });
    }
  }
};

