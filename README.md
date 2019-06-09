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
- SYM = Ein Funktionszeiger

## Register

TBD

## Stack und Stack-Frames

Ein Stack wird für das Speichern und Auslesen von Werten genutzt.

Bevor Werte abgelegt werden können, muss zuvor mittels dem STK Opcode 
Speicher reserviert werden.

## Opcodes

### NOP

Erhöht den EP und macht sonst nichts weiter.

### STK num

Initialisiert einen neuen Stack mit `num` möglichen Werten.

### SET vid, num|str|sym

Speichert den übergebenen Wert in der Variable `vid`.

### INI sym|vid, num

Initialisiert einen Funktionsaufruf mit `num` Argumenten.

### SND vid|num|sym|str

Sendet einen Wert an den Funktionsaufruf.

### EXC

Führt den Funktionsaufruf aus.

### DEL vid

Löscht den Wert der Variable `vid`

### END

Beendet die Ausführung.

## Beispiel hello-world

```
STK   1                 ; 1 Variable auf dem Stack reservieren
SET   0, "hallo welt"   ; "hallo welt" in Variable 0 speichern
INI   puts, 1           ; Funktionsaufruf zu "putc" mit 1 Argument initialisieren
SND   0                 ; Variable 0 an Funktion übergeben
EXC                     ; Funktion aufrufen
END                     ; Program beenden
```
