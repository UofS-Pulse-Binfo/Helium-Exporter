<?php
/**
 * @file
 * Master template file of data exporter for Helium.
 */
?>

<div id="helium-exporter-contariner" style="max-width: 900px">
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
        <div id="helium-exporter-bg-line"></div>
        <div id="helium-exporter-show-steps">
          <div class="helium-exporter-show-steps-item">
            <img src="<?php print $path_module . '/theme/images/helium-export-step-01.gif'; ?>" style="border: none" alt="Step 01 : Install" title="Step 01 : Install" />
            <strong>Step #1:</strong>
            <p>
              Download and install Helium viewer application in your computer. Please use the link below and select an operating system.
              <a href="<?php print $helium_resource['download']; ?>">&#xbb; Download and install Helium</a>
            </p>
          </div>

          <div class="helium-exporter-show-steps-item">
            <img src="<?php print $path_module . '/theme/images/helium-export-step-02.gif'; ?>" style="border: none" alt="Step 02 : Export Data" title="Step 02 : Export Data" />
            <strong>Step #2:</strong>
            <p>
              Use the built-in data exporter below to generate a data file. Click the link below for guide on using the exporter.
              <a href="#">&#xbb; How to download data</a>
            </p>
          </div>
          
          <div class="helium-exporter-show-steps-item">
            <img src="<?php print $path_module . '/theme/images/helium-export-step-03.gif'; ?>" style="border: none" alt="Step 03 : Load Data" title="Step 03 : Load Data" />
            <strong>Step #3:</strong>
            <p>
              Load the data file generated in step #2 into Helium to visualize. Click the link below to explore and how to use Helium.
              <a href="#">&#xbb; How to load data into Helium</a>
            </p>
          </div>
          <div class="helium-exporter-clear-float"></div>                
        </div>
      </div>

      <div id="helium-exporter-download-form">        
        <?php 
          if ($is_configured) {
            // Render Helium Exporter Form:
            print drupal_render_children($form); 
          }
          else {
            drupal_set_message('Helium Exporter module is not configured. Please contact administrator.', 'warning');
          }
        ?>
      </div>
    </div>
  </div>

  <div id="helium-exporter-citation">
    Helium: Visualization of Large Scale Plant Pedigrees.<br />
    Shaw, P.D., Kennedy, J., Graham, M., Milne, I. and Marshall, D.F. 2014.<br /> 
    BMC Bioinformatics. 15:259. <a href="https://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-15-259" target="_blank">DOI: 10.1186/1471-2105-15-259</a>
  </div>
</div>













