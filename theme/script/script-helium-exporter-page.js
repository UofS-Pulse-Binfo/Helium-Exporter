/**
 * Export to data for Helium script.
 */

 (function ($) {
  Drupal.behaviors.heliumExporter = {
    attach: function (context, settings) {   
      customCheckboxSubmit();

      // Add event listener to expand step by step guide window.
      $('#helium-exporter-show').once(function() {
        $(this).click(function(e) {
          e.preventDefault();

          $(this).text(function() {
            return ($(this).text() == 'Show me how') 
              ? 'Okay, Got It!' : 'Show me how';
          });

          $('#helium-exporter-show-window').slideToggle(200);
        });
      });

      // Array to hold all selections made for all available
      // custom checkbox set.
      var checkTable = new Array();
      // Each custom checkbox instance will convert all items
      // as search options for autocomplete field.
      var searchOptions = new Array()
      $('.helium-exporter-custom-checkbox-set').each(function(){
        var setId = $(this).attr('id');

        checkTable[ setId ] = new Array();
        searchOptions[ setId ] = new Array();

        $(this).find('li').each(function() {
          searchOptions[ setId ].push($(this).text());
        });
      });

      // CUSTOM CHECKBOX:
      
      // Class to indicate state of each selection 
      // on-Checked or off-Unchecked.
      var checkState = {
        'off': 'helium-exporter-item-uncheck',
        'on' : 'helium-exporter-item-check'
      };

      // Add event listener to items clicked or checked and
      // set all items to unchecked by default on load.
      $('.helium-exporter-custom-checkbox li')
        .addClass(checkState.off)
        .click(function() {        
          // Current item selected.
          var item = $(this);
          // Identify which custom checkbox set to affect event
          // and reference components to use.
          var customCheckboxSet = customCheckboxGetSet(item);

          // Set state of item (check or uncheck).
          // Elements: class to remove, class to add.
          var checkClass = [];

          if (item.hasClass(checkState.off)) {
            // Item checked from default unchecked.
            // OFF to ON.
            checkClass.push(checkState.off, checkState.on);
            // Save selection:
            checkTable[ customCheckboxSet ].push(item.index());
          }
          else {
            // Item unchecked from checked state.
            // ON to OFF.
            checkClass.push(checkState.on, checkState.off);
            // Omit previously selected item:
            var i = checkTable[ customCheckboxSet ].indexOf(item.index());
            if (i > -1) {
              checkTable[ customCheckboxSet ].splice(i, 1);
            }
          }
          
          // Apply class to show state of item.
          item
            .removeClass(checkClass[0])
            .addClass(checkClass[1]);

          // Save selections made into field API defined to capture 
          // items checked.
          customCheckboxSave(customCheckboxSet, checkTable[ customCheckboxSet ]);
          customCheckboxSubmit();
        });
      //

      // Add event listener to select and deselect option.
      $('.helium-exporter-field-options a:last-child')
        .click(function(e) {
          e.preventDefault();
          // Current item selected.
          var element = $(this);
            
          // Identify which custom checkbox set to affect event
          // and reference components to use.
          var customCheckboxSet = customCheckboxGetSet(element);
          var items = $('#' + customCheckboxSet).find('li');

          // Set state of item (check or uncheck).
          // Elements: class to remove, class to add.
          var checkClass = [];

          if (element.hasClass(checkState.on)) {
            // Item select all. OFF to ON (all).
            checkClass.push(checkState.on, checkState.off);

            var countItem = items.length;
            checkTable[ customCheckboxSet ] = new Array(countItem).fill().map((v, i) => i);
          }
          else {
            // Item deselect all. ON to OFF (all).
            checkClass.push(checkState.off, checkState.on);
            checkTable[ customCheckboxSet ] = [];
          }
            
          // Apply class to show state of item.
          element
            .removeClass(checkClass[0])
            .addClass(checkClass[1]);
            
          items
            .removeClass(checkClass[1])
            .addClass(checkClass[0]);

          // Save selections made into field API defined to capture 
          // items checked.
          customCheckboxSave(customCheckboxSet, checkTable[ customCheckboxSet ]);
          customCheckboxSubmit();
        });
      //
      
      // AUTOCOMPLETE SEARCH

      // Add event listener to search link.
      var searchId = '';

      $('.helium-exporter-field-options a:first-child')
        .click(function(e) {
          e.preventDefault();
          var element = $(this);

          // Identify which custom checkbox set to affect event
          // and reference components to use.
          var customCheckboxSet = customCheckboxGetSet(element);
          $('#' + customCheckboxSet)
            .find('.helium-exporter-custom-checkbox-search')
            .fadeIn('fast')
            .find('input').val('').focus();
          
          // Scroll to top of list to start search.
          $('#' + customCheckboxSet)
            .find('.helium-exporter-custom-checkbox')
            .scrollTop(-1);

          // Set the search id to parent container
          // for autocomplete to reference search options values.
          searchId = customCheckboxSet;
        });
      //

      // Add event listener to close search window.
      $('.helium-exporter-custom-checkbox-search a')
        .click(function(e) {
          e.preventDefault();
          $(this)
            .parent().fadeOut('fast')
            .find('input').val('');
        });
      //

      // Add event listener to search field when selected/on focus.
      $('.helium-exporter-custom-checkbox-search input')
        .click(function() {
          if ($(this).val()) {
            $(this).select();
          }
        });
      //  

      // Convert text field in search window into autocomplete search field.
      $('.helium-exporter-custom-checkbox-search input')
        .autocomplete(
          {
            select: function(event, ui) {
              var customCheckboxSet = $('#' + searchId)
                .find('.helium-exporter-custom-checkbox');

              // Search option selecttion:
              // Attempt to scroll to specific otion in window.
              var optionIndex = searchOptions[ searchId ].indexOf(ui.item.value.trim());
              // Get position of this option in reference to the window.
              var element = customCheckboxSet.find('li').eq(optionIndex);
              var elementPos = element.position();
              customCheckboxSet.scrollTop(elementPos.top);
              // Mark found element.
              element.prepend('<span><div></div></span>');
              
              var t = 0;
              var timer = setInterval(function() {
                if (t < 5) {
                  var o = (t%2 == 0) ? 0 : 1;
                  element.find('span > div').css('opacity', o);
                } 
                else {
                  element.find('span').remove();
                  clearInterval(timer);
                }

                t++;
              }, 300);

              $(this).parent().fadeOut('slow');
            },
            source: function(request, response) {
              // Search options:
              var results = $.ui.autocomplete.filter(searchOptions[ searchId ], request.term);
              // Fist 5 results.
              response(results.slice(0, 5));
            }
          }
        );
      //    
      

      /**
       * Save selections (checked items) made.
       * Index of the item as assigned in DOM will be save
       * and this will be reference backed to the list/array
       * of options returned in the backend to identify a selection.
       *
       * @param customCheckboxSet
       *   Reference to the parent element (attr ID) containing the
       *   element that triggered the event (save). This container
       *   will allow for reference to Drupal form field API to hold 
       *   selections made.
       * 
       * @param items
       *   Items selected/check to save.
       */
      function customCheckboxSave(customCheckboxSet, items) {
        // Add event listener to submit button - ensure selection made.
        var submit = $('#helium-exporter-download-submit');

        // Reset field each time before applying recent selections.
        var selectionsField = $('#' + customCheckboxSet).find('input[type = hidden]');
        selectionsField.val('');

        if (items.length > 0) {
          // Start empty each time.
          selectionsField.val('');
          items.sort();
          
          selectionsField.val(JSON.stringify(items));
        }
      }

      /**
       * Find the parent of element that triggered
       * an event. The returned parent will indicate the custom
       * checkbox set to affect.
       * 
       * @param $item
       *   Object, reference to the element clicked/selected.
       * 
       * @return string
       *   Id attribute value of the container div holding the custom
       *   checkbox set the element belongs.
       */
      function customCheckboxGetSet(item) {
        var parent = item.closest('.helium-exporter-custom-checkbox-set')
          .attr('id');
        
        return parent;
      }

      /**
       * Ensure that form can only be submitted if selections have
       * been made for all instances of custom checkbox.
       */
      function customCheckboxSubmit() {
        var check = new Array();

        $('.helium-exporter-custom-checkbox-set').each(function() {
          var v = $(this).find('input[type = hidden]').val();
          var o = (v == '') ? 0 : 1;
          check.push(o);            
        });

        if (check.includes(0)) {
          // A set without a value, disable submit.
          // Disable submit button until selections made.
          $('#helium-exporter-download-submit')
            .addClass('form-button-disabled')
            .attr('disabled', true);          
        }
        else {
          // All clear for submit.
          $('#helium-exporter-download-submit')
            .removeClass('form-button-disabled')
            .attr('disabled', false);          
        }
      }      
}};}(jQuery));