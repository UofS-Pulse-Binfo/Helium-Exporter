![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
[![Run PHPUnit Tests](https://github.com/UofS-Pulse-Binfo/helium_exporter/actions/workflows/phpunit.yml/badge.svg)](https://github.com/UofS-Pulse-Binfo/helium_exporter/actions/workflows/phpunit.yml)

# Helium-Exporter

![banner](https://user-images.githubusercontent.com/15472253/154712154-3d3388bc-9f87-4968-992a-7b647e9c0522.png)

Helium Exporter is a custom Tripal data exporter that will download germplasm and phenotypic data into a file format that is fully compatible with the [Helium Pedigree Visualization Framework](https://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-15-259).

## Dependencies

1. [Drupal 7](https://www.drupal.org/)
2. [Tripal 3.x](http://tripal.info/)
3. [Raw Phenotypes Module](https://github.com/UofS-Pulse-Binfo/rawphenotypes)

**Note: For phenotypic data, this module assumes you have used the raw phenotypes module linked to above. This means that the data is not stored in Chado but in a custom schema within Drupal. If you are interested in this module but have phenotypes stored differently, let us know as we're happy to work with you to support other storage options!**

## Installation
### Exporter (this module)
1. Install all dependencies. Please note to also install all the dependencies for the Raw Phenotypes module as well.
2. Install and enable this module as you would any other Drupal Module.
3. Configure module. Link to configuration page is displayed next to the enabled module in the modules page.

### Helium

Please use the [Helium Docs](https://github.com/cardinalb/helium-docs/wiki/Download-Helium) to install Helium in your local computer.

## Export and Visualize Pedigree
![steps to visualize](https://user-images.githubusercontent.com/15472253/154708936-cc6e2705-86e2-4dcc-bf5e-54756a82cb4e.png)

## Funding

This work is supported by Saskatchewan Pulse Growers [grant: BRE1516, BRE0601], Western Grains Research Foundation, Genome Canada [grant: 8302, 16302], Government of Saskatchewan [grant: 20150331], and the University of Saskatchewan.

## Citation

Reynold L. Tan (2022). Helium Exporter: Visualizing large-scale phenotypic data and germplasm pedigrees in Tripal using Helium. University of Saskatchewan, Pulse Crop Research Group, Saskatoon, SK, Canada.

This module relies on Tripal and Helium whose citations are below:

- Spoor S, Cheng CH, Sanderson LA, Condon B, Almsaeed A, Chen M,
Bretaudeau A, Rasche H, Jung S, Main D, Bett K, Staton M, Wegrzyn JL,
Feltus FA, Ficklin SP. Tripal v3: an ontology-based toolkit for construction of FAIR biological community databases.Database. July 2019, 2019: baz077, https://doi.org/10.1093/database/baz077
- Shaw, P.D., Graham, M., Kennedy, J. et al. Helium: visualization of large scale plant pedigrees. BMC Bioinformatics 15, 259 (2014). https://doi.org/10.1186/1471-2105-15-259
