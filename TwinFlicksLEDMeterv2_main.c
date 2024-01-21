/*-----------------------------------------------------------------------------
	TwinFlicksLEDMeterv2_main.c
-------------------------------------------------------------------------------
	Flickinger Audio
	Twin Flicks LED Meter v2
	David McDonald
	2-5-2018

	Purpose: Takes an input voltage from output of preamp circuit and converts
	to LED metering on 8 SMD led array. Also senses pushbutton mode pushes and
	changes between metering modes: PPM, VU, Peak Monitor, Peak Hold.
	Inputs: Voltage from P0.1, P0.7
	Outputs: 3.3V to 8 LED meter on pins P1.6, P1.5, P1.4, P1.3, P1.2

-------------------------------------------------------------------------------
*/

/************************************************************
 STANDARD BITS
************************************************************/

#define BIT0                   (0x0001)
#define BIT1                   (0x0002)
#define BIT2                   (0x0004)
#define BIT3                   (0x0008)
#define BIT4                   (0x0010)
#define BIT5                   (0x0020)
#define BIT6                   (0x0040)
#define BIT7                   (0x0080)
#define BIT8                   (0x0100)
#define BIT9                   (0x0200)
#define BITA                   (0x0400)
#define BITB                   (0x0800)
#define BITC                   (0x1000)
#define BITD                   (0x2000)
#define BITE                   (0x4000)
#define BITF                   (0x8000)

#define NOLEDS 8
#define BUTTONPUSH() (!(P0_B1)) //Button Push occurs when P0.1IN=0
#define FADETIME 50

#include <SI_EFM8BB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
#include <stdint.h>

uint8_t clipresult=0;
uint8_t peakresult=0;


void CharliePlexLEDS(uint8_t Led){			//Switch-case statement for each LED On-state
	switch(Led){
	case NOLEDS:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__OPEN_DRAIN | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__HIGH | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 0:
		P1MDOUT = P1MDOUT_B6__PUSH_PULL | P1MDOUT_B5__PUSH_PULL | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__LOW | P1_B4__HIGH | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 1:
		P1MDOUT = P1MDOUT_B6__PUSH_PULL | P1MDOUT_B5__PUSH_PULL | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__LOW | P1_B5__HIGH | P1_B4__HIGH | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 2:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__PUSH_PULL | P1MDOUT_B4__PUSH_PULL | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__LOW | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 3:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__PUSH_PULL | P1MDOUT_B4__PUSH_PULL | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__LOW | P1_B4__HIGH | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 4:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__OPEN_DRAIN | P1MDOUT_B4__PUSH_PULL | P1MDOUT_B3__PUSH_PULL | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__HIGH | P1_B3__LOW | P1_B2__HIGH;
		break;

	case 5:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__OPEN_DRAIN | P1MDOUT_B4__PUSH_PULL | P1MDOUT_B3__PUSH_PULL | P1MDOUT_B2__OPEN_DRAIN;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__LOW | P1_B3__HIGH | P1_B2__HIGH;
		break;

	case 6:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__OPEN_DRAIN | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B3__PUSH_PULL | P1MDOUT_B2__PUSH_PULL;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__HIGH | P1_B3__HIGH | P1_B2__LOW;
		break;

	case 7:
		P1MDOUT = P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B5__OPEN_DRAIN | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B3__PUSH_PULL | P1MDOUT_B2__PUSH_PULL;
		P1 = P1_B6__HIGH | P1_B5__HIGH | P1_B4__HIGH | P1_B3__LOW | P1_B2__HIGH;
		break;
	}
}

uint8_t ModeSelect(){
	static uint8_t mode=0;
	uint16_t x;


	if (BUTTONPUSH()) {			//Simple single Button Push Test LEDS
		x=3000;
		do{
			x--;
		}
		while(x); 				//software delay for switch de-bounce
		if(BUTTONPUSH())
		{
			while(BUTTONPUSH());
			if(clipresult){
				clipresult=0;
				peakresult=0;

			}
			else{
			mode++;
			mode&=0x03;
			}
		}
	}

	return mode;
}

void TestLoop(){					// Test loop for LED testing
	uint16_t x=0;
	uint8_t lednum;

	do {
		for (lednum=0;lednum<8;lednum++){
			CharliePlexLEDS(lednum);
			x=30000;
			do{
				x--;
				if(BUTTONPUSH()) break;
			}
			while(x); //software delay for led
			if(BUTTONPUSH()) break;
		}
		for (lednum=0;lednum<8;lednum++){
			CharliePlexLEDS(7-lednum);
			x=30000;
			do{
				x--;
				if(BUTTONPUSH()) break;
			}
			while(x); //software delay for led
			if(BUTTONPUSH()) break;
		}
	} while (!BUTTONPUSH());

}

void initADC(){
	ADC0MX = ADC0MX_ADC0MX__ADC0P7;														//Set Multiplexer to read from P0.7
	ADC0CN0 = ADC0CN0_ADEN__ENABLED | ADC0CN0_ADCM__ADBUSY;								//Enable ADC
	REF0CN = REF0CN_IREFLVL__2P4 | REF0CN_REFSL__INTERNAL_VREF;							//Internal reference voltage set to 2.4V
	ADC0CF = ADC0CF_AD8BE__NORMAL | ADC0CF_ADGN__GAIN_1 | ADC0CF_ADTM__TRACK_NORMAL;
}

uint16_t readADC(){
	uint16_t high=0 , low=0, adc_result;
	initADC();
	ADC0CN0 |= ADC0CN0_ADBUSY__SET;				// Start ADC conversion
	while(!(ADC0CN0 &=~ ADC0CN0_ADINT__SET));	// Wait until conversion is done
	high = ADC0H;								// high variable set as high byte
	low = ADC0L;								// low variable set as low byte
	adc_result = (high<<8)|low;					// Combines High and Low bytes and to return
	return adc_result;
}

uint8_t EncodeLED(uint8_t mode, uint16_t adc_result){		// Input adc_result & mode. Output ledresult, each bit represents an LED
	uint8_t ledresult=0;
	static uint16_t count=0;
	switch(mode){
	case 0:							// Default RMS meter mode
		if(adc_result>19){
			ledresult|=BIT0;
			if(adc_result>30) {
				ledresult|=BIT1;
				if(adc_result>49) {
					ledresult|=BIT2;
					if(adc_result>79) {
						ledresult|=BIT3;
						if(adc_result>126) {
							ledresult|=BIT4;
							if(adc_result>201) {
								ledresult|=BIT5;
								if(adc_result>322) {
									ledresult|=BIT6;
									if(adc_result>573) {
										ledresult|=BIT7;
									}
								}
							}
						}
					}
				}

			}
		}
		break;

	case 1:							// Peak hold w/ Clip flash
		if(adc_result>19){
			ledresult|=BIT0;
			if(peakresult<BIT0){
				peakresult=BIT0;
				count=FADETIME;
			}
			if(adc_result>30) {
				ledresult|=BIT1;
				if(peakresult<BIT1){
					peakresult=BIT1;
					count=FADETIME;
				}
				if(adc_result>49) {
					ledresult|=BIT2;
					if(peakresult<BIT2){
						peakresult=BIT2;
						count=FADETIME;
					}
					if(adc_result>79) {
						ledresult|=BIT3;
						if(peakresult<BIT3){
							peakresult=BIT3;
							count=FADETIME;
						}
						if(adc_result>126) {
							ledresult|=BIT4;
							if(peakresult<BIT4){
								peakresult=BIT4;
								count=FADETIME;
							}
							if(adc_result>201) {
								ledresult|=BIT5;
								if(peakresult<BIT5){
									peakresult=BIT5;
									count=FADETIME;
								}
								if(adc_result>322) {
									ledresult|=BIT6;
									if(peakresult<BIT6){
										peakresult=BIT6;
										count=FADETIME;
									}
									if(adc_result>573) {
										ledresult|=BIT7;
										if(peakresult<BIT7){
											peakresult=BIT7;
											count=FADETIME;
										}
										if(adc_result>910) {
												clipresult=1;
										}
									}
								}
							}
						}
					}
				}
			}

		}
		if(clipresult==1){
			peakresult|=BIT7;
		}
		else {
		if(count>0){
			count--;
		}
		else {
			peakresult>>=1;
			count=FADETIME;
		}
		}
			ledresult|=peakresult;

		break;

	case 2:							// RMS top only
		if(adc_result>19){
			ledresult=BIT0;
		}
		if(adc_result>30){
			ledresult=BIT1;
		}
		if(adc_result>49) {
			ledresult=BIT2;
		}
		if(adc_result>79) {
			ledresult=BIT3;
		}
		if(adc_result>126) {
			ledresult=BIT4;
		}
		if(adc_result>201) {
			ledresult=BIT5;
		}
		if(adc_result>322) {
			ledresult=BIT6;
		}
		if(adc_result>573) {
			ledresult=BIT7;
		}
		break;

	case 3:
		TestLoop();				// For LED testing
		ledresult=0;
		break;
	}
	return ledresult;
}

void DisplayLEDs(ledresult){		// Input ledresult byte, runs CharliePlexLEDs internally to display
	uint16_t x=0;
	uint8_t y;
	for(y=0;y<8;y++){
		if(ledresult&(1<<y)){
			CharliePlexLEDS(y);
		}
		else CharliePlexLEDS(NOLEDS);
		x=300;
		do{
			x--;
		}
		while(x);
	}
}

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------

void main (void)	{					// Call hardware initialization routine
	uint16_t x=0,y=0, adc_result;
	uint8_t mode, ledresult;
	enter_DefaultMode_from_RESET();

	while (1){
		initADC();
		adc_result=readADC();
		mode=ModeSelect();
		ledresult=EncodeLED(mode, adc_result);
		DisplayLEDs(ledresult);
		DisplayLEDs(ledresult);
		DisplayLEDs(ledresult);
		}


		/*		if (BUTTONPUSH()) {			//Simple single Button Push Test LEDS
			x=3000;
			do{
				x--;
			}
			while(x); //software delay for switch de-bounce
			if (BUTTONPUSH(){
			TestLoop();
			}
			else CharliePlexLEDS(NOLEDS);			// For LED testing

		}
		 */
	}
