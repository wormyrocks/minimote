
#include <avr/interrupt.h>
#include <avr/io.h>
//#include "HIDSerial.h"
//#include <TinyWireM.h>

#define PWR 5
#define L1 8
#define L2 7
#define L3 3
#define L4 2
#define PADOUT 0
#define PADIN 1
volatile u8 show_num = 0;
volatile u8 show_num_ind = 0;
u32 shdwn = 10000;
ISR(TIMER1_COMPA_vect) {
  if (show_num && show_num < 32) {
    if (show_num & (0x10 >> show_num_ind)) showLED_(show_num_ind + 1);
    else showLED_(0);
    show_num_ind = (show_num_ind + 1) % 5;
  }
};

#define MEM_I2C_ADDR 0xa0

#define L1OFF PORTB &= ~(1<<2)
#define L1ON PORTB |=  (1<<2)
#define L1IN DDRB &=   ~(1<<2)
#define L1OUT DDRB |=  (1<<2)

#define L2OFF PORTA &= ~(1<<7)
#define L2ON PORTA  |= (1<<7)
#define L2IN DDRA &=   ~(1<<7)
#define L2OUT DDRA |=  (1<<7)

#define L3OFF PORTA &= ~(1<<3)
#define L3ON PORTA |=  (1<<3)
#define L3IN DDRA &=   ~(1<<3)
#define L3OUT DDRA |=  (1<<3)

#define L4OFF PORTA &= ~(1<<2)
#define L4ON PORTA |=  (1<<2)
#define L4IN DDRA &=   ~(1<<2)
#define L4OUT DDRA |=  (1<<2)

#define TOUCH1OFF PORTA &= ~1
#define TOUCH1ON PORTA |= 1
#define TOUCH1IN DDRA &= ~1
#define TOUCH1OUT DDRA |= 1
#define TOUCH1CLR PINA &= ~1
#define TOUCH1VAL PINA & 1

#define TOUCH2OFF PORTA &= ~2
#define TOUCH2ON PORTA |= 2
#define TOUCH2IN DDRA &= ~2
#define TOUCH2OUT DDRA |= 2
#define TOUCH2CLR PINA &= ~2
#define TOUCH2VAL (PINA>>1) & 1


void showLED(u8 led) {
  show_num = 0;
  showLED_(led);
}

void showLED_(u8 led) {
  if (led == 0) {
    L1IN;
    L2IN;
    L3IN;
  } else if (led == 1) {
    L1OUT;
    L2OUT;
    L3IN;
    L1ON;
    L2OFF;
  } else if (led == 2) {
    L1OUT;
    L2OUT;
    L3IN;
    L1OFF;
    L2ON;
  } else if (led == 3) {
    L2OUT;
    L3OUT;
    L1IN;
    L3OFF;
    L2ON;
  } else if (led == 4) {
    L2OUT;
    L3OUT;
    L1IN;
    L2OFF;
    L3ON;
  } else if (led == 5) {
    L1OUT;
    L3OUT;
    L2IN;
    L1OFF;
    L3ON;
  }
}
u8 capRead() {
  u8 total = 0;
  noInterrupts();
  TOUCH1OFF;
  TOUCH2IN;
  TOUCH2OUT;
  TOUCH2OFF;
  delayMicroseconds(10);
  TOUCH2IN;
  TOUCH2CLR;
  TOUCH1ON;
  interrupts();
  while (!TOUCH2VAL) total++;
  noInterrupts();
  TOUCH2ON;
  TOUCH2OUT;
  TOUCH2ON;
  TOUCH2IN;
  TOUCH2CLR;
  TOUCH1OFF;
  interrupts();
  while (TOUCH2VAL) total++;
  return total;
}

u32 capReadMult(u8 samplenum)
{
  u8 m = show_num;
  show_num = 0;
  u32 out = 0;
  for (u8 i = 0; i < samplenum; ++i) out += capRead();
  show_num = m;
  return out;
}

/*void ext_eeprom_write_byte(u16 addr, u8 b) {
  TinyWireM.begin();
  TinyWireM.beginTransmission(MEM_I2C_ADDR);
  TinyWireM.write((addr & 0xff));
  TinyWireM.write((addr >> 8) & 0xff);
  TinyWireM.write(b);
  TinyWireM.endTransmission(1);
}

u8 ext_eeprom_read_byte() {
  TinyWireM.begin();
  TinyWireM.beginTransmission(MEM_I2C_ADDR);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(MEM_I2C_ADDR, 2);
  return TinyWireM.receive();
}*/

void setup() {
  pinMode(L4, OUTPUT);
  digitalWrite(L4, LOW);
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, HIGH);
  cli();
  TCCR1A = 0;
  OCR1AH = 0x00;
  OCR1AL = 0x80;
  TIMSK1 = 0x2;
  TCCR1B = 0xc;
  sei();
  show_num=0;
}
u8 i = 1;
void loop() {
  delay(1000);
  ++show_num;
  if (show_num==32) show_num=0;
  if (millis() > shdwn) digitalWrite(PWR, LOW);
}
