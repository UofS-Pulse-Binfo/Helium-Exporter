/**
 * Export to data for Helium script.
 */
 (function ($) {
  Drupal.behaviors.heliumExporter = {
    attach: function (context, settings) {           
      var w = $('#helium-exporter-contariner').width();
      
      $('#helium-exporter-diagram')
        .css('width', w + 'px')
        .css('height', w/4.5 + 'px');

      var slideWindow = $('#helium-exporter-show-window');
      // Listen to request to show steps.
      $('#helium-exporter-show').click(function(e) {
        e.preventDefault();
        slideWindow.slideDown(200);
      });

      // Listen to when user gets all steps.
      $('#helium-exporter-hide').click(function(e) {
        e.preventDefault();
        slideWindow.slideUp(200);
      });
      
}};}(jQuery));