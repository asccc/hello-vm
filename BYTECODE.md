# Bytecode

Ein Opcode mit einem Addressierungs-Argument 
ist 32bits (4 bytes) groß.

Ein Opcode mit zwei Addressierungs-Argumenten 
ist 48bits (6 bytes) groß.

Je nach Anforderung kann ein Opcode mit 
einem Immediate je nach Modus wachsen.

Die Größe eines Opcodes + Immediate variiert 
je nach BITMODE.

- 8bit = 32bit opcode + 8bit immediate
- 16bit = 32bit opcode + 16bit immediate
- 32bit = 32bit opcode + 32bit immediate
- 64bit = 32bit opcode + 64bit immediate

## Bitset

### 4 bits für den gewünschten Modus
  - 0001 = BYTE
  - 0010 = WORD
  - 0011 = DWORD
  - 0100 = QWORD

### 8 bits für den gewünschten Opcode
  - 00000000 = reserved
  - 00000001 = nop 
  - 00000010 = sub
  - ...

### 3 bits für das erste Argument
  - 000 = Kein Argument
  - 001 = Speicheraddressierung
  - 010 = Registeradressierung
  - 011 = Immediate
  - 100 = Reserviert
  - ...
  - 111 = Reserviert

### 4 bits zur Identifikation des ersten Registers
  - 0000 = unused
  - 0001 = SP
  - 0010 = BP
  - 0011 = DS
  - 0100 = CS
  - 0101 = R0
  - 0110 = R1
  - 0111 = R2
  - 1000 = Reserviert
  - ...
  - 1111 = Reserviert

### 8 bits für ein Offset relativ zum übergebenen Register oder Speicher
  Eingelesen wird ein 8bit Integer mit Vorzeichen.

  Sollte ein Offset größer als 127 oder kleiner als -128 benötigt werden,
  muss über einen speziellen Opcode ein Offset als Immediate übergeben 
  und die Zieladresse auf diesen Weg errechnet werden.

### 3 bits für das zweite Argument
  - 000 = Kein Argument
  - 001 = Speicheraddressierung
  - 010 = Registeradressierung
  - 011 = Immediate
  - 100 = Reserviert
  - ...
  - 111 = Reserviert

### 2 bits als Daten-Indikator
  - 00 = Es folgen keine weiteren Daten
  - 01 = Es folgen 16 bits zur Initialisierung des zweiten Arguments
  - 10 = Reserviert
  - 11 = Reserviert
