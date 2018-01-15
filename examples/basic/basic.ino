
#include <ustr.h>

const char PROGMEM in_rom[] = "in rom";
const char in_ram[] = "in ram";

void modify_first_char_and_display (ustr u)
{
  Serial.print("    ");
  (*u)++;
  while (*u)
    Serial.print(*u++);
  Serial.println();
}

void setup ()
{
  Serial.begin(115200);
  
  // initialize eeprom
  EEPROM.begin();
  EEPROM.put(USTR_EE_SIZE - 2, '!');
  EEPROM.put(USTR_EE_SIZE - 1, 0);

  Serial.println("I am always modified the same way:");
  modify_first_char_and_display(ustr::ptr(in_ram));

  Serial.println("I cannot be modified:");
  modify_first_char_and_display(ustr::rom(in_rom));

  Serial.println("I am changing accross reboots:");
  modify_first_char_and_display(ustr::ee(USTR_EE_SIZE - 2));

  Serial.println("I cannot be modified:");
  modify_first_char_and_display(USTR("i am in rom too"));
}

void loop ()
{
}
