#include <pololu/orangutan.h>
 
// *** UNCOMMENT ONE OF THE FOLLOWING PREPROCESSOR DIRECTIVES ***
// (the remaining two should be commented out)
#define WORKING_CORRECTLY  // this is the right way to use playMode()
//#define ALWAYS_AUTOMATIC // mode is always PLAY_AUTOMATIC, timing is inaccurate
//#define ALWAYS_CHECK     // mode is always PLAY_CHECK, delays interrupt the sequence

#include <avr/pgmspace.h>
const char rhapsody[] PROGMEM = "O6 T40 L16 d#<b<f#<d#<f#<bd#f#"
  "T80 c#<b-<f#<c#<f#<b-c#8"
  "T180 d#b<f#d#f#>bd#f#c#b-<f#c#f#>b-c#8"
 //"c>c#<c#>c#<b>c#<c#>c#c>c#<c#>c#<b>c#<c#>c#"
 // "c>c#<c#>c#<b->c#<c#>c#c>c#<c#>c#<b->c#<c#>c#"
 // "c>c#<c#>c#f>c#<c#>c#c>c#<c#>c#f>c#<c#>c#"
 // "c>c#<c#>c#f#>c#<c#>c#c>c#<c#>c#f#>c#<c#>c#d#bb-bd#bf#d#c#b-ab-c#b-f#d#"
 ;


void play_winning_music()
{
  TCCR0A = 0;         // configure timer0 to run at 78 kHz
  TCCR0B = 0x04;      // and overflow when TCNT0 = 256 (~3 ms)
  play_from_program_space(rhapsody);
  while(is_playing())
  {
    // allow the sequence to keep playing automatically through the following delays
#ifndef ALWAYS_CHECK
    play_mode(PLAY_AUTOMATIC);
#else
    play_mode(PLAY_CHECK);
#endif
    clear();
    lcd_goto_xy(0, 0);
    print("I AM");
    lcd_goto_xy(0, 1);
    print("GOOD!");
 
    int i;
    for (i = 0; i < 8; i++)
    {
#ifdef ALWAYS_CHECK
      play_check();
#endif
      red_led(1);
      delay_ms(500);
      red_led(0);
      delay_ms(500);
    }
    // turn off automatic playing so that our time-critical code won't be interrupted by
    // the buzzer's long timer1 interrupt.  Otherwise, this interrupt could throw off our
    // timing measurements.  Instead, we will now use playCheck() to keep the sequence
    // playing in a way that won't throw off our measurements.
#ifndef ALWAYS_AUTOMATIC
    play_mode(PLAY_CHECK);
#endif
    unsigned char maxTime = 0;
    for (i = 0; i < 8000; i++)
    {
      TCNT0 = 0;
      while (TCNT0 < 20)    // time for ~250 us
        ;
      if (TCNT0 > maxTime)
        maxTime = TCNT0;    // if the elapsed time is greater than the previous max, save it
#ifndef ALWAYS_AUTOMATIC
      play_check();   // check if it's time to play the next note and play it if so
#endif
    }
  }
}
