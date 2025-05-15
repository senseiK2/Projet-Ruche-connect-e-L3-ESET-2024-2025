#ifndef DS18B20_H_
#define DS18B20_H_

uint8_t DS18B20_reset(GPIO_TypeDef *GPIO, uint8_t PIN);
void DS18B20_WBit(GPIO_TypeDef *GPIO, uint8_t PIN, uint8_t Bit);
void DS18B20_WOctet(GPIO_TypeDef *GPIO, uint8_t PIN, uint8_t Octet);
uint8_t DS18B20_RBit(GPIO_TypeDef *GPIO, uint8_t PIN);
uint8_t DS18B20_ROctet(GPIO_TypeDef *GPIO, uint8_t PIN);
float DS1820_Lecture(GPIO_TypeDef *GPIO, uint8_t broche);

#endif
