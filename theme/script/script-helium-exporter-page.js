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

      // Listen to checkbox group field options select and deselect.
      $('#helium-exporter-germplasm-options a, #helium-exporter-trait-options a')
        .click(function(e) {
          e.preventDefault();
          var txt = $(this).text();
          var relatedCheckboxes = $(this).parent().attr('id')
            .replace('options', 'checkboxes');
          
          var setTo = (txt == 'Select All') ? true : false;  
          
          console.log(relatedCheckboxes);

          $('#' + relatedCheckboxes + ' input').each(function() {
            $(this).attr('checked', setTo);
          });
        });
}};}(jQuery));