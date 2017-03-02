#include <msp430.h>
#include <stdint.h>
#include <intrinsics.h>
#define LCDMEMS 6
volatile unsigned char* LCDP[] = {&LCDM10,&LCDM6,&LCDM4,&LCDM19,&LCDM15,&LCDM8,&LCDM8,&LCDM8,&LCDM8,&LCDM15,&LCDM19,&LCDM4,&LCDM6,&LCDM10,&LCDM10,&LCDM10};
volatile unsigned char LCD[] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x04};

void onboard_seg_display_init(void)
{
    PJSEL0 = BIT4 | BIT5;                   // For LFXT

    LCDCPCTL0 = 0xFFD0;		// Init. LCD segments 4, 6-15
    LCDCPCTL1 = 0xF83F;		// Init. LCD segments 16-21, 27-31
    LCDCPCTL2 = 0x00F8; 	// Init. LCD segments 35-39

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do
    {
      CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
      SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory

    LCDCCTL0 |= LCDON;
}

void delay(volatile unsigned del) {
	while (--del > 0);             // Count down until the delay counter reaches 0
}

void main(void) {

    WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer

    onboard_seg_display_init();     // Init the LCD

    // init. external buttons
    P2DIR &= ~BIT0;
    P2REN |=  BIT0;
    P2OUT |=  BIT0;

    P9DIR &= ~BIT1;
    P9REN |=  BIT1;
    P9OUT |=  BIT1;

    P9DIR &= ~BIT2;
    P9REN |=  BIT2;
    P9OUT |=  BIT2;

    P9DIR &= ~BIT4;
    P9REN |=  BIT4;
    P9OUT |=  BIT4;

    P1DIR &= ~BIT1;
    P1REN |=  BIT1;
    P1OUT |=  BIT1;

	/*P9DIR &= 0xE9;
	P9REN |= 0x16;
	P9OUT |= 0x16; */

    int head = 15;
    volatile unsigned del = 30000;
    unsigned char but = 0x00;
    unsigned char dir = 0x00;
    unsigned char len = 0x01;
    for (;;) {
    	if(!(P2IN & BIT0)){
    		if(!(but &= BIT0)&&(del < 60000)){ //min speed
    		    but |= BIT0;
    			del += 10000; //decrease speed
    		 }
    	}
    	else
    		but &= ~BIT0; //simple debounce
    	if(!(P9IN & BIT1)){
    	 	 if(!(but &= BIT1)&&(del > 10000)){ //max speed
    			but |= BIT1;
    			del -= 10000; //increase speed
    	 	 }
      	}
    	 else
    		but &= ~BIT1; //simple debounce

    	if(!(P1IN & BIT1)){
    	 	 if(!(but &= BIT2)){
    			but |= BIT2;
    			if(dir == 0){
    				head -= len-1; //flip snake direction
    				dir = BIT0; }
    			else {
    				dir = 0;
    				head += len-1; } //flip snake direction
    	 	 }
      	}
    	 else
    		but &= ~BIT2; //simple debounce

    	if(!(P9IN & BIT2)){
    	    if(!(but &= BIT3) && (len < 15)){ //increase length
    	    	but |= BIT3;
    	    	len++;
    	    }
    	}
    	else
    		but &= ~BIT3;

    	if(!(P9IN & BIT4)){
    	    if(!(but &= BIT4) && (len > 1)){ //decrease length
    	    	but |= BIT4;
    	    	if(dir ==0)
    	    		*LCDP[(head-len+1)%16] &= ~(LCD[(head-len+1)%16]); //for cw
    	    	else
    	    		*LCDP[(head+len-1)%16] &= ~(LCD[(head+len-1)%16]); //for ccw
    	    	len--;
    	    }
    	}
    	else
    		but &= ~BIT4;
    	//Determine to move in each direction.
    	if(dir == 0){ //cw
    		head++; //increase position
    		*LCDP[head%16] |= LCD[head%16]; //advance head
    		*LCDP[(head-len)%16] &= ~(LCD[(head-len)%16]); //remove old head
    		//head++;
    		if(head > 31) head -= 16; //prevent eventual overflow
    	}
    	else{ //ccw
    		head--; //decrease position
    		*LCDP[head%16] |= LCD[head%16]; //advance head
    		*LCDP[(head+len)%16] &= ~(LCD[(head+len)%16]); //remove old head
    		if(head < 16) head += 16; //prevent eventual underflow
    		//head--;
    	}
    	delay(del); 
    }
}
