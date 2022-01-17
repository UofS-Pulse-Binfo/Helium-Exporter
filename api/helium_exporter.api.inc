<?php
/**
 * @file
 * Contains general API required by this module.
 */

 /**
  * Fetch all experiments with raw phenotypic data.
  */
function helium_exporter_get_experiments() {
  $all_project = chado_query("
    SELECT project_id, name FROM {project} ORDER BY name ASC
  ");

  return ($all_project->rowCount() > 0) ? $all_project->fetchAllAssoc(0, 1) : [1 =>  'a'];
}
