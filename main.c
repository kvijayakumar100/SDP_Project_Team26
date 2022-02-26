/*
 * GccApplication2.c
 *
 * Created: 2/26/2022 2:35:29 PM
 * Author : sdp
 */ 

#include <avr/io.h>

void TWI_init_master(void) // Function to initialize master
{
	TWBR=0x01;    // Bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0);    // Setting prescalar bits
}

void TWI_start(void)
{
	// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT))); // Wait till start condition is transmitted
	while((TWSR & 0xF8)!= 0x08); // Check for the acknowledgement
}

void TWI_write_address(unsigned char data)
{
	TWDR=data; // Address and write instruction
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8)!= 0x18);  // Check for the acknoledgement
}

void TWI_read_address(unsigned char data)
{
	TWDR=data;    // Address and read instruction
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
	while((TWSR & 0xF8)!= 0x40);  // Check for the acknoledgement
}

void TWI_write_data(unsigned char data)
{
	TWDR=data;    // put data in TWDR
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x28); // Check for the acknoledgement
}

void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
}

void TWI_read_data(void)
{
	TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
	while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
	while((TWSR & 0xF8) != 0x58); // Check for the acknoledgement
	recv_data = TWDR;
	PORTB=recv_data;
}

uint8_t get_tot_mins(uint8_t difLevel) {
	uint8_t totalMins;
	
	if (0x0 < difLevel <= 0x3) {
		totalMins = 0x5;
	}
	else if (0x3 < difLevel <= 0x6) {
		totalMins = 0x6;
	}
	else {
		totalMins = 0x7;
	}

	return totalMins;
}

void send_data(uint8_t peripheral, uint8_t data) {
	TWI_start();
	TWI_write_address(peripheral);
	TWI_write_data(data);
	TWI_stop();
}

void get_data(uint8_t peripheral, uint8_t data) {
	TWI_start();
	TWI_read_address(peripheral);
	TWI_read_data(data);
	TWI_stop();
}

int main(void)
{
    uint8_t level = 0x1;
	uint8_t totalMin = get_tot_mins(level);
	bool boolVow = init_display();
	
	DDRD = 0b00000111;		// LED strike pins D0-D2 set as output
	
	TWI_init_master();
	
    while (1) 
    {
		// first simon says data sent
		TWI_start();
		TWI_write_address(0x38);
		TWI_write_data(level);
		TWI_stop();
		
		// second simon says data sent
		TWI_start();
		TWI_write_address(0x38);
		TWI_write_data(boolVow);
		TWI_stop();
		
		// third simon says data sent
		TWI_start();
		TWI_write_address(0x38);
		TWI_write_data(rSeed);
		TWI_stop();
		
		// first password data sent
		TWI_start();
		TWI_write_address(0x29);
		TWI_write_data(level);
		TWI_stop();
		
		// second password data sent
		TWI_start();
		TWI_write_address(0x29);
		TWI_write_data(rSeed);
		TWI_stop();
		
		// first clock data sent
		TWI_start();
		TWI_write_address(clck);
		TWI_write_data(totalMin);
		TWI_stop();
		
		while ((strikes < 3) && !game_over) {
			
			state = get_data(devList[i%3]);

			switch (state) {

				case 0x0: break;

				case 0x1:
				strikes = strikes + 0x1;
				if (devList[i%3] != simon) {
					send_data(simon, strikes);
				}
				break;

				case 0x2:
				finList[i%3] = true;
				break;
				
			}
			
			if (strikes == 1){
				PORTD |= (1 << DDD0);
			}

			else if (strikes == 2){
				PORTD |= (1 << DDD1);
			}
			
			else if (strikes == 3){
				PORTD |= (1 << DDD2);
			}

			i++;

			/* determine game over */
			if (((finList[0] & finList[1]) == true) || (finList[2] == true)) {
				game_over = true;
			}
		}
    }
}

