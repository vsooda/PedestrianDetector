Pedestrian Detector
===================

This project is a port of Piotr Dollar's Image & Video Matlab Toolbox found [here](https://github.com/pdollar/toolbox/ "Piotr Dollar's Matlab Toolbox").

If you need more information please feel free to contact me.
Bare in mind that this may be outdated regarding Piotr Dollar's work.

Requirements:
* OpenCV
* CMake

Work made for [IST - Técnico Lisboa](http://tecnico.ulisboa.pt/en/ "Técnico Lisboa")

usage:
./build/peddetector data/Ze.png

CHANGELOG:

30/04/2013
Fabio R.
This is still an prototype. It is implemented with a cmake, but it is standalone
 regarding the HDA project cmake system.

31/07/2013
Matteo T. & Fabio R.
Comparing with the matlab code in:
 ~/PhD/my_code/SoftCascade/6.1.BaseInitialNegativesAsInBootstrap/Data/

12/08/2013
Fabio R.
Ported the code over to ROS, maintaining the native part in the same project.

20/02/2016
sooda
clean project for video pedestrian detect
