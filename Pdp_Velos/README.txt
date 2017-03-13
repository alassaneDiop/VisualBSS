# VisualBSS

VisualBikeSharingSystem - version 0.1 - 11/03/2017

===============================================================================
Authors:
Damien BIELAWSKI - Sébastien BIELAWSKI - Alassane Diop - Alaric Braillon

Le programme permet de visualiser des trajets de vélos en libre service à
partir de données stockées dans des fichiers (de type CSV).
Il affiche les trajets et les stations à partir d'un fichier chargé.

===============================================================================
Requierement:
	-Qt 5.6 or sup
	-OpenGL 3.3 ou sup
	-C++14 compiler

===============================================================================
Compilation:
	In Pdp_Velos repository:
		-mkdir build && cd build
		-qmake ../Pdp_Velos.pro
		-make

===============================================================================
Execution:
	./Pdp_Velos <DATA-FILE-NAME.csv>

===============================================================================
Bugs:

-Trips and stations in the map widget are not shown correctly in the center of
the widget, due a zoom calculation issue.

-Stations/Trips and Glyphs cannot be displayed in the same time. There is a
VAO/VBO issue.

