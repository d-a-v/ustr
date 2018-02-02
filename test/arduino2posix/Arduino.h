
// arduino Stream emulation for Printf.h

#include <stdio.h>

#ifndef _ARDUINO_H
#define _ARDUINO_H

#include <string>

#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>

#include <pins_arduino.h>

class Stream
{
public:
	virtual size_t write (uint8_t c) = 0;
};


inline void yell (const char* s, int l)
{
	for (int i = 0; i < l; i++)
		printf("%s", s);
}

int myputchar (int c);

class String: private std::string
{
public:
	String(): std::string() { }
	String(const char* s): std::string(s) { }
	
	int length () const { return (int)size(); }
	void remove (int from) { resize(from); }
	String& operator += (char c) { std::string::operator += (c); return *this; }
	char& operator [] (int index) { return std::string::operator [] (index); }
	const char* c_str () const { return std::string::c_str(); }
	void reserve (unsigned int size) { return std::string::reserve(size); }
};

#include <aprintf.h>

class HardwareSerial: public Stream
{
public:
	HardwareSerial () { system("stty raw -echo"); }
	~HardwareSerial () { system("stty sane"); }
	void begin (int) { }
	bool available () { fd_set set; timeval z = { 0, 100000 }; FD_ZERO(&set); FD_SET(0, &set); return select(1, &set, NULL, NULL, &z) == 1;}
	char read () { char c = getchar(); if (c == 3 /*^C*/) { fprintf(stderr, "\n\naborted by user\r\n"); exit(2); } if (c == '\r' || c == '\n') { yell(" ", 40); yell("\b", 40); } myputchar(c); return c; }
	size_t write (uint8_t c) { return myputchar(c); }

	void print (char c) { printf("%c", c); }
	void println (char c) { printf("%c\r\n", c); }
	void print (const char* s) { printf("%s", s); }
	void println (const char* s) { printf("%s\r\n", s); }
	void print (const String& s) { printf("%s", s.c_str()); }
	void println (const String& s) { printf("%s\r\n", s.c_str()); }
	void println () { printf("\r\n"); }
};

#define HIGH 1
#define LOW 0

#define pinMode(a,b) do { (void)a; } while (0)
#define digitalWrite(p,v) do { yell(" ", 10); uprintf(&Serial, "(pin %2i: out %i)", (p), !!(v)); yell("\b", 25); fflush(stdout); } while (0)
#define digitalRead(p) ({ struct timeval t; gettimeofday(&t, NULL); (t.tv_sec + p) & 1; })
#define analogWrite(p,v)  do { yell(" ", 10); uprintf(&Serial, "(pin %2i: pwm %i)", (p), !!(v)); yell("\b", 25); fflush(stdout); } while (0)

extern HardwareSerial Serial;
extern struct timeval tstart;

void delay (int ms);
unsigned long millis ();
int analogRead (int pin);

#endif // _ARDUINO_H
