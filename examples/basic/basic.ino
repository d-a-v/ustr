
#include <ustr.h>

const char PROGMEM in_rom[] = "in rom";
char in_ram[] = "in ram";

void modify_first_char_and_display(ustr u)
{
    Serial.print("    ");
    (*u)++;
    while (*u)
    {
        Serial.print(*u++);
    }
    Serial.println();
}

void setup()
{
    Serial.begin(115200);

    // initialize eeprom
#ifdef ESP8266
    EEPROM.begin(512);
#endif
    EEPROM.put(510, '!');
    EEPROM.put(511, 0);

    Serial.println("I am always modified the same way:");
    modify_first_char_and_display(ustr::ptr(in_ram));

    Serial.println("I cannot be modified:");
    modify_first_char_and_display(ustr::rom(in_rom));

    Serial.println("I am changing accross reboots:");
    modify_first_char_and_display(ustr::ee(510));

    Serial.println("I cannot be modified:");
    modify_first_char_and_display(USTR("i am in rom too"));
}

void loop()
{
}
