#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define F_CPU 1000000UL
#define DELAY 4
#define NUMBER_OF_TICKS_PER_SEC 15625 //we used pre64
unsigned char Seconds_unit=0;
unsigned char Seconds_tenth=0;
unsigned char Minutes_unit=0;
unsigned char Minutes_tenth=0;
unsigned char Hours_unit=0;
unsigned char Hours_tenth=0;

void Vid_Timer1_Init(void);
void Vid_Display(void);
void Vid_Interrupt_Init(void);
int main()
{
	Vid_Timer1_Init();
	Vid_Interrupt_Init();
	while(1)
	{
		Vid_Display();
	}
}
void Vid_Timer1_Init(void)
{
	TCCR1A=(1<<FOC1A);
	TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS11);
	TIMSK=(1<<OCIE1A);
	OCR1A=NUMBER_OF_TICKS_PER_SEC;
	sei();
	/*
	 * choosing pre64 and  CTC mode for timer 1
	 * enabling interrupt enable for Timer1 CTC mode
	 * Top=15625
	 */
}
ISR(TIMER1_COMPA_vect)
{
	OCR1A=NUMBER_OF_TICKS_PER_SEC;
	Seconds_unit++;
	if(10==Seconds_unit)
		{
			Seconds_tenth++;
			Seconds_unit=0;
		}
	if(6==Seconds_tenth)
		{
			Minutes_unit++;
			Seconds_unit=0;
			Seconds_tenth=0;
		}
	if(10==Minutes_unit)
		{
			Minutes_tenth++;
			Minutes_unit=0;
			Seconds_unit=0;
			Seconds_tenth=0;
		}
	if(6==Minutes_tenth)
		{
			Hours_unit++;
			Minutes_tenth=0;
			Minutes_unit=0;
			Seconds_unit=0;
			Seconds_tenth=0;
		}
	if(10==Hours_unit)
		{
			Hours_tenth++;
			Hours_unit=0;
			Minutes_tenth=0;
			Minutes_unit=0;
			Seconds_unit=0;
			Seconds_tenth=0;
		}
	/*
	 * every 1 second the second_units increment
	 * and every 10 seconds the second_tenth increment
	 * and so on
	 */

}
void Vid_Display(void)
{
	DDRC=0x0F;
	DDRA=0xFF;
	PORTA=0x00;
	_delay_ms(DELAY);
	PORTC=Seconds_unit;
	PORTA=(1<<5);
	_delay_ms(DELAY);
	PORTA=0x00;
	PORTC=Seconds_tenth;
	PORTA=(1<<4);
	_delay_ms(DELAY);
	PORTA=0x00;
	PORTC=Minutes_unit;
	PORTA=(1<<3);
	_delay_ms(DELAY);
	PORTA=0x00;
	PORTC=Minutes_tenth;
	PORTA=(1<<2);
	_delay_ms(DELAY);
	PORTA=0x00;
	PORTC=Hours_unit;
	PORTA=(1<<1);
	_delay_ms(DELAY);
	PORTA=0x00;
	PORTC=Hours_tenth;
	PORTA=(1<<0);
	_delay_ms(DELAY);
	/*
	 * multipliexing 7-segments idea is to but the data on data bus (PORTC) while the control bus (PORTA)
	 * is unactive.
	 * then activate the bit of the 7-segment that is supposed to show number.
	 * then delay a little bit and so on
	 */

}
void Vid_Interrupt_Init(void)
{

	DDRD&=~(1<<2);
	DDRD&=~(1<<3);
	PORTD|=(1<<2);
	DDRB&=~(1<<2);
	PORTB|=(1<<2);
	MCUCR|=(1<<ISC01)|(1<<ISC10)|(1<<ISC11);
	GICR|=(1<<INT0)|(1<<INT1)|(1<<INT2);
	/*
	 * Enabling int 0 falling endge, int 1 rising edge and int2 falling edge;
	 *
	 *
	 */
}
ISR(INT0_vect)
{
	Seconds_unit=0;
	Seconds_tenth=0;
	Minutes_unit=0;
	Minutes_tenth=0;
	Hours_unit=0;
	Hours_tenth=0;
/*
 * just reseting by making all the values that i already incremented in the timer with 0 again.
 */
}
ISR(INT1_vect)
{
	TCCR1B&=~(1<<CS10);
	TCCR1B&=~(1<<CS11);
	/*
	 * pausing the timer by removing the prescalar(timer stop)
	 */
}
ISR(INT2_vect)
{
	TCCR1B|=(1<<CS10)|(1<<CS11);
/*
 * resuming the timer by making the prescalar 64 again as it was
 */
}
