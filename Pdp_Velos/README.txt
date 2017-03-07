# VisualBSS

VisualBikeSharingSystem - version 0.1 - 08/03/2017

Auteurs:
Damien BIELAWSKI - Sébastien BIELAWSKI - Alassane Diop - Alaric Braillon

Le programme permet de visualiser des trajets de vélos en libre service à
partir de données stockées dans des fichiers (de type CSV).
Il affiche les trajets et les stations à partir d'un fichier chargé.


utilise Qt 5.8
compilé avec GCC 4.9.2
executé sur Debian 8.7
requiert OpenGL 3.3 ou sup

Pour compiler:
	dans le repertoire Pdp_Velos:
		-qmake Pdp_Velos.pro
		-make

Usage:
	./Pdp_Velos <NOM DU FICHIER.csv>
