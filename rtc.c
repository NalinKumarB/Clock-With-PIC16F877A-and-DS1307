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

unsigned char sec,min,hour;

const unsigned char seg_code[10] =
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
    0x6F  //9
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
        case 0: PORTD = 0x0E; break;
        case 1: PORTD = 0x0D; break;
        case 2: PORTD = 0x0B; break;
        case 3: PORTD = 0x07; break;
    }

    __delay_ms(2);
}

void display_time(void)
{
    unsigned char h_tens;
    unsigned char h_units;
    unsigned char m_tens;
    unsigned char m_units;

    h_tens  = (hour >> 4) & 0x03;
    h_units = hour & 0x0F;

    m_tens  = (min >> 4) & 0x0F;
    m_units = min & 0x0F;

    display_digit(h_tens,0);
    display_digit(h_units,1);
    display_digit(m_tens,2);
    display_digit(m_units,3);
}

/******************** MAIN ********************/

void main(void)
{
    unsigned int refresh;

    TRISB = 0x00;
    TRISD = 0x00;

    PORTB = 0x00;
    PORTD = 0x00;

    i2c_init();

    
   // Uncomment only once to set RTC time.
    //Example: 12:45:00

//    ds1307_write(0x00,0x00);
//    ds1307_write(0x01,0x45);
//    ds1307_write(0x02,0x12);
   
    while(1)
    {
        sec  = ds1307_read(0x00) & 0x7F;
        min  = ds1307_read(0x01);
        hour = ds1307_read(0x02) & 0x3F;

        for(refresh=0; refresh<100; refresh++)
        {
            display_time();
        }
    }
}
