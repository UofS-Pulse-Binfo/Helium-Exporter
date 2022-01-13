<?php

/**
 * @file
 * The main functionality form page of this module.
 */

/**
 * Menu callback
 */
function helium_exporter_form($form, &$form_state) {
  $form['download_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download Data'),
    '#id' => 'helium-exporter-download-fieldset'
  );
  
  $experiment_options = helium_exporter_get_experiments();
  $experiment_options ?? array(0 => 'Experiments');
  $form['download_data']['experiment_select'] = array(
    '#type' => 'select',
    '#title' => '',
    '#options' => $experiment_options
  );

  return $form;
}
