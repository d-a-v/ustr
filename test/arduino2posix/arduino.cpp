
#define ARDUINO_EMU 1

struct timeval tstart;
HardwareSerial Serial;

#include <math.h>
#include <string.h>
#include <curses.h>

void delay (int ms)
{
	usleep(ms * 1000);
}

unsigned long millis ()
{
	struct timeval tnow;
	gettimeofday(&tnow, NULL);
	return   (tnow.tv_sec  - tstart.tv_sec ) * 1000
	       + (tnow.tv_usec - tstart.tv_usec) / 1000;
}

int analogRead (int pin)
{
	return 512 * (1.0 + sin(millis() / 3142.0 / pin));
}

int myputchar (int c)
{
	switch (c)
	{
	case 10: putchar(13); break;
	case 127: return 0;
	default:;
	}
	//putchar(c); fflush(stdout);
	addch(c);
	return 1;
}

int main (void)
{
	gettimeofday(&tstart, NULL);
	setup();
	while (1)
		loop();
	return 0;
}
