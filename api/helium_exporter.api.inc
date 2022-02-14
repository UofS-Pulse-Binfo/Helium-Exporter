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

  return 
    array(
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

/**
 * Fetch parents (mother and father) of stocks.
 * 
 * @param $germplasm
 *   Integer, stock id number.
 * @param $lineage
 *   Boolean true or false, true indicates to include
 *   selection of aside from maternal and paternal information.
 * 
 * @return array
 *   Associative array where keys are mother and father corresponding to 
 *   mother and father of a give germplasm.
 */
function helium_exporter_get_parents($germplasm, $lineage = 0) {
  $relations = helium_exporter_get_configuration();
  $germplasm_record = helium_exporter_get_germplasm($germplasm);
  
  // NON Parental Relationships:
  // See if any selections of etc., in each level of the tree a counter (level)
  // is set. The final result will be sorted by level (desc) the top most first
  // where the parent will then be search from this point on.

  // RECURSIVE: starting with the non recursive query (1ST SELECT): the id (object) returned
  // will passed to the recursive part (2ND SELECT) of the query. In each iteration, object
  // id is used to fetch all non parental relationships leading to the
  // line where parents can be identified in the succeeding query and recursion stops.
  $subject_relations = "
    WITH RECURSIVE relations AS (      
      SELECT 
        1 AS level, name AS subject_name, stock_id AS subject_id, 1 AS type, 
        name AS object_name, stock_id AS object_id
      FROM {stock} WHERE stock_id = :germplasm
    
      UNION ALL
  
      SELECT 
        relations.level + 1 AS level, relations.object_name AS subject_name, rel.subject_id AS subject, 
        rel.type_id AS type, stock.name AS object_name, rel.object_id AS object_id 
      FROM {stock_relationship} AS rel INNER JOIN relations ON rel.subject_id = relations.object_id
      INNER JOIN {stock} ON rel.object_id = stock.stock_id
      WHERE rel.type_id IN (:subject_relations)
    )
    
    SELECT * FROM relations ORDER BY level DESC limit 500
  ";

  $selection = chado_query(
    $subject_relations, 
    array(':germplasm' => $germplasm, ':subject_relations' => array_keys($relations['subject']))
  );

  $parents_row = array();
  $full_row = array();

  // Top most node before parents.
  $top = array();
  foreach($selection as $i => $sel) {
    if ($i == 0) {
      // Reference the top most node. If node has no parent and lineage
      // is set to off, this will be the only node returned.
      $top = array('id' => $sel->object_id, 'name' => $sel->object_name);
    }
    
    // Anything other mother and father will be tagged S or the type name.
    // Helium does not include any data that will indicate anything.
    if ($relations['subject'][ $sel->type ]) {
      $full_row[] = array($sel->subject_name, $sel->object_name, $relations['subject'][ $sel->type ]);
    }
  }
  
  // Parental Relationships:
  // Get maternal and paternal parent from the
  // selection result, top node leading to parents.
  $parents = chado_query("
    SELECT rel.subject_id, rel.type_id, rel.object_id, name
    FROM {stock_relationship} AS rel INNER JOIN {stock} ON rel.subject_id = stock.stock_id
    WHERE rel.object_id = :selection and rel.type_id IN (:object_relations)
  ", array(':selection' => $top['id'], ':object_relations' => array_keys($relations['object'])));
  
  $no_parents = 0;
  if ($parents->rowCount() > 0) {
    foreach($parents as $item) {
      // F - Female parent and M - Male parent.
      // NOT! F - father and M - mother.
      $type = ($item->type_id == $relations['maternal']) ? 'F' : 'M';
      $line_name = ($lineage) ? $germplasm_record->name : $top['name'];
      $line = array($line_name, $item->name, $type);

      if ($lineage) {
        $parents_row[] = $line;
      }
      else {
        $full_row[] = $line;
      }
    }
  }
  else {
    $no_parents = 1;
  }

  // Return values:
  if ($lineage) {
    return ($no_parents) ? $full_row : $parents_row;
  }
  else {
    return $full_row;
  }
} 

/**
 * Fetch germplasm record.
 * 
 * @param $germplasm
 *   Integer, stock_id number.
 * 
 * @param object
 *   Stock record that matched the stock_id
 */
function helium_exporter_get_germplasm($germplasm) {
  $id = $germplasm;

  $germplasm = chado_query("
    SELECT * FROM {stock} WHERE stock_id = :germplasm LIMIT 1
  ", array(':germplasm' => $id));

  return ($germplasm->rowCount() > 0) ? $germplasm->fetchObject() : 0;
}

/**
 * Get stock relationships
 */
function helium_exporter_get_relationship($germplasm) {
  $relations = chado_query("
    SELECT rel.subject_id, rel.type_id, cvterm.name as type, rel.object_id 
    FROM {stock_relationship} AS rel LEFT JOIN {cvterm} ON cvterm_id = rel.type_id
    WHERE rel.subject_id = :germplasm OR rel.object_id = :germplasm
  ", array(':germplasm' => $germplasm));

  return ($relations->rowCount() > 0) ? $relations->fetchAll() : 0;
}

/**
 * Get relationship types.
 * 
 * @return array
 *   Term id as key and cvterm name (relationship type) as the value.
 */
function helium_exporter_get_relationship_types() {  
  // Get relationships used in stock table.
  $relations = array();

  $sql = "
    SELECT sr.type_id, cvt.name AS type_name
    FROM {stock_relationship} sr LEFT JOIN {cvterm} cvt ON cvt.cvterm_id = sr.type_id
    GROUP BY sr.type_id, cvt.name ORDER BY COUNT(sr.*) DESC
  ";
  
  $rel_query = chado_query($sql);

  if ($rel_query->rowCount() > 0) {
    foreach ($rel_query as $r) {
      $relations[ $r->type_id ] = $r->type_name;
    }
  }

  return $relations;
}

/**
 * Get configuration settings for subject and object.
 */
function helium_exporter_get_configuration() {
  $relations = variable_get('helium_exporter_pedigree_relations');
  $relations = unserialize($relations);

  return $relations;
}

/**
 * Get phenotypes.
 */
function helium_exporter_get_phenotypes($experiment, $traits, $germplasm) {
  // Add these two traits into the official traits selected by user.
  $arr_notso_trait = array('Planting Date (date)', '# of Seeds Planted (count)');
  $more_trait = chado_query("
    SELECT cvterm_id FROM {cvterm} RIGHT JOIN pheno_project_cvterm USING(cvterm_id)
    WHERE name IN (:not_so_trait) AND project_id = :experiment
  ", array(':not_so_trait' => $arr_notso_trait, ':experiment' => $experiment))
  ->fetchCol();
   
  $trait = array_merge($traits, $more_trait);
  
  $filter_sql = " (
    SELECT plant_id FROM {pheno_plant} WHERE stock_id IN (:germplasm) AND plant_id IN (
      SELECT plant_id FROM pheno_plant_project WHERE project_id = :experiment
    )
  ) ";
  $filter_args = array(':germplasm' => $germplasm, ':experiment' => $experiment);

  // Prepare headers.
  $filter_args[':traits'] = $traits;
  $tmp_headers = chado_query("
    SELECT 'C' || t2.cvterm_id AS id, t2.name
    FROM {pheno_measurements} t1 LEFT JOIN chado.cvterm t2 ON t2.cvterm_id=t1.type_id
    WHERE t2.cvterm_id IN (:traits) AND t1.plant_id IN" . $filter_sql . "
    GROUP BY t2.cvterm_id, t2.definition
  ", $filter_args);

  $headers = array('A0' => 'LineName');

  foreach($tmp_headers as $header) {
    $headers[ $header->id ] = $header->name;
  }
  
  ksort($headers);

  // Fill the data into the correct column header, already sorted.
  $phenotypes_sql = "
    SELECT t2.plant_id AS id, '0' AS tid, 'Name' AS def, t1.name AS value, 'A' AS grp
    FROM {stock} AS t1 INNER JOIN pheno_plant AS t2 USING(stock_id)
    WHERE t2.plant_id IN" . $filter_sql

    // Finally we add in all the traits.
    . "UNION
    SELECT t1.plant_id AS id, t1.type_id as tid, t2.name AS def, ARRAY_TO_STRING(ARRAY_AGG(DISTINCT t1.value), '<m_value>') AS value, 'C' as grp
    FROM pheno_measurements AS t1 INNER JOIN {cvterm} AS t2 ON t1.type_id = t2.cvterm_id
    WHERE t2.cvterm_id IN (:traits) AND t1.plant_id IN" . $filter_sql

    // Lastly we order the results by plant_id and grouping string, and tid.
    . "GROUP BY t1.plant_id, t1.type_id, t2.name
    ORDER BY id, grp, tid ASC
  ";

  $phenotypes = chado_query($phenotypes_sql, $filter_args);
  $data = [];

  if ($phenotypes->rowCount() > 0) {
    $rows = array();
    foreach($phenotypes as $phenotype) {
      $rows[ $phenotype->id ][ $phenotype->grp . $phenotype->tid ] = $phenotype->value;
    }
  
    foreach($rows as $row) {
      foreach($headers as $id => $col) {
        if (!isset($row[ $id ]) || $row[ $id ] == 'NA') {
          $row[ $id ] = '';
        }
        else {
          $v = explode('<m_value>', $row[ $id ]);
          if (count($v) >= 2) {
            $row[ $id ] = str_replace('<m_value>', ',', $row[ $id ]);
          }
        }
      }
            
      ksort($row);
      $data[] = $row;
    }
  }

  return array('headers' => $headers, 'rows' => $data);
}