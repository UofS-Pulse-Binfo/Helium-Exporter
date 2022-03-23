<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Tests\DatabaseSeeders\RawphenotypeSeeder;

class HeliumExporterTest extends TripalTestCase {
  use DBTransaction;
  
  /**
   * Test install, construct pedigree information and fetch categorical data.
   */
  public function testHelium() {  
    // INSTALL:
    
    // Dependencies: rawphenotypes.
    $is_enabled = module_exists('rawpheno');
    $this->assertTrue($is_enabled, 'Module rewpheno is enabled');
    
    // Dependencies: tripal download API
    $is_enabled = module_exists('trpdownload_api');
    $this->assertTrue($is_enabled, 'Module trpdownload API is enabled');

    // Module.
    $is_enabled = module_exists('helium_exporter');
    $this->assertTrue($is_enabled, 'Module enabled');
    
    // Terms and configuration values:
    $default_rels = variable_get('helium_exporter_pedigree_relations');
    $default_rels = unserialize($default_rels);
    
    foreach($default_rels as $term => $config) {
      if ($term == 'maternal' || $term == 'paternal') {
        $this->assertIsInt((int) $config, "Configuration $term is integer");          
      }
      else {
        $this->assertIsArray($config, "Configuration $term is Array");
      }
    }
    
    // PEDIGREE AND CATEGORICAL DATA:
    //
    $seeder = new RawphenotypeSeeder();
    $data = $seeder->up();

    if ($data) {
      // EXPERIMENTS - get germplasm and traits used.

      // Load experiment active in raw phenotypes for user id #1.
      global $user;
      $user->uid = 1;
      
      $experiment = helium_exporter_get_experiments();
      $experiment_name = array_values($experiment)[0];
      $this->assertEquals($data['experiment']['name'], $experiment_name, 
        'Test experiment is not equal to returned experiment.');
      
      // Get experiment assets - germplasm and traits as setup in rawphenotypes.
      $experiment_id = array_keys($experiment)[0];
      $experiment_asset = helium_exporter_get_experiment_assets($experiment_id);

      // Assert the asset belongs to the test experiment.
      $this->assertEquals($experiment_name, $experiment_asset['experiment'],
        'Test experiment is not equal to returned experiment asset.');
    
      // Assert the stocks used in experiment matched test data.
      $this->assertEquals($data['germplasm'][0], array_values($experiment_asset['germplasm'])[0],
        'Test germplasm is not equal to returned germplasm of experiment asset.');
      
      // Assert the stocks used in experiment matched test data.
      $this->assertEquals($data['trait']['name'], $experiment_asset['traits'][0]['name'],
        'Test trait is not equal to returned trait of experiment asset.');
     
        
      // PEDIGREE:

      // FULL PEDIGREE - all germplasm relationships.
      // Test create a pedigree: Full (all germplasm relationship).
      // THIS IS Stock 1.
      $germplasm_id = array_keys($experiment_asset['germplasm'])[0];
      $pedigree = helium_exporter_get_parents($germplasm_id, 0);
      
      // Relationship created by seeder.
      // 1. Stock 1 is selection of Stock 2.
      // 2. Stock 2 is registered cultivar of 3
      // 3. Stock 3 is selection of Stock 4
      // 4. Stock 5 is maternal parent of 4 
      // 5. Stock 6 is paternal parent of 4.
      
      // Assert there are 5 item (relations) in the array returned.
      $this->assertEquals(count($data['stock_relations']), count($pedigree),
        '5 Relationships is not equal to number of relationships of the returned value.');
      
      // Assert if each relations match relations setup by seeder.
      $match = 0;
      foreach ($pedigree as $relation) {
        $rel = implode(' ', $relation);
        foreach ($data['stock_relations'] as $test_relation) {
          $test_rel = implode(' ', $test_relation);
          if ($rel == $test_rel) {
            $match++;
          }
        }
      }  

      $this->assertEquals(count($data['stock_relations']), $match,
        'Relationships is not equal to the relationships of the returned value.');
      
      // PARENTAL RELATIONSHIPS ONLY - maternal and paternal parents only.
      // 1. Stock 5 is maternal parent of 4 
      // 2. Stock 6 is paternal parent of 4.
      $pedigree = helium_exporter_get_parents($germplasm_id, 1);

      // Assert there are 5 item (relations) in the array returned.
      $this->assertEquals(count($data['stock_parentage']), count($pedigree),
        '2 Relationships is not equal to number of relationships of the returned value.');

      // Assert if each relations match relations setup by seeder.
      $match = 0;
      foreach ($pedigree as $relation) {
        $rel = implode(' ', $relation);
        foreach ($data['stock_parentage'] as $test_relation) {
          $test_rel = implode(' ', $test_relation);
          if ($rel == $test_rel) {
            $match++;
          }
        }
      }  

      $this->assertEquals(count($data['stock_parentage']), $match,
        'Relationships is not equal to the relationships of the returned value.');

      
      // CATEGORICAL DATA:
      // This is Trait 1 (cm):
      $traits = array($data['traits'][0]['cvterm_id']);
      $categorical = helium_exporter_get_phenotypes($experiment_id, $traits, $germplasm_id);
      
      // Assert single phenotype and value is 123.
      $this->assertEquals(1, count($categorical['rows']),
        'The number of phenotypes is not equal to the returned categorical data.');
      
      $this->assertEquals('123', $categorical['rows'][0]['C' . $traits[0]],
        'Phenotype value 123 is not equal to the returned categorical data');
    }
  }
}
