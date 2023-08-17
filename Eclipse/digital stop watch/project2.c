#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;
/*function to display the seconds ,
minutes and hours on the 6 7-segments at the same time */
void display();
void Timer1_CTC_Init(void){
	TCCR1A=(1<<FOC1A) | (1<<FOC1B); //non pwm
	TCNT1 = 0;		/* set timer1 initial count to zero */
	OCR1A = 15625;    /* Set the Compare value to 15625 */
	TIMSK |= (1<<OCIE1A); /* enable timer1 compare a Interrupt */
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS11); /* set prescalar to 64 CS10 =1 , CS11=1
	set the timer to operate at compare mood*/
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}
ISR(TIMER1_COMPA_vect){
	second++;
	if(second==60)
	{
		second=0;
		minute++;
	}
	if(minute==60)
	{
		second=0;
		minute=0;
		hour++;
	}
}
void INT0_Init_Reset(void)
{
	DDRD  &= (~(1<<PD2));  // Configure INT1/PD2 as input pin
	PORTD |= (1<<PD2);     // Enable the internal pull up resistor at PD2 pin
	GICR  |= (1<<INT0);    // Enable external interrupt pin INT0

	// Trigger INT0 with the falling edge
	MCUCR |= (1<<ISC11);
	MCUCR &= ~(1<<ISC10);
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}
ISR(INT0_vect)
{
	second=0;
	minute=0;
	hour=0;
}
void INT1_init_pause(void)
{
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1
	// Trigger INT1 with the raising edge
		MCUCR |= (1<<ISC11) |(1<<ISC10);
		SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}
ISR(INT1_vect)
{
	TIMSK &=~(1<<OCIE1A);
}
void INT2_Init_Resume(void)
{
	DDRB  &= (~(1<<PB2));  // Configure INT2/PB2 as input pin
	PORTB |= (1<<PB2);     // Enable the internal pull up resistor at PB2 pin
	GICR  |= (1<<INT2);    // Enable external interrupt pin INT2

	// Trigger INT2 with the falling edge
	MCUCSR &= ~(1<<ISC2);
	SREG  |= (1<<7);       // Enable interrupts by setting I-bit
}
ISR(INT2_vect){
	TIMSK |=(1<<OCIE1A);
}
int main()
{
	INT0_Init_Reset();
	INT2_Init_Resume();
	INT1_init_pause();
	Timer1_CTC_Init();
	DDRA |=(0x3F); //Configure the first 6 pins in port A as output pins
	PORTA &=(0xC0); //Set the first 6 pins in port A initially to zero
	DDRC |=(0x0F); //Configure the first 4 pins in port C as output pins
	PORTC &=(0xF0); //set the first 6 pins in port C initially to zero
	while (1)
	{
		display(); /* calling display function to display the seconds ,
		minutes and hours on the 6 7-segments at the same time */
	}
}
void display(){
	PORTA=(1<<5);
			PORTC =second%10;
			_delay_ms(2);
			PORTA=(1<<4);
			PORTC=second/10;
			_delay_ms(2);
			PORTA=(1<<3);
			PORTC=minute%10;
			_delay_ms(2);
			PORTA=(1<<2);
			PORTC=minute/10;
			_delay_ms(2);
			PORTA=(1<<1);
			PORTC=hour%10;
			_delay_ms(2);
			PORTA=(1<<0);
			PORTC=hour/10;
			_delay_ms(2);
}
