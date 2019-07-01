# Bytecode

Ein Opcode ist 32bits groß.

## Bitset

### 3 bits für den gewünschten Modus
  - 000 = Irrelevant
  - 001 = BYTE
  - 010 = WORD
  - 011 = DWORD
  - 100 = QWORD
  - 101 = Reserviert
  - 110 = Reserviert
  - 111 = Reserviert

### 12 bits für den gewünschten Opcode
  - 000000000000 = Reserviert
  - 000000000001 = nop 
  - 000000000010 = sub
  - ...

### 3 bits für das erste Argument
  - 000 = Kein Argument
  - 001 = Speicheraddressierung
  - 010 = Registeradressierung
  - 011 = Immediate
  - 100 = Reserviert
  - ...
  - 111 = Reserviert

### 5 bits zur Identifikation des ersten Registers
  - 00000 = unused
  - 00001 = SP
  - 00010 = BP
  - 00011 = DS
  - 00100 = CS
  - 00101 = R0
  - 00110 = R1
  - 00111 = R2
  - 01000 = Reserviert
  - ...
  - 11111 = Reserviert

### 3 bits für das zweite Argument
  - 000 = Kein Argument
  - 001 = Speicheraddressierung
  - 010 = Registeradressierung
  - 011 = Immediate
  - 100 = Reserviert
  - ...
  - 111 = Reserviert

### 5 bits zur Identifikation des zweiten Registers
  - 00000 = unused
  - 00001 = SP
  - 00010 = BP
  - 00011 = DS
  - 00100 = CS
  - 00101 = R0
  - 00110 = R1
  - 00111 = R2
  - 01000 = Reserviert
  - ...
  - 11111 = Reserviert

### 1 bit als Displacement Indikator
  - 0 = Es folgt kein Displacement
  - 1 = Es folgt ein je nach Modus 1, 2, 4 oder 8 Byte Displacement

## Displacements

Displacements diesen zur relativen Adressierung von Speicher.
Da es keine Zuordnung gibt zu welchem Argument ein Displacement gehört,
kann nur eines der beiden Argumente Speicheradressierung nutzen.

Displacements werden automatisch bei der Speicheradressierung geladen.

Die Größe von Displacements richtet sich nach der (kompilierenden) Platform.
Bei 32bit Machinen werden 32bit Displacements genutzt. 
Bei 64bit Machinen entsprechend 64bit Displacements.

Hierbei spielt der Opcode-Modus keine Rolle.

## Immediates

Immediates werden benutzt um scalare Werte als Argumente zu übergeben.

Immediates müssen von den Opcode-Handlern geladen werden. Es können
beliebig viele Werte genutzt werden (sofern diese im Bytecode 
hinterlegt wurden). Ebenfalls ist die größe von Immediates nicht 
vordefiniert. D.h. theoretisch kann ein 8bit Opcode ein 64bit
Immediate laden.
