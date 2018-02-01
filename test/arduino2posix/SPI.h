
#ifndef __SPI_H
#define __SPI_H

#define LSBFIRST 0
#define MSBFIRST 1

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

class SPISettings
{
public:
	SPISettings (uint32_t clock = 0, uint8_t bitOrder = MSBFIRST, uint8_t dataMode = SPI_MODE0)
	{
		(void)clock;
		(void)bitOrder;
		(void)dataMode;
	}
};

class _SPI
{
public:
	void begin () { fprintf(stderr, "(fake spi)\r\n"); }
	void beginTransaction (const SPISettings& s) { (void)s; }
	uint8_t transfer (uint8_t) { return 0xaa; }
	void endTransaction () { }
} SPI;

#endif // __SPI_H
