/*
Copyright 2017 david gauchard

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1.  Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2.  Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3.  Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __USTR_H
#define __USTR_H

#include <ustr-defs.h>

#ifdef __cplusplus

// mapping of ustr::c_off:
// RAM(RW):	0..RAM-1	offset in ram = real ram offset
// EEPROM(RW):	RAM..RAM+EE-1	offset in eeprom (starts at 0 in EEPROM::)
// FLASH(RO):	RAM+EE..oo	offset in rom/flash(ro) (starts at 0 in pgm_read_byte())

#ifndef USTR_RAM_SIZE
#define USTR_RAM_SIZE		MAXSSCALAR(ustr_t)
#endif

#ifndef USTR_EE_SIZE
#define USTR_EE_SIZE		0
#define _UEE(x...)
#else
#include <EEPROM.h>
#define _UEE(x...)		x
#endif

#ifndef USTR_ROM_SIZE
#define USTR_ROM_SIZE		0
#define _UROM(x...)
#define USTR(x)			((char*)(x))
#define STATICUSTR2(var,str)	static const char var [] = str
#define STATICUSTR(val)		STATICUSTR2(__ ## val, #val)
#else
#define _UROM(x...)		x
#define USTR(x)			(PSTR(x)+USTR_OFFS_ROM) // PSTR() is arduino's
#define STATICUSTR2(var,str)	static const char ustrom_##var [] PROGMEM = str; static const char* var = (ustrom_##var + USTR_OFFS_ROM)
#define STATICUSTR(val)		STATICUSTR2(__ ##val, #val)
#endif

#ifndef ESP8266
#define _UESP8266(x...)
#define EEPROM_update(x...)	EEPROM.update(x)
#else
#define _UESP8266(x...)		x
#define EEPROM_update(x...)	EEPROM.put(x)
#endif

#define USTR_OFFS_EE		(                 (USTR_RAM_SIZE))
#define USTR_OFFS_ROM		((USTR_OFFS_EE) + (USTR_EE_SIZE))

// generic 'char' equivalent identified by mapped address (ram, rom/flash, eeprom)
class PACK ustrc
{
private:

	// forbidden for all
	const ustrc& operator= (const ustrc&) { return *this; }

protected:

	// forbidden for user
	ustr_t c_off;		// mapped address
	ustrc () { }
	
public:

	static void begin ()
	{
		_UEE(_UESP8266(
			static char begun = 0;
			if (!begun)
			{
				EEPROM.begin(USTR_EE_SIZE);
				begun = 1;
			}
		))
	}
	
	static void commit ()
	{
		_UEE(_UESP8266(EEPROM.commit();))
	}
	
	enum type_e
	{
		e_str = 0,
	_UEE(	e_eeprom, )
	_UROM(	e_rom, )
	};

	type_e type () const
	{
		// keep order
	_UROM(	if (c_off >= USTR_OFFS_ROM) return e_rom; )
	_UEE(	if (c_off >= USTR_OFFS_EE) return e_eeprom; )
		return e_str;
	}
	
	char operator= (char c)
	{
		switch (type())
		{
	_UROM(	case e_rom:	/* silently ignore */ return c; )
	_UEE(	case e_eeprom:	EEPROM_update(c_off - USTR_OFFS_EE, c); return c; )
		case e_str:	return *(char*)c_off = c;
		}
		return 0; //XXX warn
	}
	
	operator char() const
	{
		switch (type())
		{
		case e_str:	return *(char*)c_off;
	_UROM(	case e_rom:	return pgm_read_byte((const void*)(c_off - USTR_OFFS_ROM)); )
	_UEE(	case e_eeprom:	return EEPROM.read(c_off - USTR_OFFS_EE); )
		}
		return 0; // keep gcc quiet
	}
	
#define OP(op) \
		bool operator op (const ustrc& other) const { return operator char() op other.operator char(); } \
		bool operator op (char other) const { return operator char() op other; }
	OP(==)
	OP(>)
	OP(>=)
	OP(<)
	OP(<=)
#undef OP

#define OP(op,tr) \
		ustrc& operator op (const ustrc& other) { operator=(operator char() tr other.operator char()); return *this; } \
		ustrc& operator op (char other) { operator=(operator char() tr other); return *this; }
	OP(+=, +)
	OP(-=, -)
	OP(*=, *)
	OP(/=, /)
	OP(&=, &)
	OP(|=, |)
	OP(^=, ^)
#undef OP

	ustrc& operator++ ()    { return operator+=(1); }
	ustrc& operator-- ()	{ return operator-=(1); }

	char operator++ (int) { char ret = operator char(); operator+=(1); return ret; }
	char operator-- (int) { char ret = operator char(); operator-=(1); return ret; }
};

class ucstr;

class PACK ustr: protected ustrc
{
public:

	ustr (const char* s = 0): ustrc()		{ begin(); c_off = (ustr_t)(long)s; }
	const ustr& operator=	(const ustr& u)		{ c_off = u.c_off; return *this; }

	static	ustr	ptr	(ustr_t m) 		{ return ustr((const char*)m); }
	static	ustr	ptr	(const char* m)		{ return ustr(m); }
_UEE(	static	ustr	ee	(ustr_t ee)		{ return ptr(ee + USTR_OFFS_EE); } )
_UROM(	static	ustr	rom	(const char* s)		{ return ptr((ustr_t)s + USTR_OFFS_ROM); } )

	operator bool		() const		{ return !!c_off; }
	operator ustr_t		() const		{ return c_off; }
	operator const char*	() const		{ return (const char*)c_off; }

	ustr	operator+	(int d) const		{ ustr copy = *this; return copy.operator+=(d); }
	ustr	operator-	(int d) const		{ return operator+(-d); }
	ustr_t	operator-	(const ustr& o) const	{ return c_off - o.c_off; }

	ustrc&	operator*	()			{ return *this; }

	ustr&	operator+=	(int d)			{ c_off += d; return *this; }
	ustr&	operator-=	(int d)			{ return operator+=(-d); }
	ustr&	operator++	()			{ return operator+=(1); }
	ustr&	operator--	()			{ return operator-=(1); }

	ustr	operator++	(int)			{ ustr copy = *this; operator++(); return copy; }
	ustr	operator--	(int)			{ ustr copy = *this; operator--(); return copy; }
	
#define OP(op) \
		bool operator op (const ustr& other) const { return c_off op other.c_off; } \
		bool operator op (const char* other) const { return (const char*)c_off op other; }
	OP(==)
	OP(>)
	OP(>=)
	OP(<)
	OP(<=)
#undef OP

	operator ucstr ();
};

class PACK ucstr: public ustr
{
private:

public:
	ucstr (const char* s = 0): ustr(s)		{ }
	const ucstr& operator=	(const ucstr& u)	{ ustr::operator=(u); return *this; }

	operator const char*	() const		{ return (char*)c_off; }
};

inline ustr::operator ucstr() { return ucstr(operator const char*()); }

ustr_t	ustrlen	(ucstr str);
ustr	ustrcpy	(ustr d, ucstr s);
ustr_t	ustrcmp	(ucstr s1, ucstr s2);

#else // !__cplusplus

typedef char* ustr;
typedef const char* ucstr;

#define ustrlen strlen
#define ustrcpy strcpy
#define ustrcmp strcmp

#endif // !__cplusplus

#endif // __USTR_H
