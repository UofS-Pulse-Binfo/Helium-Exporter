<?php

namespace Tests\DatabaseSeeders;

use StatonLab\TripalTestSuite\Database\Seeder;

class RawphenotypeSeeder extends Seeder
{
  public function up() {
    $data = array();

    // Create user. This user is authenticated user
    // but does not have any experiments assigned.
    $new_user = array(
      'name' => 'Helium Exporter',
      'pass' => 'secret',
      'mail' => 'helium_exporter@mytripalsite.com',
      'status' => 1,
      'init' => 'Email',
      'roles' => array(
          DRUPAL_AUTHENTICATED_RID => 'authenticated user',
      ),
    );

    // The first parameter is sent blank so a new user is created.
    $user = user_save(new \stdClass(), $new_user);
    $data['user'] = $user->uid;

    // Add cv - stock_relationship.
    $cv = chado_insert_cv('stock_relationship', 'Stock relationship');
    
    // Add cvterms into stock_relationship cv.
    $terms = array(
      'is_donor_backcross_parent_of',
      'is_extracted_from',
      'is_individual_of_population',
      'is_maternal_parent_of',
      'is_paternal_parent_of',
      'is_progeny_of_selfing_of',
      'is_recurrent_backcross_parent_of',
      'is_registered_cultivar_of',
      'is_selection_of',
      'is_synonym_of' 
    );
    
    // Configuration array.
    $relations = array(
      'maternal' => 0,
      'paternal' => 0,
      'object' => array(),
      'subject' => array()
    );

    $is_selection = '';
    $is_cultivar = '';
    foreach($terms as $term) {
      $id = chado_insert_cvterm(
        array(
          'id' =>  'tripal:' . $term,
          'name' => $term,
          'definition' => $term
        )
      );

      if ($term == 'is_maternal_parent_of') {
        $relations['maternal'] = $id->cvterm_id;
        $relations['object'][ $id->cvterm_id ] = $id->name;
      }
      elseif ($term == 'is_paternal_parent_of') {
        $relations['paternal'] = $id->cvterm_id;
        $relations['object'][ $id->cvterm_id ] = $id->name;
      }
      else {
        $relations['subject'][ $id->cvterm_id ] = $id->name;
        if ($term == 'is_selection_of') {
          $is_selection = $id->cvterm_id;
        }
        if ($term == 'is_registered_cultivar_of') {
          $is_cultivar = $id->cvterm_id;
        }
      }
    }

    $data['configuration'] = $relations;

    // Set term as configuration
    variable_set('helium_exporter_pedigree_relations', serialize($relations));

    // Add organism.
    $organism = chado_insert_record('organism', array(
      'abbreviation' => 'T. databasica',
      'genus' => 'Tripalus',
      'species' => 'databasica',
      'common_name' => 'Cultivated Tripal'
    ));

    // Add Stock - accession.
    $accession = chado_query("SELECT cvterm_id FROM {cvterm} WHERE name = 'accession' LIMIT 1")
      ->fetchField();

    $stock1 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 01',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'description Stock 01',
    ));

    $stock2 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 02',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'description Stock 02',
    ));

    $stock3 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 03',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'description Stock 03',
    ));

    $stock4 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 04',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'description Stock 04',
    ));

    $stock5 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 05',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'd escription Stock 05',
    ));

    $stock6 = chado_insert_record('stock', array(
      'organism_id' => $organism['organism_id'],
      'name' => 'Stock 06',
      'uniquename' => 'uname-' . uniqid(),
      'type_id' => $accession,
      'dbxref_id' => null,
      'description' => 'description Stock 06',
    ));

    $data['germplasm'] = array(
      $stock1['name'],
      $stock2['name'],
      $stock3['name'],
      $stock4['name'],
      $stock5['name'],
      $stock6['name']
    );

    // Create relations.
    // Stock 1 is selection of Stock 2.
    // Stock 2 is registered cultivar of 3
    // Stock 3 is selection of Stock 4
    // Stock 5 is maternal parent of 4 
    // Stock 6 is paternal parent of 4.
    
    chado_insert_record('stock_relationship', array(
      'subject_id' => $stock1['stock_id'],
      'object_id' => $stock2['stock_id'],
      'type_id' => $is_selection
    ));

    chado_insert_record('stock_relationship', array(
      'subject_id' => $stock2['stock_id'],
      'object_id' => $stock3['stock_id'],
      'type_id' => $is_cultivar
    ));

    chado_insert_record('stock_relationship', array(
      'subject_id' => $stock3['stock_id'],
      'object_id' => $stock4['stock_id'],
      'type_id' => $is_selection
    ));

    chado_insert_record('stock_relationship', array(
      'subject_id' => $stock5['stock_id'],
      'object_id' => $stock4['stock_id'],
      'type_id' => $relations['maternal']
    ));

    chado_insert_record('stock_relationship', array(
      'subject_id' => $stock6['stock_id'],
      'object_id' => $stock4['stock_id'],
      'type_id' => $relations['paternal']
    ));

    $data['stock_relations'] = array(
      array($stock1['name'], $stock2['name'], $relations['subject'][ $is_selection ]),
      array($stock2['name'], $stock3['name'], $relations['subject'][ $is_cultivar ]),
      array($stock3['name'], $stock4['name'], $relations['subject'][ $is_selection ]),
      array($stock4['name'], $stock5['name'], 'F'),
      array($stock4['name'], $stock6['name'], 'M'),
    );
    
    $data['stock_parentage'] = array(
      array($stock1['name'], $stock5['name'], 'F'),
      array($stock1['name'], $stock6['name'], 'M')
    );

    // Create experiment.
    $experiment = chado_insert_record('project', array(
      'name' => 'Experiment 1',
      'description' => 'Experiment 1'
    ));

    // Add user to experiment.
    db_insert('pheno_project_user')
      ->fields(array(
        'project_id' => $experiment['project_id'],
        'uid' => 1 // Admin id.
      ))
      ->execute();

    $data['experiment'] = array(
      'id' => $experiment['project_id'],
      'name' => $experiment['name']
    );

    // Create traits.
    $term = 'Trait 1 (cm)';
    $trait = chado_insert_cvterm(
      array(
        'id' =>  'rawpheno_tripal:' . $term,
        'name' => $term,
        'definition' => $term
      )
    );

    $data['traits'] = array(
      array('cvterm_id' => $trait->cvterm_id, 'name' => $trait->name)
    );

    // Register experiment to rawphenotypes
   db_insert('pheno_project_cvterm')
    ->fields(array(
      'project_id' => $experiment['project_id'],
      'cvterm_id' => $trait->cvterm_id,
      'type' => 'essential')
    )
    ->execute();

    // Create plant id.
    $pheno_plantid = db_insert('pheno_plant')
      ->fields(array('stock_id' => $stock1['stock_id']))
      ->execute();

    // Map plant id to experiment.
    db_insert('pheno_plant_project')
      ->fields(array('project_id' => $experiment['project_id'], 'plant_id' => $pheno_plantid))
      ->execute();

    // Insert phenotype.
    $unit = chado_query("SELECT cvterm_id FROM {cvterm} WHERE name = 'cm' LIMIT 1")
      ->fetchField();

    db_insert('pheno_measurements')
      ->fields(array('plant_id' => $pheno_plantid,
        'type_id' => $trait->cvterm_id,
        'unit_id' => $unit,
        'cvalue_id' => 123,
        'value' => 123,
        'modified' => date("D M d, Y h:i:s a", time())))
      ->execute();

    return $data;
  }
}