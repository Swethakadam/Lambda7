#include <avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#define SET_BIT(PORT, PIN) PORT |= (1<<PIN)
#define CLR_BIT(PORT, PIN) PORT &= ~(1<<PIN)

volatile int Brake_pedal=0;
void initADC();
int16_t readADC(int8_t ch);

int main(void)
{
    CLR_BIT(DDRD, 2);
    SET_BIT(PORTD, 2);
    SET_BIT(EICRA, ISC00);
    SET_BIT(EIMSK, INT0);
    sei();
    initADC();
    CLR_BIT(DDRC, PC0);
    CLR_BIT(DDRC, PC1);
    CLR_BIT(DDRC, PC2);
    CLR_BIT(DDRC, PC3);
    CLR_BIT(DDRC, PC4);
    CLR_BIT(DDRC, PC5);
    SET_BIT(DDRB, 5);
    SET_BIT(DDRB, 4);
    SET_BIT(DDRB, 3);
    SET_BIT(DDRB, 2);
    SET_BIT(DDRB, 1);
    int16_t Speed=0, Yaw_rate=0, Steer_angle=0, Wheel_speed_FL=0, Wheel_speed_FR=0,Tire_preseure=0, diff;

    while(1)
    {
        Speed = readADC(0);
        _delay_ms(200);
        Yaw_rate = readADC(1);
        _delay_ms(200);
        Steer_angle = readADC(2);
        _delay_ms(200);
        Wheel_speed_FL = readADC(3);
        _delay_ms(200);
        Wheel_speed_FR = readADC(4);
        _delay_ms(200);
        Tire_preseure = readADC(5);
        _delay_ms(200);

       Yaw_rate = Yaw_rate * 0.351625;
       Steer_angle = Steer_angle * 0.351625;
       diff = Steer_angle - Yaw_rate;

        if (Tire_preseure <30)
       {
           SET_BIT(PORTB, 1);
             _delay_ms(200);
        }
        else
       {
           CLR_BIT(PORTB, 1);
             _delay_ms(200);
       }



      if(Speed > 40)
      {
          //ABS
          if (Brake_pedal == 1)
          {
              SET_BIT(PORTB, 5);
             _delay_ms(200);
          }
          else
          {
              CLR_BIT(PORTB, 5);
              _delay_ms(200);
          }
          //Electronic stability control
          if((diff)>20)
        {
          PORTB|=(1<<PB4);
          _delay_ms(200);
          PORTB&=~(1<<PB3);
          _delay_ms(200);


        SET_BIT(PORTB, 5);
        _delay_ms(200);
        }

          else
          {
              CLR_BIT(PORTB, 5);
              _delay_ms(200);
          }


         if((diff)<-20)
        {
          PORTB|=(1<<PB3);
          _delay_ms(200);
          PORTB&=~(1<<PB4);
          _delay_ms(200);


              SET_BIT(PORTB, 5);
              _delay_ms(200);
          }
          else
          {
              CLR_BIT(PORTB, 5);
              _delay_ms(200);
          }


        if(((diff)>-20)&&((diff)<20))
        {
          PORTB&=~(1<<PB3);
          _delay_ms(200);
          PORTB&=~(1<<PB4);
          _delay_ms(200);
          CLR_BIT(PORTB, 5);
              _delay_ms(200);
        }


        //Traction control
        if (Wheel_speed_FL == Wheel_speed_FR)
        {
            CLR_BIT(PORTB, 2);
            _delay_ms(200);

              CLR_BIT(PORTB, 5);
              _delay_ms(200);
          }
        else
          {
              SET_BIT(PORTB, 5);
              _delay_ms(200);
              SET_BIT(PORTB, 2);
                _delay_ms(200);
          }
        }
        CLR_BIT(PORTB, 5);
         CLR_BIT(PORTB, 2);
        CLR_BIT(PORTB, 3);
        CLR_BIT(PORTB, 4);
      }

    return 0;
}





ISR(INT0_vect)
{
    if(!(PIND&(1<<PD2)))
        Brake_pedal = 1;
    else
        Brake_pedal = 0;
}
void initADC()
{
    ADMUX=(1<<REFS0);
    ADCSRA=(1<<ADEN)|(7<<ADPS0);
}
int16_t readADC(int8_t ch)
{
    ADMUX&=0xf8;
    ch=ch&0b00000111;
    ADMUX|=ch;

    ADCSRA|=(1<<ADSC);
    while(!(ADCSRA&(1<<ADIF)));
    ADCSRA|=(1<<ADIF);
    int16_t TEMP=0;
    TEMP|=(ADCH<<8);
    TEMP|=(ADCL);
    //ADC=TEMP;
    return(TEMP);
}
