# draadloze communicatie proof of concept
minimale hard- en software waarmee aangetoond wordt dat duplex kan gecommuniceerd worden tussen de microcontroller en een [laptop|smartphone] (schappen wat niet past), gebruik makend van [programma] (in te vullen)
<br />
### configuratie
Sluit alles aan volgens het schema en laad de code op in de esp32.
### opmerkingen
Geef een eigen naam aan de ESP32 en verander de naam ESP32_RVL naar je zelf gekozen naam in de code: SerialBT.begin("ESP32_RVL"); //naam geven
### gebruiksaanwijzing
Verbind de esp32 met de USB-poort van je computer en connecteer via bluetooth met je gsm. Open een app (bv. Serial Bluetooth Terminal) waarmee je via bluetooth tekst kan versturen en verbind ook via de app met de esp32.
Nu kan je via bluetooth tekst versturen tussen de seriele monitor op de laptop en je app op je gsm.
