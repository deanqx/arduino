#include "can.c"
#include "hal.h"
#include "usart.h"

int main(void) {
  hal_init();
  uart0_puts("hello\r\n");

  can_init();
  // WRITE
  // 11 bit address
  // 2 byte data
  // RTS

  while (1) {
  }
}
