/**
 * Export to data for Helium script.
 */
 (function ($) {
  Drupal.behaviors.heliumExporter = {
    attach: function (context, settings) {           
      var slideWindow = $('#helium-exporter-show-window');
      // Listen to request to show steps.
      $('#helium-exporter-show').click(function(e) {
        e.preventDefault();
        var t = $(this).text();

        if (t == 'Show me how') {
          $('#helium-exporter-diagram').fadeIn('slow');
          $(this).text('Okay, Got It!');
          slideWindow.slideDown(200);
        }
        else {
          $('#helium-exporter-diagram').fadeOut('fast');
          $(this).text('Show me how');
          slideWindow.slideUp(200);
        }
      });
}};}(jQuery));