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
  // FIELD: FIELDSET download data.
  $form['download_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download Data'),
    '#id' => 'helium-exporter-download-fieldset'
  );
  
  // FIELD: SELECT FIELD select experiments.
  $experiment_options = helium_exporter_get_experiments();
  $experiment_assets = '';

  // An experiment has been selected - get germplasm and traits.
  if (isset($form_state['values']) && $form_state['values']['experiment_select'] != '') {
    $default_experiment = $form_state['values']['experiment_select'];
    unset($experiment_assets);
    $experiment_assets = helium_exporter_get_experiment_assets($default_experiment);
  }
  else {
    $ids = array_keys($experiment_options);
    $default_experiment = $ids[0];
    unset($experiment_assets);
    $experiment_assets = helium_exporter_get_experiment_assets($default_experiment);
  }

  $AJAX_wrapper_id = 'helium-exporter-ajax-wrapper';
  $form['download_data']['experiment_select'] = array(
    '#type' => 'select',
    '#title' => t('Experiment'),
    '#options' => $experiment_options,
    '#ajax' => array(
      'event' => 'change',
      'callback' => 'helium_exporter_ajaxrequest',
      'wrapper' => $AJAX_wrapper_id,
      'progress' => array('type' => '', 'message' => ''),
    )
  );
  
  // Select all and deselect all options.
  $form['download_data']['AJAX_wrapper'] = array(
    '#prefix' => '<div id="' . $AJAX_wrapper_id . '">',
    '#suffix' => '</div>'
  );

  $field_options = '<span id="%s" class="helium-exporter-field-options"><a href="">Select All</a> | <a href="">Deselect All</a></span>';

  // FIELD: GROUP CHECKBOXES select germplasm.
  $form['download_data']['AJAX_wrapper']['title_germplasm'] = array(
    '#title' => count($experiment_assets['germplasm']) . t(' Germplasm') . sprintf($field_options, 'helium-exporter-germplasm-options'),
    '#type' => 'item',
  );

  $form['download_data']['AJAX_wrapper']['germplasm_select'] = array(
    '#type' => 'checkboxes',
    '#options' => $experiment_assets['germplasm'],
    '#default_value' => array(),
    '#prefix' => '<div id="helium-exporter-germplasm-checkboxes" class="helium-exporter-group-checkboxes-wrapper">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('helium-exporter-group-checkboxes')),
    '#id' => 'helium-exporter-germplasm-select',
  );

  // FIELD: GROUP CHECKBOXES select traits.
  $form['download_data']['AJAX_wrapper']['title_traits'] = array(
    '#title' => count($experiment_assets['traits']) . t(' Traits') . sprintf($field_options, 'helium-exporter-trait-options'),
    '#type' => 'item',
  );

  $form['download_data']['AJAX_wrapper']['traits_select'] = array(
    '#type' => 'checkboxes',
    '#options' => $experiment_assets['traits'],
    '#default_value' => array(),
    '#prefix' => '<div id="helium-exporter-trait-checkboxes" class="helium-exporter-group-checkboxes-wrapper">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('helium-exporter-group-checkboxes')),
    '#id' => 'helium-exporter-traits-select',
  );

  $form['download_data']['download_submit'] = array(
    '#type' => 'submit',
    '#value' => 'Download',
    '#id' => 'helium-exporter-download-submit',
  );

  return $form;
}

/**
 * AJAX callback
 */
function helium_exporter_ajaxrequest($form, $form_state) {
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

    $checkbox = array(
      'germplasm' => $form_state['values']['germplasm_select'],
      'traits' => $form_state['values']['traits_select']
    );

    // Inspect each checkbox to make sure an item was selected
    // before submitting the form.
    $failed = array(
      'germplasm' => 0,
      'traits' => 0
    );

    // Failed by default and prove otherwise.
    // Only one selection is sufficient the field is good.
    foreach($checkbox as $fld => $chk) {
      foreach($chk as $id => $item) {
        if ($item) {
          $failed[ $fld ] = 1;
          break;
        }
      }
    }

    if (!$failed['germplasm']) {
      form_set_error('germplasm_select', 'Please select germplasm.');
    }

    if (!$failed['traits']) {
      form_set_error('traits_select', 'Please select traits.');
    }
  }
}

/**
 * Implements hoo_submit().
 */
function helium_exporter_form_submit($form, &$form_state) {
  $experiment = $form_state['values']['experiment_select'];
  $checkbox = array(
    'germplasm' => $form_state['values']['germplasm_select'],
    'traits' => $form_state['values']['traits_select']
  );

  // Filter checkboxes to only those that have been check,
  // drop the rest.
  $selected = array(
    'germplasm' => [],
    'traits' => []
  );

  foreach($checkbox as $fld => $chk) {
    foreach($chk as $id => $item) {
      if ($item) {
        // Has been checked! save it.
        $selected[ $fld ][] = $id; 
      }
    }
  }

  $germplasm = implode('+', $selected['germplasm']);
  $traits = implode('+', $selected['traits']);
  $querystring = 'e=' . $experiment . '&g=' . $germplasm . '&t=' . $traits;
  
  $form_state['redirect'] = array(
    '/helium/exporter/download',
    array(
      'query' => array('code' => base64_encode($querystring)),
    ),
  );
}