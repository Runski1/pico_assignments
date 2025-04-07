#include "pico/stdlib.h"
#include <pico.h>
#include <pico/time.h>
#include <stdio.h>

bool blink_callback(struct repeating_timer *t) {
  uint pin = *(uint *)t->user_data;
  gpio_put(pin, !gpio_get(pin));
  return true;
}

void setup_blink(uint *pin, int timer_ms, struct repeating_timer *t) {
  gpio_init(*pin);
  gpio_set_dir(*pin, GPIO_OUT);
  gpio_put(*pin, 0);
  add_repeating_timer_ms(timer_ms, blink_callback, pin, t);
}

int main() {
  stdio_init_all();

  uint led_pin1 = 20;
  uint led_pin2 = 21;
  uint led_pin3 = 22;
  struct repeating_timer timer1;
  struct repeating_timer timer2;
  struct repeating_timer timer3;
  setup_blink(&led_pin1, 125, &timer1);
  setup_blink(&led_pin2, 250, &timer2);
  setup_blink(&led_pin3, 500, &timer3);
  uint i = 0;
  for (;;) {
    sleep_ms(1000);
    printf("%d\n", ++i);
  }
  return 0;
}
