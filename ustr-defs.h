
#ifndef __USTRDEFS_H
#define __USTRDEFS_H

#if defined(ARDUINO_AVR_DIGISPARK)
#include <avr/pgmspace.h>
#define USTR_RAM_SIZE 512
#define USTR_EE_SIZE 512
#define USTR_ROM_SIZE 8192
typedef uint16_t ustr_t;

#elif __AVR_ATmega328P__
#include <avr/pgmspace.h>
#define USTR_RAM_SIZE 0x1000	// data/heap<0x800 stack<0x900
#define USTR_EE_SIZE 1024
#define USTR_ROM_SIZE 32768
typedef uint16_t ustr_t;

#elif __AVR_ATmega168__
#include <avr/pgmspace.h>
#define USTR_RAM_SIZE 1024
#define USTR_EE_SIZE 512
#define USTR_ROM_SIZE 16384
typedef uint16_t ustr_t;

#elif ESP8266
#include <pgmspace.h>
#define USTR_EE_SIZE 4096		// should be enough
#define USTR_ROM_SIZE (32*1024*1024)	// 32MB flash at most
#define USTR_RAM_SIZE 0x80000000	// > ram mapping
typedef uint32_t ustr_t;

#elif ARDUINO_EMU
#define USTR_EE_SIZE 1024
#define USTR_RAM_SIZE (MAXUSCALAR(typeof(ustr_t)) - USTR_EE_SIZE)
typedef unsigned long ustr_t;		// pointer always fit in a long

#else

#warning ustr: generic arch, should be personalized
typedef unsigned long ustr_t;		// pointer always fit in a long
#endif

///////////////////////////////////////////////////////////////

#ifndef PACK
#define PACK __attribute__((packed))
#endif

#ifndef typeof
#define typeof __typeof__
#endif

#ifndef MAXSCALAR
#define MAXSCALAR(type,sz_bit)	((type)((((((type)1)<<((sz_bit)-1))-1)<<1)+1))
#define MAXUSCALAR(type)	MAXSCALAR(type,(sizeof(type)<<3))
#define MAXSSCALAR(type)	MAXSCALAR(type,(sizeof(type)<<3)-1)
#define MINSSCALAR(type)	((type)((type)1<<((sizeof(type)<<3)-1)))
#endif

#ifndef MAX
#define MAX(a,b) ({ typeof(a) _a = (a); typeof(b) _b = b; _b > _a? _b: _a; })
#define MIN(a,b) ({ typeof(c) _c = (c); typeof(d) _d = d; _d < _c? _d: _c; })
#endif

#ifndef SHL
// SHiftLeft - returns z << b, b can be < 0
#define SHL(z,b)		({ typeof(z) _z = (z); typeof(b) _b = (b); _b < 0? _z >> -_b: _z << _b; })
// SHiftRight - returns z >> b, b can be < 0
#define SHR(z,b)		(SHL((z),-(b)))
#endif

#ifndef STR
#define STR_HELPER(x)		#x
#define STR(x)			STR_HELPER(x)
#endif

#ifndef ARDUINO
// emulation
#define F(x) x
#define PSTR(x) x
#define USTR(x) ((char*)(x))
#define PROGMEM
#define STATICUSTR2(var,str) static const char var [] = str
#define STATICUSTR(val) STATICUSTR2(__ ## val, #val)
#endif

#endif // _USTRDEFS_H
