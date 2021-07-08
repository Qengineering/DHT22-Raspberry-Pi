#ifndef TDHT22_H
#define TDHT22_H

#include <wiringPi.h>
//------------------------------------------------------------------------------------
struct TSenData
{
    float Temp  = 0.0;
    float Hum   = 0.0;
    bool  Valid = false;

    void operator = (const TSenData &a){
        Temp =a.Temp;
        Hum  =a.Hum;
        Valid=a.Valid;
    }
};
//------------------------------------------------------------------------------------
class TDHT22:public TSenData
{
public:
    bool Init(void);
    void Fetch(void);			//It takes 32.5 mSec to fetch a new reading

    TDHT22(int PinNo, bool Fahrenheit=false);
    virtual ~TDHT22();

protected:

private:
    bool Fh;
    bool Init_ok;
    int  wPin;
    int  data[5];

    void StartPuls(void);
};
//------------------------------------------------------------------------------------
#endif // TDHT22_H
