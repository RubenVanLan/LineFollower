# Instructable

### stap 1
bestel alle componenten uit de bill of materials  

### Stap 2: Chassis Montage
- Soldeer de female header pinns voor de ESP32 op de onderkant van de printplaat
- Soldeer de male header pins aan QTR8-A en DRV-8833
- Soldeer de female header pins voor de QTR8-A en DRV-8833 aan de printplaat
- Soldeer de drukknoppen, condensatoren, weerstanden, en Voltregulator op de printplaat alvorens je de batterijhouder erop soldeert
- Soldeer de batterijhouder op het chassis.
- Bevestig de motoren op het chassis met schroeven door middel van een 3D-print houder
- Soldeer aan de motoren dupont draadjes
- Monteer de wielen op de motorassen
- Monteer de lijnvolgsensoren aan de voorkant van het chassis en plak het oogje achter de sensoren zodat deze niet op het papier hangen
- Bevestig de motor driver module en ESP32 module op het chassis 

### Stap 3: Elektrische Verbindingen
#### Motor driver module:
Verbind de Dupont draadjes met de female header pinns gesoldeerd op J1-MotorL en J2-MotorR.

#### Lijnvolgsensoren:
- verbind de 3.3V van de ESP aan Vcc van de sesor
- verbind dr GND
- Maak een loop op de 3.3V bypass van de senor

### Stap 4: Software Installatie

#### ESP32 configureren in Arduino IDE:
- Open de Arduino IDE.
- Ga naar Tools > Board > Board Manager en installeer de ESP32-bibliotheek.

#### Bibliotheken installeren:
SerialCommand (Steven Cogswell)

#### USB-naar-serieel driver installeren:
USB-naar-serieel driver: Silicon Labs CP210x USB-to-UART driver
Zonder die driver verschijnt de ESP32 niet als COM-poort

#### Code uploaden:
- Sluit de ESP32 aan op de computer via een USB-kabel.
- Selecteer de juiste boardinstellingen:
- Board: ESP32 Dev Module
- Port: (selecteer de juiste COM-poort)
- Upload de code
- Als het uploaden mislukt tijdens de connectie stap, hou dan de boot-knop van de esp32 ingedrukt tijdens het connecteren (net na het compilen)

#### Bluetooth testen:
- Verbind met de ESP32 via Bluetooth vanaf een smartphone en/of computer
- Test de communicatie door gegevens te verzenden en te ontvangen

### Stap 5: stel de parameters in 
Voer de commando's uit. Zie handleiding of raadpleeg het help-menu door het commando help te sturen

### Stap 6: Functionele Test
- Print het parcour
- Plaats de robot op het zwarte vlak en calibreer, plaat de robot op Wit en calibreer
- Plaats de robot op het parcour
- Controleer of de robot de lijn correct volgt, regel indien nodig bij
