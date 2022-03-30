#include "AS5134.h"

//Create the encoder object
AS5134 encoder(4, 6, 7);

enum {
    WRITE_OTP = 31,
    PROG_OTP = 25,
    READ_OTP = 15,
    READ_ANA = 9,
    EN_PROG = 16
}CMD;

enum {
    SISO_SIMPLE_DATAWIDTH = 16,
    SISO_EXTENDED_DATAWIDTH = 62
}SISO_MODE_DATAWIDTH;

void loopthis();


void setup()
{
    Serial.begin(115200);
    encoder.init();
    encoder.setData(EN_PROG,0x8CAE); // 0x8CAE is the magic number to put it in OTP mode
    loopthis();

}

void loop()
{
    loopthis();
    
}

void loopthis()
{
    uint64_t data = 0;
    uint8_t R_add_bit = 27;
    delay(200);
    //data = encoder.getData(READ_OTP,SISO_EXTENDED_DATAWIDTH);

    data |= (uint64_t(R_add_bit) << 16) |  (uint64_t(1)<<13);
    delay(100);

    encoder.setData(WRITE_OTP,data, SISO_EXTENDED_DATAWIDTH);
    Serial.println(uint8_t(data>>16));
    
    delay(500);
}

