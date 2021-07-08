#include "DHT22.h"
#include <time.h>

#define MAX_TIMINGS 85                  // Takes 84 state changes to transmit data
//------------------------------------------------------------------------------------
static int durn(struct timespec t1, struct timespec t2) {
	return(((t2.tv_sec-t1.tv_sec)*1000000) + ((t2.tv_nsec-t1.tv_nsec)/1000));	// elapsed microsecs
}
//------------------------------------------------------------------------------------
TDHT22::TDHT22(int PinNo, bool Fahrenheit)
{
    wPin=PinNo;
    Fh  =Fahrenheit;
}
//------------------------------------------------------------------------------------
TDHT22::~TDHT22()
{
    //dtor
}
//------------------------------------------------------------------------------------
bool TDHT22::Init(void)
{
    Init_ok = ( wiringPiSetup() >= 0 );

    if(Init_ok) pinMode( wPin, INPUT );

    return Init_ok;
}
//------------------------------------------------------------------------------------
void TDHT22::StartPuls(void)
{
    // Signal Sensor we're ready to read by pulling pin UP for 10 mS.
    // pulling pin down for 18 mS and then back up for 40 µS.
    pinMode( wPin, OUTPUT );
    digitalWrite( wPin, HIGH );
    delay(10);
    digitalWrite( wPin, LOW );
    delay(18);
    digitalWrite( wPin, HIGH );
    delayMicroseconds(40);
    pinMode( wPin, INPUT );
}
//------------------------------------------------------------------------------------
void TDHT22::Fetch(void)
{
    if(!Init_ok) return;

    struct timespec	st, cur;
    int uSec = 0;
    int Toggles   = 0;
    int BitCnt    = 0;
    int lastState = HIGH;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    StartPuls();

    // Read data from sensor.
    for(Toggles=0; (Toggles < MAX_TIMINGS) && (uSec < 255); Toggles++) {

        clock_gettime(CLOCK_REALTIME, &st);
        while((digitalRead(wPin)==lastState) && (uSec < 255) ) {
            clock_gettime(CLOCK_REALTIME, &cur);
            uSec=durn(st,cur);
        };

        lastState = digitalRead( wPin );

        // First 2 state changes are sensor signaling ready to send, ignore them.
        // Each bit is preceeded by a state change to mark its beginning, ignore it too.
        if( (Toggles > 2) && (Toggles % 2 == 0)){
            // Each array element has 8 bits.  Shift Left 1 bit.
            data[ BitCnt / 8 ] <<= 1;
            // A State Change > 35 µS is a '1'.
            if(uSec>35) data[ BitCnt/8 ] |= 0x00000001;

            BitCnt++;
        }
    }

    // Read 40 bits. (Five elements of 8 bits each)  Last element is a checksum.
    if((BitCnt >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        Valid= true;
        Hum  = (float)((data[0] << 8) + data[1]) / 10.0;
        Temp = (float)((data[2] << 8) + data[3]) / 10.0;
        if(data[2] & 0x80)  Temp *= -1;         // Negative Sign Bit on.
        if(Fh){ Temp *= 1.8; Temp += 32.0; }    // Convert to Fahrenheit
    }
    else {                                      // Data Bad, use cached values.
        Valid= false;
        Hum  = 0.0;
        Temp = 0.0;
    }
}
//------------------------------------------------------------------------------------
