<?php

/**
 * @file
 * The main functionality form page of this module.
 */

/**
 * Menu callback.
 * Implements hook_form().
 */
function helium_exporter_form($form, &$form_state) {
  // #
  // FIELD: FIELDSET download data.
  $form['download_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download Data'),
    '#id' => 'helium-exporter-download-fieldset'
  );
    
    // All active experiments.
    $experiment_options = helium_exporter_get_experiments();
    $experiment_assets = '';

    if (isset($form_state['values']) && $form_state['values']['experiment_select'] != '') {
      // An experiment has been selected using this select element.
      $default_experiment = $form_state['values']['experiment_select'];
      $experiment_assets = helium_exporter_get_experiment_assets($default_experiment);
    }
    else {
      // Get the first item in the list of experiments and set it
      // as the default experiment on load.
      $ids = array_keys($experiment_options);
      $default_experiment = $ids[0];
      $experiment_assets = helium_exporter_get_experiment_assets($default_experiment);
    }

    // #
    // FIELD: SELECT FIELD select experiments.
    $AJAX_wrapper_id = 'helium-exporter-ajax-wrapper';
    $form['download_data']['experiment_select'] = array(
      '#type' => 'select',
      '#title' => t('Experiment'),
      '#options' => $experiment_options,
      '#id' => 'helium-exporter-experiment-select',
      '#ajax' => array(
        'event' => 'change',
        'wrapper' => $AJAX_wrapper_id,
        'callback' => 'helium_exporter_ajax_callback',
        'progress'  => array('type' => '', 'message' => ''),
      )
    );
  
    // AJAX wrapper element - everything wrapped in this element
    // will be affected by AJAX change event.
    $form['download_data']['AJAX_wrapper'] = array(
      '#prefix' => '<div id="' . $AJAX_wrapper_id . '">',
      '#suffix' => '</div>'
    );

      
      // CUSTOM CHECKBOX:
      
      // Custom checkbox components: Title/Options/Search Window/Checkboxes
      $markup = '
        <strong>%s:</strong> %d <span class="helium-exporter-selections"></span>
        <span class="helium-exporter-field-options">
          <a href="#" alt="Search a value" title="Search a value">Search</a> | 
          <a class="helium-exporter-item-check" href="#" alt="Select or deselect all options" title="Select or deselect all options">All</a>
        </span>
        <div class="helium-exporter-custom-checkbox-search">
          <input type="text" value="">
          <a href="#" alt="Close search window" title="Close search window">Close</a> | <a href="#" alt="Select value" title="Select value">Select value</a>
        </div>
        <div class="helium-exporter-custom-checkbox">
          %s
        </div>
      ';
  
      // Create custom checkbox for the following:
      // Order is relevant and is the order each will appear.
      $sets = array(
        'germplasm' => $experiment_assets['germplasm'],
        'traits' => $experiment_assets['traits']
      );

      // Save titles for each set for hooks to be able
      // to identify fields to process.
      $form_state['custom_checkbox_sets'] = $sets;
      
      // Construct sets.
      foreach($sets as $title => $items) {
        $theme_items = theme('item_list', array('items' => array_values($items)));
        $full_markup = sprintf($markup, t(ucfirst($title)), count($items), $theme_items);
        
        // Create markup.
        $form['download_data']['AJAX_wrapper'][ 'custom_checkbox_' . $title ] = array(
          '#type' => 'markup',
          '#prefix' => '<div id="helium-exporter-custom-checkbox-' . $title . '" class="helium-exporter-custom-checkbox-set">' . $full_markup,
          '#suffix'  => '</div>',
        );
        
        // #
        // FIELD: HIDDEN Selections made in custom checkbox will be handled by
        // this hidden field. Validate only this field.
        $form['download_data']['AJAX_wrapper'][ 'custom_checkbox_' . $title ][ 'field_' . $title ] = array(
          '#type' => 'hidden',
        );
      }
  
  // Submit options.
  $form['download_data']['submit_container'] = array(
    '#prefix' => '<div id="helium-exporter-submitcontrols-container">',
    '#suffix' => '</div>'
  );
    
    // #
    // FIELD: CHECKBOX option to either return all germplasm relationships
    // or just parental relationships.
    $form['download_data']['submit_container']['full_lineage'] = array(
      '#type' => 'checkbox',
      '#title' => t('Parental Relationships Only'),
      '#default_value' => 0,
      '#prefix' => '<div id="helium-exporter-full-lineage" title="Export parental relationships only">',
      '#suffix' => '</div>'
    );

    // #
    // FIELD: SUBMIT
    $form['download_data']['submit_container']['download_submit'] = array(
      '#type' => 'submit',
      '#value' => 'Download',
      '#id' => 'helium-exporter-download-submit',
    );

    
  return $form;
}

/**
 * AJAX callback
 */
function helium_exporter_ajax_callback($form, $form_state) {
  return $form['download_data']['AJAX_wrapper'];
}

/**
 * Implements hook_validate().
 */
function helium_exporter_form_validate($element, &$form_state) {
  if ($form_state['values']) {
    $experiment = $form_state['values']['experiment_select'];
    if (!$experiment) {
      form_set_error('experiment_select', 'Please select an experiment.');
    }
    
    $custom_checkbox_sets = $form_state['custom_checkbox_sets'];
    $custom_checkbox_sets = array_keys($custom_checkbox_sets);

    foreach($custom_checkbox_sets as $set) {
      $title = 'field_' . $set;
      if (empty($form_state['values'][ $title ])) {
        form_set_error('', 'Please select ' . $set . '.');
        
        $form_state['custom_checkbox_validation'] = TRUE;
      }
    }
  }
}

/**
 * Implements hook_submit().
 */
function helium_exporter_form_submit($form, &$form_state) {
  $experiment = $form_state['values']['experiment_select'];
  $lineage = $form_state['values']['full_lineage'];
  
  $custom_checkbox_sets = $form_state['custom_checkbox_sets'];

  $checkbox = array(
    'germplasm' => $form_state['values']['field_germplasm'],
    'traits' => $form_state['values']['field_traits']
  );

  // Filter checkboxes to only those that have been check,
  // drop the rest.
  $selected = array(
    'germplasm' => [],
    'traits' => []
  );

  foreach($checkbox as $fld => $chk) {
    $chk = json_decode($chk);

    foreach($chk as $id) {      
      // Has been checked! save it.
      $selected[ $fld ][] = array_keys($custom_checkbox_sets[ $fld ])[ $id ];   
    }
  }
  
  $germplasm = implode('+', $selected['germplasm']);
  $traits = implode('+', $selected['traits']);
  // This is the filename to be used for categorical data.
  $He_categorical_file = 'He_categorical_' .  date('YMd') . '_' . time() . '.helium';
  $querystring = 'e=' . $experiment . '&g=' . $germplasm . '&t=' . $traits . '&l=' . $lineage . '&f=' . $He_categorical_file;

  $filename = 'He_parameters_' . date('YMd') .'_'. time() . '.txt';
  $path = file_directory_temp() . '/';

  $tmp_file =  file_create_url($path . $filename);
  $FILE = fopen($tmp_file, 'w') or die ('Unable to read temporary file.');
  fwrite($FILE, $querystring);
  fclose($FILE); 

  $new_querystring = 'he=' . $filename;
  $form_state['redirect'] = array(
    '/helium/exporter/download',
    array(
      'query' => array('code' => base64_encode($new_querystring)),
    ),
  ); 
}