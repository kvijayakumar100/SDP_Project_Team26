#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#define DEBOUNCE_TIME 25

void TWIInit(void)
{
	TWSR = 0x00;
	TWBR = 0x0C;
	TWAR = 0x70;
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

void TWIStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

void TWIStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWIWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

uint8_t TWIReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t TWIGetStatus(void)
{
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	return status;
}

void flashSeq(int *sequence, int seqLength)
{
	for (int j = 0; j <= seqLength; j++)
	{
		
		_delay_ms(150);
		if (sequence[j] == 0) //Green
		{
			PORTD |= (1 << DDD1);
			_delay_ms(150);
			PORTD &= ~(1 << DDD1);
		}
		if (sequence[j] == 1) //Red
		{
			PORTD |= (1 << DDD3);
			_delay_ms(150);
			PORTD &= ~(1 << DDD3);
		}
		if (sequence[j] == 2) //Blue
		{
			PORTD |= (1 << DDD5);
			_delay_ms(150);
			PORTD &= ~(1 << DDD5);
		}
		if (sequence[j] == 3) //Yellow
		{
			PORTD |= (1 << DDD7);
			_delay_ms(150);
			PORTD &= ~(1 << DDD7);
		}
	}
}

int compareArrays(int a[], int b[], int arrSize)
{
	for(int i = 0; i < arrSize; i++)
	{
		if (a[i] != b[i])
		{
			return 0;
		}
	}
	return 1;
}


int main(void)
{
	DDRB = 0x00;
	DDRD = 0b10101010;
	DDRC = 0b00001011;
	PORTC = 0b00000100;
	uint8_t difLevel;
	uint8_t contVowel;
	uint8_t rSeed;
	uint8_t strikes = 0x00;
	bool First = true;
	bool complete = false;
	int numCompleted = 0;
	bool gameOver = false;
	TWIInit();
	PORTC &= ~(1 << DDC3);
	while (gameOver == false)
	{
		if (First == true)
		{
			while (1)
			{
				if (TWIGetStatus() == 0x60)
				{
					//PORTC |= (1 << DDC0);
					difLevel = TWIReadACK();
					TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
					TWIInit();
					break;
				}
			}
			
			while (1)
			{
				if (TWIGetStatus() == 0x60)
				{
					//PORTC |= (1 << DDC0);
					contVowel = TWIReadACK();
					TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
					TWIInit();
					break;
				}
			}
			while (1)
			{
				if (TWIGetStatus() == 0x60)
				{
					//PORTC |= (1 << DDC0);
					rSeed = TWIReadACK();
					TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
					TWIInit();
					break;
				}
			}
		}
		First = false;
		int need2Complete = difLevel + 2;
		srand(rSeed);

		int seqLength = difLevel+2+numCompleted;

		int sequence[seqLength];
		
		int corrSequence[seqLength];
		
		int pressedSequence[seqLength];
		
		
		//PORTC |= (1 << DDC1); // turn red led on, indicating module is unfinished
		
		PORTC |= (1 << DDC0); // turn red led on, indicating module is unfinished

		for (int i = 0; i < seqLength; i++)
		{
			
			sequence[i] = rand() % 4;
			
			if (contVowel == 0x01)
			{
				if (strikes == 0)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 2;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 1;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 0;
					}
				}
				
				else if (strikes == 1)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 2;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 0;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 1;
					}
				}
				
				else if (strikes == 2)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 0;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 1;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 2;
					}
				}
			}
			if (contVowel == 0x00)
			{
				if (strikes == 0)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 0;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 2;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 1;
					}
				}
				
				else if (strikes == 1)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 1;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 2;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 0;
					}
				}
				
				else if (strikes == 2)
				{
					if (sequence[i] == 0)
					{
						corrSequence[i] = 2;
					}
					
					else if (sequence[i] == 1)
					{
						corrSequence[i] = 3;
					}
					
					else if (sequence[i] == 2)
					{
						corrSequence[i] = 0;
					}
					
					else if (sequence[i] == 3)
					{
						corrSequence[i] = 1;
					}
				}
			}
			
		}
		
		flashSeq(sequence, seqLength);
		
		int count = 0;
		
		while(complete == false){
			
			/*if (TWIGetStatus() == 0xA8)
			{
				TWIWrite(strikes);
				TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
				TWIInit();
			}*/
			
			if ((PIND & (1 << PIND0)))
			{
				_delay_ms(DEBOUNCE_TIME);
				if ((PIND & (1 << PIND0)))
				{
					PORTD ^= (1<<DDD1);
					_delay_ms(20);
					PORTD &= ~(1<<DDD1);
					pressedSequence[count] = 0;
					count++;
				}
			}
			if ((PIND & (1 << PIND2)))
			{
				_delay_ms(DEBOUNCE_TIME);
				if ((PIND & (1 << PIND2)))
				{
					PORTD ^= (1<<DDD3);
					_delay_ms(20);
					PORTD &= ~(1<<DDD3);
					pressedSequence[count] = 1;
					count++;
				}
			}
			if ((PIND & (1 << PIND4)))
			{
				_delay_ms(DEBOUNCE_TIME);
				if ((PIND & (1 << PIND4)))
				{
					PORTD ^= (1<<DDD5);
					_delay_ms(20);
					PORTD &= ~(1<<DDD5);
					pressedSequence[count] = 2;
					count++;
				}
			}
			if ((PIND & (1 << PIND6)))
			{
				_delay_ms(DEBOUNCE_TIME);
				if ((PIND & (1 << PIND6)))
				{
					PORTD ^= (1<<DDD7);
					_delay_ms(20);
					PORTD &= ~(1<<DDD7);
					pressedSequence[count] = 3;
					count++;
				}
				
			}
			
			if ((PINC & (1 << PINC3)))
			{
				flashSeq(sequence, seqLength);
			}
			
			if(count == seqLength)
			{
				complete = true;
			}
			
		}
		if(compareArrays(pressedSequence, corrSequence, seqLength) == 1)
		{
			numCompleted ++;
			PORTC &= ~(1 << DDC0);
			PORTC |= (1 << DDC1);
			_delay_ms(100);
			
			if (need2Complete == numCompleted)
			{
				gameOver = true;
				while(TWIGetStatus() != 0xA8); //look here if strike gets sent or not right away not after password
				TWIWrite(2);
				TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
				TWIInit();
			}
			else
			{
				PORTC |= (1 << DDC0);
				PORTC &= ~(1 << DDC1);
				complete = false;
			}
			
		}
		else
		{
			PORTC &= ~(1 << DDC0);
			_delay_ms(50);
			PORTC |= (1 << DDC0);
			strikes += 1;
			complete = false;
			while(TWIGetStatus() != 0xA8);
			TWIWrite(1);
			TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
			TWIInit();
		}
		
		if (TWIGetStatus() == 0xA8)
		{
			TWIWrite(0);
			TWCR = (0 << TWSTO) | (1 << TWINT) | (1 << TWEA);
			TWIInit();
		}
		
	}
	
}
