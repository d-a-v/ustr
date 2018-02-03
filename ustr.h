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

#if defined(__cplusplus) && defined(USTR_IS_RAM)

#ifndef USTR_IS_EE
#define _UEE(x...)
#else
#include <EEPROM.h>
#define _UEE(x...)		x
#endif

#ifndef USTR_IS_ROM
#define _UROM(x...)
#define ROM2USTR(x)		(x)
#define USTR2ROM(x)		(x)
#define USTR_IS_ROM(x)		(0)
#define USTR(x)			(x)
#else
#define _UROM(x...)		x
#define STATICUSTR2(var,str)	static const char ustrom_##var [] PROGMEM = str; static const char* var = ROM2USTR(ustrom_##var)
#define USTR(x)			ROM2USTR(PSTR(x))
#endif


#ifndef ESP8266
#define _UESP8266(x...)
#else
#define _UESP8266(x...)		x
#endif

// generic 'char* as pointer to char' equivalent identified by mapped address (ram, rom/flash, eeprom)
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

#if 0
	static void commit ()
	{
		_UEE(_UESP8266(EEPROM.commit();))
	}
#endif
	enum type_e
	{
		e_str = 0,
		e_eeprom,
		e_flash,
	};

	type_e type () const
	{
	_UROM(	if (USTR_IS_ROM(c_off)) return e_flash; )
	_UEE(	if (USTR_IS_EE(c_off)) return e_eeprom; )
		return e_str;
	}
	
	char operator= (char c)
	{
		switch (type())
		{
	_UROM(	case e_flash:	/* silently ignore */ return c; )
	_UEE(	case e_eeprom:	EEPROM.put(USTR2EE(c_off), c); return c; )
		default:	return *(char*)c_off = c;
		}
	}
	
	operator char() const
	{
		switch (type())
		{
	_UROM(	case e_flash:	return pgm_read_byte((const void*)USTR2ROM(c_off)); )
	_UEE(	case e_eeprom:	return EEPROM.read(USTR2EE(c_off)); )
		default:	return *(char*)c_off;
		}
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

// generic 'char* viewed as a string'
class PACK ustr: protected ustrc
{
public:

	static	ustr	ee	(ustr_t ee)		{ return ustr(EE2USTR(ee)); }
	static	ustr	ptr	(const char* m)		{ return ustr(RAM2USTR(m)); }
	static	ustr	rom	(const char* s)		{ return ustr(ROM2USTR(s)); }

	ustr (ustr_t u)					{ c_off = u; }
	ustr (const char* s = 0): ustrc()		{ c_off = (ustr_t)(long)s; }
	const ustr& operator=	(const ustr& u)		{ c_off = u.c_off; return *this; }

	operator bool		() const		{ return !!c_off; }
	operator ustr_t		() const		{ return c_off; }
	operator char*		() const		{ return (char*)c_off; }
	operator const char*	() const		{ return (const char*)c_off; }
	const char* c_str	() const		{ return (const char*)c_off; }

	ustr	operator+	(long unsigned d) const	{ ustr copy = *this; return copy.operator+=(d); }
	ustr	operator-	(long unsigned d) const	{ return operator+(-d); }
	ustr	operator+	(int d) const		{ ustr copy = *this; return copy.operator+=(d); }
	ustr	operator-	(int d) const		{ return operator+(-d); }
	ustr_t	operator-	(const ustr& o) const	{ return c_off - o.c_off; }
	ustr_t	operator-	(const char* o) const	{ return c_off - (ustr_t)o; }

	ustrc&	operator*	()			{ return *this; }

	ustr&	operator+=	(int d)			{ c_off += d; return *this; }
	ustr&	operator-=	(int d)			{ return operator+=(-d); }
	ustr&	operator++	()			{ return operator+=(1); }
	ustr&	operator--	()			{ return operator-=(1); }

	ustr	operator++	(int)			{ ustr copy = *this; operator++(); return copy; }
	ustr	operator--	(int)			{ ustr copy = *this; operator--(); return copy; }
	
//	ustr	operator[]	(int i)			{ return ustr(c_off + (ustr_t)i); }
	
#define OP(op) \
		bool operator op (const ustr& other) const { return c_off op other.c_off; } \
		bool operator op (const char* other) const { return (const char*)c_off op other; } \
		bool operator op (char* other) const { return (const char*)c_off op other; }
	OP(==)
	OP(>)
	OP(>=)
	OP(<)
	OP(<=)
#undef OP

	operator ucstr () __attribute__((weak));

	type_e type		() const		{ return ustrc::type(); }
};

// generic 'const char* viewed as a string'
class PACK ucstr: public ustr
{
private:

public:
	ucstr (const char* s = 0): ustr(s)		{ }
	const ucstr& operator=	(const ucstr& u)	{ ustr::operator=(u); return *this; }

	operator const char*	() const		{ return (const char*)c_off; }
};

ustr::operator ucstr() { return ucstr(operator const char*()); }

///XXX make templates with ustr?

ustr_t	ustrlen	(ucstr str);
ustr	ustrcpy	(ustr d, ucstr s);
ustr_t	ustrcmp	(ucstr s1, ucstr s2);
ucstr	ustrchr	(ucstr s, char c);

#else

typedef char* ustr;
typedef const char* ucstr;

#define ustrlen strlen
#define ustrcpy strcpy
#define ustrcmp strcmp

#endif // !__cplusplus

#ifndef STATICUSTR2
#define STATICUSTR2(var,str)	static const char var [] = str
#endif
#define STATICUSTR(val)		STATICUSTR2(__ ##val, #val)

#ifndef ROM2USTR
#define ROM2USTR(x)		(x)
#endif

#endif // __USTR_H
