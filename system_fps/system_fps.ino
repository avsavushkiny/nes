#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define PIN_LED 8

U8G2_ST7565_ERC12864_F_4W_SW_SPI u8g2(U8G2_R0, 18, 19, 17, 16, 20);

typedef u8g2_uint_t u8g_uint_t;

#define SECONDS 10
uint8_t flip_color = 0;
uint8_t draw_color = 1;

void draw_set_screen(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.setColorIndex(flip_color);
  u8g2.drawBox( 0, 0, u8g2.getWidth(), u8g2.getHeight() );
}

void draw_clip_test(void) {
  u8g_uint_t i, j, k;
  char buf[3] = "AB";
  k = 0;
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_6x10_tf);
  
  for( i = 0; i  < 6; i++ ) {
    for( j = 1; j  < 8; j++ ) {
      u8g2.drawHLine(i-3, k, j);
      u8g2.drawHLine(i-3+10, k, j);
      
      u8g2.drawVLine(k+20, i-3, j);
      u8g2.drawVLine(k+20, i-3+10, j);
      
      k++;
    }
  }
  u8g2.setFontDirection(0);
  u8g2.drawStr(0-3, 50, buf);
  u8g2.setFontDirection(2);
  u8g2.drawStr(0+3, 50, buf);
  
  u8g2.setFontDirection(0);
  u8g2.drawStr(u8g2.getWidth()-3, 40, buf);
  u8g2.setFontDirection(2);
  u8g2.drawStr(u8g2.getWidth()+3, 40, buf);

  u8g2.setFontDirection(1);
  u8g2.drawStr(u8g2.getWidth()-10, 0-3, buf);
  u8g2.setFontDirection(3);
  u8g2.drawStr(u8g2.getWidth()-10, 3, buf);

  u8g2.setFontDirection(1);
  u8g2.drawStr(u8g2.getWidth()-20, u8g2.getHeight()-3, buf);
  u8g2.setFontDirection(3);
  u8g2.drawStr(u8g2.getWidth()-20, u8g2.getHeight()+3, buf);

  u8g2.setFontDirection(0);

}

void draw_char(void) {
  char buf[2] = "@";
  u8g_uint_t i, j;
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_6x10_tf);
  j = 8;
  for(;;) {
    i = 0;
    for(;;) {
      u8g2.drawStr( i, j, buf);
      i += 8;
      if ( i > u8g2.getWidth() )
        break;
    }
    j += 8;
    if ( j > u8g2.getHeight() )
      break;
  }
  
}

void draw_pixel(void) {
  u8g_uint_t x, y, w2, h2;
  u8g2.setColorIndex(draw_color);
  w2 = u8g2.getWidth();
  h2 = u8g2.getHeight();
  w2 /= 2;
  h2 /= 2;
  for( y = 0; y < h2; y++ ) {
    for( x = 0; x < w2; x++ ) {
      if ( (x + y) & 1 ) {
        u8g2.drawPixel(x,y);
        u8g2.drawPixel(x,y+h2);
        u8g2.drawPixel(x+w2,y);
        u8g2.drawPixel(x+w2,y+h2);
      }
    }
  }
}

void draw_line(void) {
  u8g2.setColorIndex(draw_color);
  u8g2.drawLine(0,0, u8g2.getWidth()-1, u8g2.getHeight()-1);
}

// returns unadjusted FPS
uint16_t execute_with_fps(void (*draw_fn)(void)) {
  uint16_t FPS10 = 0;
  uint32_t time;
  
  time = millis() + SECONDS*1000;
  
  // picture loop
  do {
    u8g2.clearBuffer();
    draw_fn();
    u8g2.sendBuffer();
    FPS10++;
    flip_color = flip_color ^ 1;
  } while( millis() < time );
  return FPS10;  
}

const char *convert_FPS(uint16_t fps) {
  static char buf[6];
  strcpy(buf, u8g2_u8toa( (uint8_t)(fps/10), 3));
  buf[3] =  '.';
  buf[4] = (fps % 10) + '0';
  buf[5] = '\0';
  return buf;
}

void show_result(const char *s, uint16_t fps) {
  // assign default color value
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_8x13B_tf);
  u8g2.clearBuffer();
  u8g2.drawStr(0,12, s);
  u8g2.drawStr(0,24, convert_FPS(fps));
  u8g2.sendBuffer();
}

void setup(void) {
  /* U8g2 Project: SSD1306 Test Board */
  //pinMode(10, OUTPUT);
  //pinMode(9, OUTPUT);
  //digitalWrite(10, 0);
  //digitalWrite(9, 0);   

  /* U8g2 Project: T6963 Test Board */
  //pinMode(18, OUTPUT);
  //digitalWrite(18, 1);

  /* U8g2 Project: KS0108 Test Board */
  //pinMode(16, OUTPUT);
  //digitalWrite(16, 0);  

  /* U8g2 Project: LC7981 Test Board, connect RW to GND */
  //pinMode(17, OUTPUT);
  //digitalWrite(17, 0);  

  u8g2.begin();
  // flip screen, if required
  // u8g2.setRot180();
  
  // assign default color value
  draw_color = 1;         // pixel on
  
  //u8g2.setBusClock(2000000);

  u8g2.setContrast(0);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, 1);
}

void loop(void) {
  uint16_t fps;
  fps = execute_with_fps(draw_clip_test);
  show_result("draw clip test", fps);
  delay(5000);
  fps = execute_with_fps(draw_set_screen);
  show_result("clear screen", fps);
  delay(5000);
  fps = execute_with_fps(draw_char);
  show_result("draw @", fps);
  delay(5000);  
  fps = execute_with_fps(draw_pixel);
  show_result("draw pixel", fps);
  delay(5000);
  fps = execute_with_fps(draw_line);
  show_result("draw line", fps);
  delay(5000);
}
