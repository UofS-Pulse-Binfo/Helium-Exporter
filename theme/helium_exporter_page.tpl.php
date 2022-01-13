<?php
/**
 * @file
 * Master template file of data exporter for Helium.
 */
?>

<div id="helium-exporter-contariner">
  <div id="helium-exporter-header">  
    <div id="helium-exporter-title">Data Exporter for Helium</div>
    <div id="helium-exporter-help">
      <a href="https://knowpulse.usask.ca/help" target="_blank" alt="help" title="help"><img src="<?php print $path_module . '/theme/images/help.gif'; ?>" style="border: none;" height="25" width="25"></a>
    </div>
    <div class="helium-exporter-clear-float"></div>
  </div>
  <div id="helium-exporter-copy" class="helium-exporter-copy-page">
    <div class="helium-exporter-content-wrapper">
      <h2>Visualize plant pedigrees and overlay categorical data using Helium:</h2>
      <p>
        Helium is a generic platform in which various data types can be shown in a pedigree context.&nbsp;&nbsp;<a id="helium-exporter-show" href="#">Show me how</a><br />
        <a href="<?php print $helium_resource['download']; ?>" target="_blank">Download Helium</a> | <a href="<?php print $helium_resource['information']; ?>" target="_blank">More Information</a>
      </p>
      
      <div id="helium-exporter-show-window">
        <img id="helium-exporter-diagram" src="<?php print $path_module . '/theme/images/application-install-steps.gif'; ?>" width="900" height="200" style="border: none" alt="How to visualize data using helium" title="How to visualize data using helium" />
        <div id="helium-exporter-show-steps">
         
          <div class="helium-exporter-show-steps-item">
            <div>
              <strong>Step #1:</strong>
              <p>
                Download and install Helium viewer<br />application in your computer.
                <a href="<?php print $helium_resource['download']; ?>">&#xbb; Download and install Helium</a>
              </p>
            </div>
          </div>

          <div class="helium-exporter-show-steps-item">
            <div>
              <strong>Step #2:</strong>
              <p>
                Use the built-in data exporter below</br />to generate a data file.
                <a href="<?php print $helium_resource['download']; ?>">&#xbb; How to download data</a>
              </p>
            </div>
          </div>
          <div class="helium-exporter-show-steps-item">
            <div>
              <strong>Step #3:</strong>
              <p>
                Load the data file generated in step #2<br />into Helium to visualize.
                <a href="<?php print $helium_resource['download']; ?>">&#xbb; How to load data into Helium</a>
              </p>
            </div>
          </div>
          <div class="helium-exporter-clear-float"></div>        
        </div>
      </div>
      <div>
        <?php print drupal_render($form['download_data']); ?>
      </div>
    </div>
  </div>

</div>













