<?php

/**
 * @file
 * The main functionality form page of this module.
 */

/**
 * Menu callback
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

  $ids = array_keys($experiment_options);
  $experiment_assets = helium_exporter_get_experiment_assets($ids[0]);
  $form['download_data']['experiment_select'] = array(
    '#type' => 'select',
    '#title' => '',
    '#options' => $experiment_options
  );
  
  // Select all and deselect all options.
  $field_options = '<span id="%s" class="helium-exporter-field-options"><a href="">Select All</a> | <a href="">Deselect All</a></span>';

  // FIELD: GROUP CHECKBOXES select germplasm.
  $form['download_data']['title_germplasm'] = array(
    '#title' => count($experiment_assets['germplasm']) . t(' Germplasm') . sprintf($field_options, 'helium-exporter-germplasm-options'),
    '#type' => 'item',
  );

  $form['download_data']['germplasm_select'] = array(
    '#type' => 'checkboxes',
    '#options' => $experiment_assets['germplasm'],
    '#prefix' => '<div class="helium-exporter-group-checkboxes-wrapper">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('helium-exporter-group-checkboxes')),
    '#id' => 'helium-exporter-germplasm-select',
  );

  // FIELD: GROUP CHECKBOXES select traits.
  $form['download_data']['title_traits'] = array(
    '#title' => count($experiment_assets['traits']) . t(' Traits') . sprintf($field_options, 'helium-exporter-traits-options'),
    '#type' => 'item',
  );

  $form['download_data']['traits_select'] = array(
    '#type' => 'checkboxes',
    '#options' => $experiment_assets['traits'],
    '#prefix' => '<div class="helium-exporter-group-checkboxes-wrapper">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('helium-exporter-group-checkboxes')),
    '#id' => 'helium-exporter-traits-select',
  );

  return $form;
}
