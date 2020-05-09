#include <stdio.h>
#include "hardware.h"
#define WDTCTL_               0x0120    /* Watchdog Timer Control */
//#define WDTHOLD             (0x0080)
//#define WDTPW               (0x5A00)


extern void VRASED (uint8_t *challenge, uint8_t *response, uint8_t operation); 

extern void my_memset(uint8_t* ptr, int len, uint8_t val);

extern void my_memcpy(uint8_t* dst, uint8_t* src, int size);


//--------------------------------------------------//
//                 tty_putc function                 //
//            (Send a byte to the UART)             //
//--------------------------------------------------//
int tty_putc (int txdata) {

  // Wait until the TX buffer is not full
  while (UART_STAT & UART_TX_FULL);

  // Write the output character
  UART_TXD = txdata;

  return 0;
}

//--------------------------------------------------//
//        UART RX interrupt service routine         //
//         (receive a byte from the UART)           //
//--------------------------------------------------//
volatile char rxdata;

wakeup interrupt (UART_RX_VECTOR) INT_uart_rx(void) {

  // Read the received data
  rxdata = UART_RXD;

  // Clear the receive pending flag
  UART_STAT = UART_RX_PND;

  P3OUT = rxdata;

  // Exit the low power mode
  //LPM0_EXIT;
}

void init_uart() {
    UART_BAUD = BAUD;                   // Init UART
    UART_CTL  = UART_EN | UART_IEN_RX;
}

void init_gpio() {
  // P3 is used for outputs and is connected to the LEDs
    P3DIR = 0xFF;
    P3OUT = 97;
  // P1 is used for input p1[0] is connected to btnR (T17 on basys3)
}

int main() {
  uint8_t challenge[32];
  uint8_t response[32];
  my_memset(challenge, 32, 1);

//  uncomment for atomicity violation:
//  __asm__ volatile("br #0xa008" "\n\t");

//  uncomment for exclusive stack violation:
//  volatile int *a = 0x2500;
//  volatile int b = *a;  

//  uncomment for key AC violation:
//  volatile int *a = 0x6A00;
//  volatile int b = *a;  
  uint32_t* wdt = (uint32_t*)(WDTCTL_);
  *wdt = WDTPW | WDTHOLD;

	init_uart();
    init_gpio();


  P3OUT  =  0x01;
  volatile uint32_t count = 0; 
  volatile uint8_t buffer = 0;
        
  while (1) {
    // if btnR is pressed run attestation 
    if (P3OUT % 10 == 0) {
        buffer = P3OUT;
    	if (P3OUT % 50 == 0) {
			// proof of reset
	        P3OUT = 0xFF;
			VRASED(challenge, response, 1);
		} else {
			//regular attestation
	        P3OUT = 0xFF;
			VRASED(challenge, response, 0);
		}

        P3OUT = buffer;
    }
     while (count < 3000000) {
        count ++;    
     }
     count = 0;
     P3OUT++;

  }

  __asm__ volatile("br #0xffff" "\n\t");
  return 0;
}
