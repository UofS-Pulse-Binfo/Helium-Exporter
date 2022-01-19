<?php
/**
 * @file
 * Contains general API required by this module.
 */

 /**
  * Fetch all experiments with raw phenotypic data.
  *
  * @return array
  *   Associative array where the key is the project id and value as the project name
  *   This array will become array value in #option of the select field.
  */
function helium_exporter_get_experiments() {
  // Initial value of the select experiment field when there is no
  // experiment in projects.
  $experiments_option = array(0 => 'Select Experiment');
  
  // All experiments in rawphenotypes.
  $experiments = chado_query("
    SELECT project_id, name FROM {project} WHERE project_id IN (
      SELECT DISTINCT project_id FROM pheno_project_cvterm 
    ) ORDER BY name ASC
  ");
  
  // Create associative array of experiments setting the first element
  // of the select field to the initial value plus all the experiments
  // returned by the query.
  if ($experiments->rowCount() > 0) {
    // Reset default and set it to the first experiment
    // in the list.
    $experiments_option = [];
    foreach ($experiments as $exp) {
      $has_data = helium_exporter_experiment_has_data($exp->project_id);
      if ($has_data) {
        $experiments_option[ $exp->project_id ] = $exp->name;
      }
    }
  }
  
  return $experiments_option;
}

/**
 * Get experiment germplasm/lines and traits.
 * 
 * @param $experiment_id
 *   Integer, project_id number.
 * 
 * @return array
 *   1 array - containing the germplasm/lines
 *   1 array - containing the traits
 *   both arrays as associative arrays where the key is the id (stock_id/cvterm_id)
 *   and value as stock name or cvterm name.
 *   
 *   Experiment name.
 */
function helium_exporter_get_experiment_assets($experiment_id) {
  // Germplasm.
  $germplasm_option = array();
  
  $germplasm = chado_query("
    SELECT stock_id, name FROM {stock} WHERE stock_id IN (
      SELECT DISTINCT stock_id FROM pheno_plant WHERE plant_id IN (
        SELECT plant_id FROM pheno_plant_project WHERE project_id = :experiment
      ) 
    ) ORDER BY name ASC
  ", array(':experiment' => $experiment_id));
  
  if ($germplasm->rowCount() > 0) {
    $germplasm_option = $germplasm->fetchAllKeyed(0, 1);
  }

  // Traits.
  $trait_option = array();
  
  // All traits excluding plant properties rep, entry, location etc.
  $traits = chado_query("
    SELECT cvterm_id, name FROM {cvterm} WHERE 
    name NOT IN ('Rep', 'Entry', 'Location', 'Name', 'Plot', 'Planting Date (date)', '# of Seeds Planted (count)')
    AND cvterm_id IN (
      SELECT DISTINCT cvterm_id FROM pheno_project_cvterm WHERE project_id = :experiment
    ) ORDER BY name ASC
  ", array(':experiment' => $experiment_id));

  if ($traits->rowCount() > 0) {
    $trait_option = $traits->fetchAllKeyed(0, 1);
  }

  // Which experiment assets belong to.
  $experiment = chado_query("
    SELECT name FROM {project} WHERE project_id = :experiment LIMIT 1
  ", array(':experiment' => $experiment_id))
    ->fetchField();

  return array(
    'experiment' => $experiment,
    'germplasm' => $germplasm_option,
    'traits' => $trait_option
  );
}

/**
 * Test to ensure that only experiment with data can be suggested to user.
 * 
 * @param $experiment
 *   Integer, project_id (project tables).
 * 
 * @return boolean
 *   True, experiment has data and false otherwise (exclude from list).
 */
function helium_exporter_experiment_has_data($experiment) {
  // See if experiment has plant id (has data) at least one would suffice.
  $data = chado_query("
    SELECT plant_project_id FROM pheno_plant_project 
    WHERE project_id = :experiment LIMIT 1
  ", array(':experiment' => $experiment));

  return ($data->rowCount() > 0) ? TRUE : FALSE;
}