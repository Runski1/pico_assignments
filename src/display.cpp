#include "FreeMono12pt7b.h"
#include "PicoI2CDevice.h"
#include "PicoSPIBus.h"
#include "PicoSPIDevice.h"
#include "epd154.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "rgb_palette.h"
#include "ssd1306.h"
#include "st7789nobuf.h"
#include <pico.h>
#include <pico/time.h>
#include <stdio.h>

static const uint8_t bitmap[] = {
    0x02, 0x01, 0x06, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xa1, 0x71, 0x70, 0x38, 0x1c, 0x1c, 0x9c, 0x0c, 0x0c, 0x0f, 0x1c, 0x09,
    0x94, 0x86, 0x01, 0x02, 0x02, 0x80, 0x00, 0x00, 0x02, 0x01, 0x01, 0x04,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x02, 0x01, 0x0d, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x76, 0xf9, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0xc0,
    0x10, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xfb, 0x79, 0xf9, 0xf9, 0x7f,
    0x3f, 0x39, 0x3c, 0x7c, 0x79, 0x79, 0x3c, 0x1e, 0x12, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x40, 0x10, 0x00, 0x00,
    0x00, 0xc0, 0xa4, 0xc0, 0xc0, 0xc0, 0xc2, 0xe0, 0xe0, 0xe7, 0xfc, 0x3c,
    0x7d, 0x79, 0x7c, 0x1f, 0x16, 0x70, 0x00, 0x04, 0x06, 0x00, 0x00, 0x80,
    0xf8, 0xf0, 0xe0, 0xc0, 0xc8, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x78, 0x7c, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7f, 0x7f, 0x7c, 0x60, 0x68, 0x7c, 0x70, 0x70, 0x60, 0x60, 0x30, 0x18,
    0x3e, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f,
    0x7e, 0x7e, 0x7c};

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

  // Blinky stuff
  uint led_pin1 = 20;
  uint led_pin2 = 21;
  uint led_pin3 = 22;
  struct repeating_timer timer1;
  struct repeating_timer timer2;
  struct repeating_timer timer3;
  setup_blink(&led_pin1, 125, &timer1);
  setup_blink(&led_pin2, 250, &timer2);
  setup_blink(&led_pin3, 500, &timer3);

  // Display init
  auto bus = std::make_shared<PicoI2CBus>(1, 14, 15);
  auto dev = std::make_shared<PicoI2CDevice>(bus, 0x3C);
  ssd1306 display(dev);

  display.fill(0);
  uint i = 0;
  display.text("Time: 0s", 0, 0, 1);
  mono_vlsb rb(bitmap, 39, 39); // basically a frame buffer with extra fries
  display.blit(rb, 0, 10);
  display.show();

  for (;;) {
    sleep_ms(1000);
    char str[128 / 8]; // max string to fit on one row
    sprintf(str, "Time: %ds", ++i);
    display.rect(0, 0, 128, 8, 0, 1);
    display.text(str, 0, 0, 1);
    display.show();
  }
  return 0;
}
