# Gebruiksaanwijzing

### opladen / vervangen batterijen

De line follower robot gebruikt 2 18650 3,7V Li-ion batterijen, die kunnen worden opgeladen met een externe compatiebele lader.
Als de batterijen plat zijn kunnen deze uit de batterijhouder van de robot gehaald worden en vervangen door 2 volle batterijen.
Schakel de hoofdschakalar steeds uit als je de batterijen vervangt.

### draadloze communicatie

De draadloze communicatie verloopt via bluetooth.

#### verbinding maken

Zet de bluetooth van je gsm aan en verbind met de auto. 
Open de bluetooth serial terminal app.
Verbind in deze app met de robot door rechts bovenaan op het connect symbool te drukken. 
Je kan nu via het tekstvak commando's versturen naar de auto. In deze app kan je ook commando's sturen via zelf ingestelde sneltoetsen.

#### commando's

- help --> geeft het overzicht van de  beschikbare commando's
- debug --> geeft de huidige instellingen weer 
- run aan --> start de robot
- run uit --> stopt de robot
- set cycle [µs] --> instellen van de cyclustijd (deze moet altijd 1.5 tot 2x groter zijn dan de calculation time)
- set power [0..255] --> bepaalt de snelheid van de auto
- set diff [0..1] --> bepaald of de auto versneld (> 0.5) of vertraagd (< 0.5) in de bochten.
- set kp [0..] --> bepaald de kp waarde van de pid regelaar
- set ki [0..] --> bepaald de ki waarde van de pid regelaar
- set kd [0..] --> bepaald de kd waarde van de pid regelaar
- calibrate black --> de sensoren worden gekalibreerd op de zwartwaarden
- calibrate white --> de sensoren worden gekalibreerd op de witwaarden

### kalibratie

Het kalibreren van de sensoren gebeurd door alle 8 sensoren van de QTR-8A sensor over een zwart (afgebeeld op het parcour) of het witte gedeelte van het parcour te plaatsen, 
en het respectievelijke commando "calibrate black" of "calibrate white" uit te voeren.

### settings
De robot rijdt stabiel met volgende parameters:  

power: 220\
diff: 0.4\
kp: 18\
ki: 0\
kd: 0.2\
cycle: 1.5 tot 2x groter zijn dan de calculation time

### start/stop button

De start/stop button is 1 knop waarmee je de robot start en stopt, analoog met de commando's "run aan" en "run uit"  via de blue tooth app.
