Tuza József és Szabó Attila 4. védési anyaga Magas szintű programozási nyelvek 2. tárgyból
============================================

A repo tartalma:

-Malvin:
Az argumentumként megadott térkép (.osm) összes buszjáratának megállóit írja ki.
A project C++-ban készült.

-Gedeon:
A Malvin feladat megvalósítása Javában.
A program Maven projectben található.

-Koppány:
A Malvin feladathoz írt, OSM alapú megjelenítő, JXMapViewer2 használatával.
A program forrása Java nyelven íródott, Maven projectben.

-Petra:
A program a standard bemenetről kapott koordinátákból, egy utat keres meg az argumentumként megadott térképen (.osm).
A program először meghatározza a kereszteződéseket, majd a beadott koordinátákhoz megkeresi a legközelebbi kereszteződéseket. Miután a meghatározás megvan, felállít egy utat a két pont között, úgy hogy ugrál kereszteződésről, kereszteződésre. Mindig arra ugrik, amerre a legközelebb ér a végponthoz.
Miután az útkeresés sikeresen lezárult, visszafejti az utak koordinátáit, és ezt írja ki a standard kimenetre.
A program C++-ban készült, CMake projectként.

-Brúnó:
OSM alapú megjelenítő a Petra feladathoz, Javában, Maven projectként, JXMapViewer2 használatával.

-Megjelenítő:
Saját megjelenítő OpenGL API segítségével. A GLUT (OpenGL Utility Toolkit) kezelését a FreeGLUT végzi.
A program argumentumként megadott térképet (.osm) kirajzolja a képernyőre: az Osmium könyvtár segítségével megtörténik a beolvasása a node-oknak és a way-eknek, majd meghatározzuk hogy hol is helyezkedjen el majd a térkép, a képernyőn, ha ez megtörtént, akkor az FreeGLUT segítségével betöltjük az ablakot, majd megjelenítjük a képet.
Az alkalmazásban lehet mozogni a bal egérgombbal és zoomolni az egér görgőjének segítségével.
A program C++-ban készült, CMake projectként.

-----------------------
Licenszek:
osmium: GPLv3
JXMapViewer2: GPLv3
FreeGLUT: X-Consortium
