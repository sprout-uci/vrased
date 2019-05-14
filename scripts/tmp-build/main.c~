#include <stdio.h>
#include "hardware.h"
#define WDTCTL_               0x0120    /* Watchdog Timer Control */
//#define WDTHOLD             (0x0080)
//#define WDTPW               (0x5A00)

extern void VRASED (uint8_t *challenge, uint8_t *response); 

extern void my_memset(uint8_t* ptr, int len, uint8_t val);

extern void my_memcpy(uint8_t* dst, uint8_t* src, int size);

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

  // P3 is used for outputs and is connected to the LEDs
  P3DIR  =  0xFF;
  // P1 is used for input p1[0] is connected to btnR (T17 on basys3)

  //VRASED(challenge, response);
  //__asm__ volatile("br #0xffff" "\n\t");

  P3OUT  =  0x00;
  uint32_t count = 0; 
  volatile uint8_t buffer = 0;
        
  while (1) {
     while (count < 3000000) {
        count ++;    
     }
     count = 0;
     P3OUT++;
    // if btnR is pressed run attestation 
    if (P3OUT % 10 == 0) {
        buffer = P3OUT;
        P3OUT = 0xFF;

        VRASED(challenge, response);
        count = 0;

        P3OUT = buffer;
    }

  }
 
  __asm__ volatile("br #0xffff" "\n\t");
  return 0; 
}
