
#ifndef __EEPROM_H
#define __EEPROM_H

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#define USTR_SIZE_EE 1024	//XXX fixme

class _EEPROM
{
private:
	static int fd;
public:
	_EEPROM (const char* file = "eeprom")
	{
		if (   (fd = open(file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1
		    || ftruncate(fd, USTR_SIZE_EE) == -1)
		{
			fprintf(stderr, "cannot open/create '%s' for r/w: %s\n\r", file, strerror(errno));
			fd = -1;
		}
	}
	~_EEPROM ()
	{
		if (fd != -1)
			close(fd);
	}
		
	inline static char read   (int x)         { char c = 0; if (pread(fd, &c, 1, x) != 1) fprintf(stderr, "eeprom: %s\n\r", strerror(errno)); return c; }
	inline static void put    (int x, char c) { if (x > USTR_SIZE_EE) fprintf(stderr, "### eeprom beyond\r\n"); else if (pwrite(fd, &c, 1, x) != 1) fprintf(stderr, "eeprom: %s\n\r", strerror(errno)); }
	inline static void update (int x, char c) { put(x, c); }
} EEPROM;

int _EEPROM::fd;

#endif
