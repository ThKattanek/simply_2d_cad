# Simply 2D CAD – Feature-Dokumentation

**Simply 2D CAD** ist eine 2D-CAD-Anwendung, die auf **C++17** und **Qt 6 (Widgets/GraphicsView)** basiert. Sie bietet eine interaktive Zeichenoberfläche mit echten Weltkoordinaten, vielseitigen Fanggrenzen (Snapping), Hilfskonstruktionen und Befehlseingaben.

---
## Screenshots  
![first_screenshot_of_the_main_window](doc/images/screenshot_of_the_main_window.png)
---

## 1. Unterstützte Geometrie-Objekte (Entities)

Die Anwendung speichert und verwaltet geometrische Objekte nativ im `CadDocument`:

* **Punkte (`CadPoint`):** Einzelne Koordinatenpunkte mit maßstabsunabhängiger Darstellung.
* **Linien (`CadLine`):** Strenge 2D-Segmentlinien zwischen zwei Koordinaten.
* **Kreise (`CadCircle`):** Kreise mit Zentrum und Radius.
* **Hilfslinien / Konstruktionslinien:**
  * **Unendliche horizontale/vertikale Hilfslinien (`CadConstructionHvLine`)**
  * **2-Punkt-Hilfslinien (`CadConstructionLine`):** Unendliche Geraden durch zwei definierte Punkte.

---

## 2. Zeichen- & Konstruktionswerkzeuge (Tools)

Die Zeichenwerkzeuge werden über den `CadToolManager` gesteuert und bieten interaktive Live-Vorschauen sowie Befehlszeilen-Unterstützung:

* **Auswahl-Werkzeug (`SelectTool`):** Zum Auswählen von Elementen in der Szene.
* **Punkt (`PointTool`):** Setzt Punkte per Mausklick oder Koordinateneingabe.
* **Linienwerkzeuge (`LineTool`):**
  * **Standard-Linie:** Zeichnen über Start- und Endpunkt.
  * **Horizontale & Vertikale Linien (`HLine` / `VLine`):** Ausrichtung an Achsen.
  * **Polyline:** Fortlaufendes Zeichnen verketteter Liniensegmente.
  * **Parallele Linie (`ParallelLineTool`):** Erstellt Parallelen zu bestehenden Geometrien/Achsen mit Abstandseingabe.
* **Kreiswerkzeuge (`CircleTool`):**
  * **Zentrum – Radius:** Kreis über Mittelpunkt und Außenpunkt/Radius.
  * **Zentrum – Durchmesser:** Kreis über Mittelpunkt und Durchmesser.
  * **2-Punkte-Durchmesser:** Kreis über zwei gegenüberliegende Punkte auf dem Umfang.
* **Rechteck (`RectangleTool`):** Erstellt 4 verknüpfte Linien über zwei Eckpunkte.
* **Konstruktionswerkzeuge:**
  * **H/V Hilfslinie (`ConstructionHvLineTool`):** Horizontale oder vertikale Hilfslinie an einer Koordinate.
  * **2-Punkt-Hilfslinie (`ConstructionLineTool`):** Hilfslinie durch zwei Punkte.
  * **Parallele Hilfslinie (`ParallelConstructionLineTool`):** Erzeugt eine oder mehrere parallele Hilfslinien zu bestehenden Geometrien/Achsen.
  * **Rechtwinklige Hilfslinie (`PerpendicularConstructionLineTool`):** Erzeugt eine 90°-Hilfslinie an einem Element.
  * **Hilfslinien löschen:** Ein-Klick-Löschung aller Hilfslinien im Dokument via Makro-Befehl.

---

## 3. Fang-System (Object Snapping)

Das `SnapManager`-Modul berechnet in Echtzeit Fangpunkte basierend auf der Bildschirm-Toleranz und dem Zoomfaktor:

* **Endpunkt (`Endpoint`):** Start- und Endpunkte von Linien.
* **Mittelpunkt (`Midpoint`):** Mitte von Linien und Mittelpunkt von Kreisen.
* **Punkt (`Point`):** Einzelne Punkte (`CadPoint`).
* **Schnittpunkt (`Intersection`):** Berechnet reale Schnittpunkte zwischen:
  * Linie–Linie
  * Linie–Hilfslinie
  * Hilfslinie–Hilfslinie
  * Linie/Hilfslinie–Kreis
  * Kreis–Kreis
* **Tangente (`Tangent`):** Tangentiale Anbindung an Kreise oder rechtwinklige Fußpunkte auf Hilfslinien.
* **Lot / Senkrecht (`Perpendicular`):** Lotfußpunkte auf Segmentlinien.
* **Visuelle Marker:** Eigene maßstabsunabhängige Vektor-Marker für jeden Fang-Typ.

---

## 4. Befehlszeile & Koordinateneingabe

Unterhalb der Zeichenfläche befindet sich eine interaktive Befehlszeile (`QLineEdit` & `QLabel`), die Tastatureingaben direkt verarbeitet:

* **Kartesische Koordinaten:** Eingabe von `X,Y` (z. B. `10,20`).
* **Relativkoordinaten:** Eingabe mit `@` (z. B. `@50,0` für 50 Einheiten nach rechts vom letzten Punkt).
* **Polarkoordinaten:** Eingabe von `Länge<Winkel` (z. B. `100<45`).
* **Direkte Werte:** Eingabe einzelner Zahlen z. B. für Radien, Durchmeser oder Parallelenabstände.
* **Dynamisches Tippen:** Buchstabeneingaben auf der Zeichenfläche fokussieren automatisch die Befehlszeile.

---

## 5. Grafik-Engine & Viewport (Qt GraphicsView)

* **CAD-Koordinatensystem:** Y-Achse ist klassisch nach oben ausgerichtet (invertierter Qt-Standard).
* **Navigation:**
  * **Pan:** Mittlere Maustaste gedrückt halten.
  * **Zoom:** Rechte Maustaste ziehen oder Mausrad (Zoom um den Mauszeiger).
  * **Zoom to Fit:** Automatische Einpassung der gesamten Geometrie ins Bild beim Laden.
* **Fadenkreuz (`CrosshairItem`):** Dynamisches Fadenkreuz, das die Mausposition abbildet und maßstabsunabhängig bleibt.
* **Achsenkreuz:** Rot gestrichelte Hauptachsen für den Ursprung `(0,0)`.

---

## 6. Datei-Formate & Schnittstellen

* **Natives Dateiformat (`.s2dcad`):**
  * Binäres Dateiformat mit Header (`Magic Number`), Versionsprüfung und Stream-Serialisierung (`QDataStream`).
* **DXF Import & Export:**
  * Integration der `dxflib`-Bibliothek (AutoCAD 2000 / AC1015 Format).
  * Unterstützt den Import/Export von Punkten, Linien und Kreisen.

---

## 7. System-Features & Benutzeroberfläche

* **Undo / Redo-System (`UndoStack`):**
  * Auf dem Command-Pattern aufgebaut (`AddEntityCommand`, `RemoveEntityCommand`, `MacroCommand`).
  * Vollständige Rückgängig-Machen- und Wiederholen-Funktion für Zeichenaktionen.
  * Erkennung von ungespeicherten Änderungen (`isClean`) mit Hinweis-Dialog beim Schließen.
* **Mehrsprachigkeit (Internationalisierung):**
  * Dynamisches Umschalten der UI-Sprache zur Laufzeit (Deutsch, Englisch vorbereitet).
  * Automatische Erkennung installierter `.qm`-Übersetzungsdateien.
* **Layout & Einstellungen (`AppSettingsDialog`):**
  * Speicherung der Fenstergeometrie, Toolbar-Positionen und Einstellungen über `QSettings` (INI-Format).
  * Konfigurierbare Parameter: Standard-Speicherpfad, Fanggrenzen-Toleranz (Pixel) und Fangmarker-Größe.
```
---
## Build
### Update all translation files (*.ts)
Update *.ts files with: cmake --build [BUILD_DIRECTORY] --target simply_2d_cad_lupdate  

## License

This project is licensed under the GNU General Public License version 2 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

*   A very special thanks to **The Qt Company** for developing and maintaining the excellent **Qt Framework**. Their powerful platform made developing this CAD application a smooth and enjoyable experience.
*   Thanks to **RibbonSoft** for providing the **dxflib**, enabling standard-compliant DXF support.
*   Thanks to the open-source community for countless libraries, tutorials, and inspiration.

eof
