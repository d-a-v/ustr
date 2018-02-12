
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

#include <curses.h> // getch()

#include <pins_arduino.h>

class Stream
{
public:
	virtual size_t write (uint8_t c) = 0;
};


inline void yell (const char* s, int l)
{
	for (int i = 0; i < l; i++)
		printw("%s", s);
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

class HardwareSerial: public Stream
{
public:
	HardwareSerial () { initscr(); timeout(0); }
	~HardwareSerial () { endwin();  }
	void begin (int) { }
	bool available () { int c = getch(); if (c == ERR) { usleep(100000); return false; } ungetch(c); return true; }
	int read () { if (!available()) return -1; unsigned char ret = getch(); return ret; }
	size_t write (uint8_t c) { return myputchar(c); }

	void print (char c) { printw("%c", c); }
	void println (char c) { printw("%c\r\n", c); }
	void print (const char* s) { printw("%s", s); }
	void println (const char* s) { printw("%s\r\n", s); }
	void print (const String& s) { printw("%s", s.c_str()); }
	void println (const String& s) { printw("%s\r\n", s.c_str()); }
	void println () { printw("\r\n"); }
};

#define HIGH 1
#define LOW 0

#define pinMode(a,b) do { (void)a; } while (0)
#define digitalWrite(p,v) do { yell(" ", 10); printw("(pin %2i: out %i)", (p), !!(v)); yell("\b", 25); } while (0)
#define digitalRead(p) ({ struct timeval t; gettimeofday(&t, NULL); (t.tv_sec + p) & 1; })
#define analogWrite(p,v)  do { yell(" ", 10); printw("(pin %2i: pwm %i)", (p), !!(v)); yell("\b", 25); } while (0)

extern HardwareSerial Serial;
extern struct timeval tstart;

void delay (int ms);
unsigned long millis ();
int analogRead (int pin);

#endif // _ARDUINO_H
