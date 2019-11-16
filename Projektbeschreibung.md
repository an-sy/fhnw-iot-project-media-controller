# Projektbeschreibung

## Ausgangslage
Für einen Mitbewohner im Seniorenalter soll eine vereinfachte Bedienung seiner TV/Audioanlage konstruiert werden. Der Mitbewohner hat ein Swisscom-TV Abo und verfügt über eine Sonos Audioanlage. 
Die jetzige Bedienung ist nicht optimal, da er für die einzelnen Komponenten jeweils eine separate Fernbedienung in die Hand nehmen muss.
-	TV (LG Fernseher) mit Fernbedienung einschalten
-	Swisscom TV mit Fernbedienung aktivieren und Programm wählen
-	Mit Sonos App (Mobilephone) die Lautstärke anpassen

## Lösungsidee (Reference model)
![reference model](/01_Usecase_and_reference_model/01_usecase_referencemodel_draft1.png)


### Sonos Anlage:

Die Einrichtung der Sonosgeräte erfolgt nur über deren App. Alle Geräte müssen im selben Netz integriert sein. Die Geräte besitzen Funktionstasten, aber die Bedienung ist so konzipiert, dass die Einstellungen über die App erfolgen sollten. Da die Geräte bereits installiert sind brauchen wir eine API, die die Kommunikation mit der Anlage ermöglicht.

### Sonos API:

Auf der Suche sind wir auf folgende Sonos HTTP Api gestossen.  
https://jishi.github.io/node-sonos-http-api/

Diese node.js basierte Api wird auf unseren raspberry pi implementiert. Für unseren Zweck ist diese Variante sehr optimal.

Die Befehle für die Lautstärkeanpassung können über ein Arduino Modul (Feather Huzza) absetzen.





