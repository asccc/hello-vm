# Bytecode

Ein kompletter Opcode ohne immediate 
ist 48bits groß.

Je nach Anforderung kann ein Opcode mit 
einem immediate je nach Modus wachsen.

Die Größe eines OPcodes + Immediate variiert 
je nach BITMODE.

- 8bit = 32bit opcode + 8bit immediate
- 16bit = 32bit opcode + 16bit immediate
- 32bit = 32bit opcode + 32bit immediate
- 64bit = 32bit opcode + 64bit immediate

## Bitset

### 4 bits für den gewünschten Modus
  - 0001 = BYTE
  - 0010 = WORD
  - 0100 = DWORD
  - 1000 = QWORD

### 8 bits für den gewünschten Opcode
  - 00000000 = reserved
  - 00000001 = nop 
  - 00000010 = sub
  - ...

### 2 bits für das erste Argument
  - 00 = Kein Argument
  - 01 = Register
  - 10 = Adresse (übergeben als Immediate [size_t])
  - 11 = Immediate

### 4 bits zur Identifikation des ersten Registers
  - 0000 = unused
  - 0001 = SP
  - 0010 = BP
  - 0011 = DS
  - 0100 = R0
  - 0101 = R1
  - 0110 = R2
  - 0111 = R3
  - 1000 = R4
  - 1001 = R5
  - 1010 = R6
  - 1011 = R7
  - 1100 = unused
  - 1101 = unused
  - 1110 = unused
  - 1111 = unused

### 8 bits für ein Offset/Displacement relativ zum übergebenen Register oder Adresse
  Eingelesen wird ein 8bit Integer mit Vorzeichen.

  Sollte ein Offset größer als 127 oder kleiner als -128 benötigt werden,
  muss über einen speziellen Opcode ein Offset als Immediate übergeben 
  und die Zieladresse auf diesen Weg errechnet werden.

### 2 bits für das zweite Argument
  - 00 = Kein Argument
  - 01 = Register
  - 10 = Adresse (übergeben als Immediate [size_t])
  - 11 = Immediate

### 4 bits als Daten-Indikator
  - 0000 = Es folgen keine weiteren Daten
  - 0001 = Es folgen 16 bits zur Initialisierung des zweiten Arguments
  - 0010 = Es folgt ein Immediate, je nach Modus 8/16/32 oder 64 bits
  - 0011 = unused
  - ....
  - 1111 = unused
