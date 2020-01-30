
#ifndef __USTRDEFS_H
#define __USTRDEFS_H

#if __AVR_ATmega168__ || __AVR_ATmega328P__ || defined(ARDUINO_AVR_DIGISPARK)
#include <avr/pgmspace.h>
// ram and flash addresses cast to uint32 overlap so macro below are mandatory
// mapping:
// ram (max 0x1000=4096) | flash (max 0xE000=57344) | eeprom (max 0x1000=4096)
// 0x0000         0x0fff | 0x1000            0xefff | 0xf000            0xffff
// XXX is optimizable?
#define RAM2USTR(x)		ustr(x)
#define USTR2RAM(x)		(x)
#define USTR_IS_RAM		((x) < 0x1000)
#if 1
#define ROM2USTR(x)		ustr((x) + 0x1000)
#define USTR2ROM(x)		((x) - 0x1000)
#define USTR_IS_ROM(x)		((x) >= 0x1000 && (x) < MAXSSCALAR(ustr_t))
#define EE2USTR(x)		ustr((x) + MAXSSCALAR(ustr_t))
#define USTR2EE(x)		((x) - MAXSSCALAR(ustr_t))
#define USTR_IS_EE(x)		((x) >= MAXSSCALAR(ustr_t))
#else
#define ROM2USTR(x)		ustr((x) + 0x1000)
#define USTR2ROM(x)		((x) - 0x1000)
#define USTR_IS_ROM(x)		((x) >= 0x1000 && (x) < 0xf000)
#define EE2USTR(x)		ustr((x) + 0xf000)
#define USTR2EE(x)		((x) - 0xf000)
#define USTR_IS_EE(x)		((x) >= 0xf000)
#endif
typedef uint16_t ustr_t;

#elif ESP8266
#include <pgmspace.h>
// ram and flash addresses cast to uint32 do not overlap
// mapping: https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map
// only eeprom has to be shifted
// XXX is optimizable?
#define RAM2USTR(x)		ustr(x)
#define USTR2RAM(x)		(x)
#define USTR_IS_RAM		((x) < 0x40000000)
#define ROM2USTR(x)		ustr(x)
#define USTR2ROM(x)		(x)
#define USTR_IS_ROM(x)		((x) >= 0x40000000 && (x) < 0xf0000000)
#define EE2USTR(x)		ustr((x) + 0xf0000000)
#define USTR2EE(x)		((x) - 0xf0000000)
#define USTR_IS_EE(x)		((x) >= 0xf0000000)
typedef uint32_t ustr_t;

#elif ARDUINO_EMU
#define RAM2USTR(x)		ustr(x)
#define USTR2RAM(x)		(x)
#define USTR_IS_RAM		((x) < 0xf0000000)
#define EE2USTR(x)		ustr((x) + MAXSSCALAR(ustr_t))
#define USTR2EE(x)		((x) - MAXSSCALAR(ustr_t))
#define USTR_IS_EE(x)		((x) >= MAXSSCALAR(ustr_t))
typedef unsigned long ustr_t;	// pointer always fit in a long

#else

#if !__x86_64__ && !__i686__ && !__i386__
#warning ustr: generic arch, should be personalized
#endif
typedef unsigned long ustr_t;		// pointer always fit in a long
#endif

///////////////////////////////////////////////////////////////

#ifndef PACK
#define PACK __attribute__((packed))
#endif

#ifndef typeof
#define typeof __typeof__
#endif

#ifndef UINT_T
#define UINT_T
typedef unsigned int uint_t;
#endif

#ifndef MAXSCALAR
#define MAXSCALAR(type,sz_bit)	((type)((((((type)1)<<((sz_bit)-1))-1)<<1)+1))
#define MAXUSCALAR(type)	MAXSCALAR(type,(sizeof(type)<<3))
#define MAXSSCALAR(type)	MAXSCALAR(type,(sizeof(type)<<3)-1)
#define MINSSCALAR(type)	((type)((type)1<<((sizeof(type)<<3)-1)))
#endif

#ifndef MAX
#define MAX(a,b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = b; _b > _a? _b: _a; })
#endif

#ifndef MIN
#define MIN(c,d) ({ __typeof__(c) _c = (c); __typeof__(d) _d = d; _d < _c? _d: _c; })
#endif

#ifndef SHL
// SHiftLeft - returns z << b, b can be < 0
#define SHL(z,b)		({ __typeof__(z) _z = (z); typeof(b) _b = (b); _b < 0? _z >> -_b: _z << _b; })
// SHiftRight - returns z >> b, b can be < 0
#define SHR(z,b)		(SHL((z),-(b)))
#endif

#ifndef STR
#define STR_HELPER(x)		#x
#define STR(x)			STR_HELPER(x)
#endif

#ifdef ARDUINO
#define USTR(x)			ROM2USTR(PSTR(x))
#else
// emulation
#define F(x) x
#define PSTR(x) x
#define PROGMEM
#endif

#endif // _USTRDEFS_H
