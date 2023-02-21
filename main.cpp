/* Description: McLab15_SensorDigi_OS6_tk1
 * Hardware: L432KC or similar.
 * For testing a microwave "radar" sensor 
 * DFRobot Digital Microwave Sensor SEN0192, 
 * turn the sensitivity trimmer in about middle.
 * Or a similar digital output presence sensor like
 * Seeed Studio GROVE Digital PIR Motion Sensor. 
 * Connect:
 * L432KC D4 -- sensor output (Out)
 * GND rail  -- senror GND (GND)
 * 3.3V rail -- sensor Power (3.3V or 5V)
 * 
 * L432KC LD3 is permanently connected to D13
 * and can be named LED1 in the code.
 * Operating system OS 6.0...6.16
 * Timo Karppinen 8.2.2023  Apache-2.0
 */

#include "mbed.h"
// Sampling rate in milliseconds. The radar needs a sampling
// rate of a few millisenconds, the PIR a few thousand milliseconds. 
#define SAMPLING_RATE     2ms
// Output change every n seconds
#define OUTPUT_UPDATE     1s

float mwSampleMean = 0.0;  // from 0.0 to 1.0
int mwSampleMeanInt = 0;   // from 0 to 100

// An other thread for timing the sensor signal sampling
Thread thread2; 

void sample(){
    // Initialise the digital pin D4 as a digital input
   
    DigitalIn mwSen(D2);    //  D4
    // Variables for the microwave signal samples
    int aSample;
    static int mwSamples[100];   // false 0 or true 1 
    static int mwSampleSum = 0;  // better initialize with 0
    static int index =0;
    
    while (true) {
        aSample = mwSen.read();    // inverted !mwSen.read(); for the radar
                                // not inverterd mwSen.read(); for the PIR
        mwSamples[index] = aSample;
        // Printing for first tests. Sampling 100 ms, update 10 s
        // Comment the print line. Change sample and out update periods
        // into about a few millisseconds and one second. 
        //printf("mwSample%d at%d\n", mwSamples[index], index);
        if(index < 99){
            mwSampleSum = mwSampleSum + mwSamples[index] - mwSamples[index+1];
        }
        if(index >= 99){
            mwSampleSum = mwSampleSum + mwSamples[99] - mwSamples[0];
            index = -1;
        }
        index = index +1;
        mwSampleMean = ((float)mwSampleSum)/100;
        ThisThread::sleep_for(SAMPLING_RATE);
    }
}

int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut ledA(LED1);
    int ledAState = false;

    thread2.start(callback(sample));

    while (true) {
        ledA.write(ledAState);
        mwSampleMeanInt = (int)(100*mwSampleMean);
        if(mwSampleMeanInt > 15){       // please test with different values!
            ledAState = true;
        }  else{
            ledAState = false;
        }
        ledA.write(ledAState);
        printf("MW Sensor mean reading  %d Detected %d\n", mwSampleMeanInt, ledAState);
        ThisThread::sleep_for(OUTPUT_UPDATE);
    }
}



