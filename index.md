## Project goal
This is the main page of software OCTAnnotate that incorporates several research projects (CADOCT and REVEMOD). This application is an open-source software for computer-aided analysis (segmentation, volumetric mearsurements, visualization) of retinal layers and vitreoretinal pathologies (vitreoretinal traction, VMT and epiretinal membrane, ERM) based on optical coherence tomography (OCT) images obtained from Avanti RTvue XR device. This software is written in C++ with Qt libraries.

Currently, the software supports manual annotation of 13 retinal layers (PCV, ERM, ILM, NFL/GCL, GCL/IPL, IPL/INL, INL/OPL, OPL/ONL, ELM, Myoid/Ellipsoid, IS/OS, OS/RPE, RPE/Choroid) in OCT images. An image browser/editor is provided as well as an empty SQLite database for OCT scans. The software supports import of automatic retina layers segmentations from OCTExplorer software.

For more information please refere to the [User's Guide] (https://github.com/krzyk87/OCTAnnotate/blob/master/Users_guide_v1.0.pdf).

![Segmented OCT retina B-Scan](https://github.com/krzyk87/OCTAnnotate/blob/master/images/bscan)
![Example of virtual map of preretinal space](https://github.com/krzyk87/OCTAnnotate/blob/master/images/virtual_map.png)

### Disclaimer
Please note that this project is a "**work-in-progress**", meaning many features still needs to be implemented (see issues for the project source code). The automated segmentation of retina layers, vessels and pathologies is not yet implemented. For now this application is used mainly for investigational purposes - visual analysis of retina layers depth based on existing segmentations (such as those provided by OCTExplorer application). Additionaly, for now the software is compatibly only with Avanti RTvue 3D Retina scans. **If you wish to contribute to the project please contact us - your help will be welcome :)**

### Citing this work
Please pick one of the following papers:
- M. Stopa, P. Rakowicz, E. Marciniak, et al. "*Imaging and measurement of the preretinal space in vitreomacular adhesion and vitreomacular traction by a new spectral domain optical coherence tomography analysis*", Retina, DOI: 10.1097/IAE.0000000000001439, 2016.
- A. Stankiewicz, T. Marciniak, A. Dąbrowski, et al., "*Automatic Modeling and Classification of Vitreomacular Traction Pathology Stages*", 6th International Conference on Image Processing Theory, Tools and Applications, December 2016.
- A. Stankiewicz, T. Marciniak, A. Dąbrowski, et al., "*Matching 3D OCT Retina Images into Super-Resolution Dataset for Detailed 3D Segmentation*", 20th IEEE International Conferene on Signal Processing Algorithms, Architectures, Arrangements, and Applications, pp. 130-137, September 2016.
- A. Stankiewicz, T. Marciniak, A. Dąbrowski, et al., "*Denoising methods for improving automatic segmentation in OCT images of human eye*", Bulletin of the Polish Academy of Sciences, Technical Sciences, vol. 65, no. 1, 2017.
- A. Stankiewicz, T. Marciniak, A. Dąbrowski, et al., "*Improvement of 3D retina layers segmentation based on graph theory approach for low quality OCT images*", Metrology and Measurement Systems, vol. 23, no. 2, pp. 269-280, 2016.
- A. Stankiewicz, T. Marciniak, A. Dąbrowski, et al., "*New OCT-based Method to Generate Virtual Maps of Vitreomacular Interface Pathologies*", 18th IEEE International Conferene on Signal Processing Algorithms, Architectures, Arrangements, and Applications, pp. 83-88, September 2014.

### Licence
This project is published under the **GNU GPL v2 licence** ("a copyleft license that requires anyone who distributes code or a derivative work to make the source available under the same terms"). For full details, see the [LICENCE](https://github.com/krzyk87/OCTAnnotate/blob/master/source/LICENCE.txt) file provided in the project folder.

### Authors, contributors and contact
- Agnieszka Stankiewicz, PhD student & main developer: agnieszka.stankiewicz[at]put.poznan.pl
- Tomasz Marciniak, PhD
- Adam Dąbrowski, Prof.
- Marcin Stopa, MD
- Piotr Rakowicz, MD
- Elżbieta Marciniak, MD

### Other contributors
- Andrzej Michalski, MD
- Jacek Bera, MSc
