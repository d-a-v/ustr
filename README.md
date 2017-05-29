# ustr
arduino library for universal "char*"-compatible string: transparent access to ram(rw), flash(ro), eeprom(rw)

this library indroduces two types: ustr ucstr

* ustr is made to behave like "char*"
* ucstr is made to behave like "const char*"

they both can point to ram, flash(PROGMEM,PSTR) or eeprom.

of course this is slow, but sometimes simple code is better.
