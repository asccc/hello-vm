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

4 bits for the mode
  0001 = BYTE
  0010 = WORD
  0011 = DWORD
  0100 = QWORD
  0101 = reserved
  ....
  1111 = reserved

8 bits for the opcode group
  00000000 = reserved
  00000001 = nop 
  00000010 = sub
  ...

2 bits for the first argument
  00 = no argument
  01 = register
  10 = address (passed as imediate)
  11 = immediate

4 bits for the first register
  0000 = unused
  0001 = SP
  0010 = BP
  0011 = DS
  0100 = R0
  0101 = R1
  0110 = R2
  0111 = R3
  1000 = R4 // param passing
  1001 = R5 // param passing
  1010 = R6 // param passing
  1011 = R7 // param passing
  1100 = unused
  1101 = unused
  1110 = unused
  1111 = unused

8 bits for a offset
  value is a 8bit number with a sign

2 bits for the second argument
  00 = no argument
  01 = register 
  10 = address (passed as immediate)
  11 = immediate

4 bits data indicator
  0000 = do nothing
  0001 = load 16 bits to decode the second argument
  0010 = load a 8/16/32/64 bit immediate
  0011 = unused
  ....
  1111 = unused



