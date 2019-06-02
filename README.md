# Coding Board Contest #2
## Verkompliziertes Hello-World Programm

Meist nutzt man "Hello, World!" bei den ersten Gehversuchen in einer neuen 
Programmiersprache oder Entwicklungsumgebung.

Daher gibt es in meiner Interpretation ein ziemlich rudimentäres Hello-World Programm, 
aber dafür mit einer eigenen virtuellen Maschine und einem Assembler-ähnlichen Dialekt.

Wenn noch Zeit bleibt, dann kommt auch noch ein Compiler dazu :)

## Werte

Aktuell sind folgende Werte möglich:

- NUM = 64bit Ganzzahl mit Vorzeichen
- STR = eine Zeichenkette
- NIL = kein Wert
- FNC = Ein Funktionszeiger

## Register

Die virtuelle Maschine besitzt 2 Register für temporäre Werte.

R0 und R1

Zudem existieren noch EP (execution pointer) und SP (stack pointer).

## Stack und Stack-Frames

TBD

## Opcodes

### NOP

Erhöht den EP und macht sonst nichts weiter.

### MOV dst, src

Kopiert den Inhalt aus `src` nach `dst`.
Sollte `dst` bereits einen Wert haben (nicht NIL), wird dieser bereinigt.

### VRT sym

Ruft eine für die virtuelle Maschine exportierte Funktion auf.

### END

Beendet die Ausführung.
