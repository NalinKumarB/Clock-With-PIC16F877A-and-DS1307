// // CONFIGURATION BITS
//#pragma config FOSC = HS
//#pragma config WDTE = OFF
//#pragma config PWRTE = OFF
//#pragma config BOREN = OFF
//#pragma config LVP = OFF
//#pragma config CPD = OFF
//#pragma config WRT = OFF
//#pragma config CP = OFF
//
//
//#include <xc.h>
//
//#define _XTAL_FREQ 20000000
//
//unsigned char sec,min,hour;
//
//const unsigned char seg_code[10] =
//{
//    0x3F, //0
//    0x06, //1
//    0x5B, //2
//    0x4F, //3
//    0x66, //4
//    0x6D, //5
//    0x7D, //6
//    0x07, //7
//    0x7F, //8
//    0x6F  //9
//};
//
///******************** I2C FUNCTIONS ********************/
///*
//void i2c_wait(void)
//{
//    while ((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));
//}
//
//void i2c_init(void)
//{
//    ADCON1 = 0x06;
//
//    TRISC3 = 1; // SCL
//    TRISC4 = 1; // SDA
//
//    SSPCON  = 0x28;     // I2C Master Mode
//    SSPCON2 = 0x00;
//    SSPSTAT = 0x00;
//
//    SSPADD = 49;        // 100kHz @ 20MHz 
//}
//
//void i2c_start(void)
//{
//    i2c_wait();
//    SEN = 1;
//}
//
//void i2c_restart(void)
//{
//    i2c_wait();
//    RSEN = 1;
//}
//
//void i2c_stop(void)
//{
//    i2c_wait();
//    PEN = 1;
//}
//
//void i2c_write(unsigned char data)
//{
//    i2c_wait();
//    SSPBUF = data;
//
//    while(BF);
//
//    i2c_wait();
//}
//
//unsigned char i2c_read(unsigned char ack)
//{
//    unsigned char data;
//
//    i2c_wait();
//
//    RCEN = 1;
//
//    while(!BF);
//
//    data = SSPBUF;
//
//    i2c_wait();
//
//    ACKDT = ack;
//    ACKEN = 1;
//
//    return data;
//}
//
///******************** DS1307 FUNCTIONS ********************/
//
//unsigned char ds1307_read(unsigned char addr)
//{
//    unsigned char data;
//
//    i2c_start();
//
//    i2c_write(0xD0);       // DS1307 Write Address
//    i2c_write(addr);
//
//    i2c_restart();
//
//    i2c_write(0xD1);       // DS1307 Read Address
//
//    data = i2c_read(1);    // NACK
//
//    i2c_stop();
//
//    return data;
//}
//
//void ds1307_write(unsigned char addr, unsigned char data)
//{
//    i2c_start();
//
//    i2c_write(0xD0);
//
//    i2c_write(addr);
//
//    i2c_write(data);
//
//    i2c_stop();
//}
//
///******************** DISPLAY FUNCTIONS ********************/
//
//void display_digit(unsigned char digit, unsigned char pos)
//{
//    PORTD = 0x0F;      // All digits OFF
//
//    PORTB = seg_code[digit];
//
//    switch(pos)
//    {
//        case 0: PORTD = 0x70; break;
//        case 1: PORTD = 0xB0; break;
//        case 2: PORTD = 0xD0; break;
//        case 3: PORTD = 0xE0; break;
//    }
//
//    __delay_ms(2);
//}
//
//void display_time(void)
//{
//    unsigned char h_tens;
//    unsigned char h_units;
//    unsigned char m_tens;
//    unsigned char m_units;
//
//    h_tens  = (hour >> 4) & 0x03;
//    h_units = hour & 0x0F;
//
//    m_tens  = (min >> 4) & 0x0F;
//    m_units = min & 0x0F;
//
//    display_digit(h_tens,0);
//    display_digit(h_units,1);
//    display_digit(m_tens,2);
//    display_digit(m_units,3);
//}
//
///******************** MAIN ********************/
//
//void main(void)
//{
//    unsigned int refresh;
//
//    TRISB = 0x00;
//    TRISD = 0x00;
//
//    PORTB = 0x00;
//    PORTD = 0x00;
//
//    i2c_init();
//
//    
//   // Uncomment only once to set RTC time.
//    //Example: 12:45:00
//
//    ds1307_write(0x00,0x00);
//    ds1307_write(0x01,0x45);
//    ds1307_write(0x02,0x12);
//   
//    while(1)
//    {
//        sec  = ds1307_read(0x00) & 0x7F;
//        min  = ds1307_read(0x01);
//        hour = ds1307_read(0x02) & 0x3F;
//
//        for(refresh=0; refresh<100; refresh++)
//        {
//            display_time();
//        }
//    }
//}



// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#include <xc.h>

#define _XTAL_FREQ 20000000

// AM/PM indicator LEDs
#define AM_LED  RC1
#define PM_LED  RC0

unsigned char sec, min, hour;
unsigned char is_pm;

const unsigned char seg_code[11] =
{
    0x3F, //0
    0x06, //1
    0x5B, //2
    0x4F, //3
    0x66, //4
    0x6D, //5
    0x7D, //6
    0x07, //7
    0x7F, //8
    0x6F, //9
    0x00  //10 = blank (used to suppress leading zero)
};

/******************** I2C FUNCTIONS ********************/

void i2c_wait(void)
{
    while ((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));
}

void i2c_init(void)
{
    ADCON1 = 0x06;

    TRISC3 = 1; // SCL
    TRISC4 = 1; // SDA

    SSPCON  = 0x28;     // I2C Master Mode
    SSPCON2 = 0x00;
    SSPSTAT = 0x00;

    SSPADD = 49;        // 100kHz @ 20MHz
}

void i2c_start(void)
{
    i2c_wait();
    SEN = 1;
}

void i2c_restart(void)
{
    i2c_wait();
    RSEN = 1;
}

void i2c_stop(void)
{
    i2c_wait();
    PEN = 1;
}

void i2c_write(unsigned char data)
{
    i2c_wait();
    SSPBUF = data;

    while(BF);

    i2c_wait();
}

unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;

    i2c_wait();

    RCEN = 1;

    while(!BF);

    data = SSPBUF;

    i2c_wait();

    ACKDT = ack;
    ACKEN = 1;

    return data;
}

/******************** DS1307 FUNCTIONS ********************/

unsigned char ds1307_read(unsigned char addr)
{
    unsigned char data;

    i2c_start();

    i2c_write(0xD0);       // DS1307 Write Address
    i2c_write(addr);

    i2c_restart();

    i2c_write(0xD1);       // DS1307 Read Address

    data = i2c_read(1);    // NACK

    i2c_stop();

    return data;
}

void ds1307_write(unsigned char addr, unsigned char data)
{
    i2c_start();

    i2c_write(0xD0);

    i2c_write(addr);

    i2c_write(data);

    i2c_stop();
}

/******************** DISPLAY FUNCTIONS ********************/

void display_digit(unsigned char digit, unsigned char pos)
{
    PORTD = 0x0F;      // All digits OFF

    PORTB = seg_code[digit];

    switch(pos)
    {
        case 0: PORTD = 0x70; break;
        case 1: PORTD = 0xB0; break;
        case 2: PORTD = 0xD0; break;
        case 3: PORTD = 0xE0; break;
    }

    __delay_ms(2);
}

void display_time(void)
{
    unsigned char h_tens;
    unsigned char h_units;
    unsigned char m_tens;
    unsigned char m_units;

    h_tens  = (hour >> 4) & 0x01;   // 12-hr mode: only 0 or 1
    h_units = hour & 0x0F;

    m_tens  = (min >> 4) & 0x0F;
    m_units = min & 0x0F;

    if (h_tens == 0)
        h_tens = 10;                // blank leading zero (e.g. " 9:05")

    display_digit(h_tens, 0);
    display_digit(h_units, 1);
    display_digit(m_tens, 2);
    display_digit(m_units, 3);
}

/******************** MAIN ********************/

void main(void)
{
    unsigned int refresh;
    unsigned char raw_hour;

    TRISB = 0x00;
    TRISD = 0x00;
    TRISC0 = 0;         // PM LED
    TRISC1 = 0;         // AM LED

    PORTB = 0x00;
    PORTD = 0x00;
    AM_LED = 0;
    PM_LED = 0;

    i2c_init();

    // Uncomment only once to set RTC time, then comment out again and reprogram.
    // Example: 12:45:00 PM
    // Hour register: bit6 = 1 (12-hr mode), bit5 = 1 (PM), then BCD hour
    //   12:45 AM -> 0x52
    //   12:45 PM -> 0x72
    //   09:30 AM -> 0x49
    //   09:30 PM -> 0x69

    
//    ds1307_write(0x00, 0x00);   // seconds (also starts oscillator)
//    ds1307_write(0x01, 0x19);   // minutes
//    ds1307_write(0x02, 0x72);   // 12:xx PM in 12-hour mode
    

    while(1)
    {
        sec      = ds1307_read(0x00) & 0x7F;
        min      = ds1307_read(0x01);
        raw_hour = ds1307_read(0x02);

        is_pm = (raw_hour & 0x20) ? 1 : 0;   // bit 5: 1 = PM
        hour  = raw_hour & 0x1F;             // bits 4-0: hour (BCD, 1-12)

        PM_LED = is_pm;
        AM_LED = !is_pm;

        for(refresh = 0; refresh < 100; refresh++)
        {
            display_time();
        }
    }
}