# Header

Der Bytecode Header ist 32bits (4 Bytes) groß und beinhaltet die sog.
Magic-Bytes und den Bytecode Modus (32bit oder 64bit)

## 24 bits Magic-Bytes

Es folgen 3 Bytes mit den ASCII Werten 'H' 'V' 'M'

## 8 bit für den Modus
  - 00000000 = Reserviert
  - ...      = Reserviert
  - 00000011 = 32bit Modus
  - 00000100 = 64bit Modus
  - 00000101 = Reserviert
  - ...
  - 11111111 = Reserviert

# Opcode start (16bit)

### 12 bits für den gewünschten Opcode
  - 000000000000 = Reserviert
  - 000000000001 = nop 
  - 000000000010 = sub
  - ...

### 4 bits für die Anzahl an Argumenten
  - 0000 = Keine Argumente
  - 0001 = Ein Argument
  - 0010 = Zwei Argumente
  - 0011 = Reserviert
  - ...
  - 1111 = Reserviert

# Argument  (8bit)

### 2 bits zur Adressierung
  - 00 = Kein Argument
  - 01 = Speicheraddressierung
  - 10 = Registeradressierung
  - 11 = Reserviert

### 5 bits zur Identifikation des Registers
  - 00000 = Kein Register
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

# 1 bit als Displacement-Indikator
  - 0 = Kein Displacement
  - 1 = Es folgt ein Displacement

# Displacements

Die Größe des Displacements richtet sich nach dem 
Bytecode-Modus (4 oder 8 Byte).

# Immediates

Immediates werden von den jeweiligen OPCODE Handlern eingelesen.

Die Größe ist in den Handlern vorgegeben, idr. werden bei 8 bit Opcodes 
8bit Werte eingelesen, bei 16bit Opcodes entsprechend 16bit Werte usw.
