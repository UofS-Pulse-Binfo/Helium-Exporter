<?php
/**
 * @file
 * Master template file of data exporter for Helium.
 */
?>

<div id="helium-exporter-contariner">
  <p>
    Helium is a generic platform in which various data types can be shown in a pedigree context.&nbsp;&nbsp;<a id="helium-exporter-show" href="#">Show me how</a>
    <a href="<?php print $helium_resource['download']; ?>" target="_blank">Download Helium</a> | <a href="<?php print $helium_resource['information']; ?>" target="_blank">More Information</a>
  </p>

  <div id="helium-exporter-show-window">
    <img id="helium-exporter-diagram" src="<?php print $path_module . '/theme/images/application-install-steps.gif'; ?>" alt="How to visualize data using helium" title="How to visualize data using helium" />
    <div id="helium-exporter-show-steps">
      <div class="helium-exporter-show-steps-item">
        <div>
          <strong>Step #1:</strong>
          <p>
            Download and install Helium application in your computer.
            <a href="<?php print $helium_resource['download']; ?>">&#xbb; How to install Helium</a>
          </p>
        </div>
      </div>
      <div class="helium-exporter-show-steps-item">
        <div>
          <strong>Step #2:</strong>
          <p>
            Use the built-in data exporter below to generate a data file.
            <a href="<?php print $helium_resource['download']; ?>">&#xbb; How to download data</a>
          </p>
        </div>
      </div>
      <div class="helium-exporter-show-steps-item">
        <div>  
          <strong>Step #3:</strong>
          <p>
            Load the data file generated in #2 into Helium to visualize.
           <a href="<?php print $helium_resource['download']; ?>">&#xbb; How to load data into Helium</a>
          </p>
        </div>
      </div>
      <div class="helium-exporter-clear-float"></div>
    </div>
    <div class="helium-exporter-float-right"><a id="helium-exporter-hide" href="#">Okay, Got It!</a></div>
    <div class="helium-exporter-clear-float"></div>
  </div>

  <div id="helium-exporter-download-form">
    <?php print drupal_render($form['download_data']); ?>
  </div>
</div>













