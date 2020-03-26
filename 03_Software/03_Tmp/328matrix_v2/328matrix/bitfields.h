#include <avr/io.h>
#include <inttypes.h>

struct gpio_register_field {
	uint8_t b0:1;
	uint8_t b1:1;
	uint8_t b2:1;
	uint8_t b3:1;
	uint8_t b4:1;
	uint8_t b5:1;
	uint8_t b6:1;
	uint8_t b7:1;
};
union gpio_register {
	uint8_t byte;
	struct gpio_register_field bit;
};

// Port definitions
#define portb (*((volatile union gpio_register*)&PORTB))
#define portc (*((volatile union gpio_register*)&PORTC))
#define portd (*((volatile union gpio_register*)&PORTD))
#define HIGH 1
#define LOW 0
#define PULLUP 1
#define NOPULLUP 0

// Pin definitions
#define pinb (*((volatile union gpio_register*)&PINB))
#define pinc (*((volatile union gpio_register*)&PINC))
#define pind (*((volatile union gpio_register*)&PIND))

// Data-direction register definitions
#define ddrb (*((volatile union gpio_register*)&DDRB))
#define ddrc (*((volatile union gpio_register*)&DDRC))
#define ddrd (*((volatile union gpio_register*)&DDRD))
#define OUTPUT 1
#define INPUT 0

