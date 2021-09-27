#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

void AWAIT_CALL(unsigned char address);
void SLAVE_RECEIVE(void);

unsigned char data[] = {};

int main(void){
	DDRA = 0xFF;

	AWAIT_CALL(0x20);

	SLAVE_RECEIVE();

	for(uint8_t i=0;data[i]!=0;i++){
		PORTA = data[i];
		_delay_ms(1000);
	}
}

/*!
 *	@brief Wait and listen for call from master device.
 *	@param Device's address (unsigned char).
 */
void AWAIT_CALL(unsigned char address){
	TWAR = address; 							///< Load device's address onto TWAR register.
	TWCR = (1<<TWEN)|(1<<TWEA);						///< Enable TWI and set TWINT bit to 1. Generate an ACK bit on the TWI bus if the address is received.
	while(!(TWCR & (1<<TWINT)));						///< Wait while TWI completes current action.
	while((TWSR & 0xF8) != 0x60);						///< While address has not been received or ACK has not been sent.
}

/*!
 *	@brief Receive data from master device.
 */

void SLAVE_RECEIVE(void){
	uint8_t i = 0;
	while((TWSR & 0xF8) != 0xA0){						///< While a STOP condition has not been received.
		TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);				///< Enable TWI and set TWINT bit to 1. Generate an ACK bit if a data byte has been received.
		while(!(TWCR & (1<<TWINT)));					///< Wait while TWI completes current action.
		if((TWSR & 0xF8) != 0xA0){
			while((TWSR & 0xF8) != 0x80);				///< If a STOP condition has not been received, wait until data is received.
			data[i] = TWDR;						///< Store received contents in the array.
		}
		i++;
	}
}
