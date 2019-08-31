/* -------------------------------------------------------
                           ws2812b.h

     Header zur WS2812b Software fuer "intelligente"
     Leuchtdioden

     MCU      :  STM8S103F3
     Hardware :  WS2812b Leuchtdiodenreihe
     Takt     :  interner Takt 16 MHz
     Compiler :  SDCC 3.5

     88.05.2017  R. Seelig
   ------------------------------------------------------ */


/*
##################################################################
                      STM8S103F3P6 Pinout
##################################################################


                            ------------
UART1_CK / TIM2_CH1 / PD4  |  1     20  |  PD3 / AIN4 / TIM2_CH2 / ADC_ETR
    UART1_TX / AIN5 / PD5  |  2     19  |  PD2 / AIN3
    UART1_RX / AIN6 / PD6  |  3     18  |  PD1 / SWIM
                     NRST  |  4     17  |  PC7 / SPI_MISO
              OSCIN / PA1  |  5     16  |  PC6 / SPI_MOSI
             OSCOUT / PA2  |  6     15  |  PC5 / SPI_CLK
                Vss (GND)  |  7     14  |  PC4 / TIM1_CH4 / CLK_CCO / AIN2
                VCAP (*1)  |  8     13  |  PC3 / TIM1_CH3 /
                Vdd (+Ub)  |  9     12  |  PB4 / I2C_SCL
           TIM2_CH3 / PA3  | 10     11  |  PB5 / I2C_SDA
                            -----------

*1 :  Ist mit min. 1uF gegen GND zu verschalten

*/

#ifndef in_ws2812b
  #define in_ws2812b

  #include <stdint.h>
  #include "stm8s.h"
  #include "stm8_init.h"

/* #############################################################################
                                 Benutzerangaben
   ############################################################################# */

  /* -------------------------------------------------------------
       hier angeben, an welchem Portpin der WS2812 Strang
       angeschlossen ist (benutzten Port "entkommentieren"
     ------------------------------------------------------------- */

  // Port, an dem die LED angeschlossen ist
  // #define ws_porta
  // #define ws_portb
  // #define ws_portc
   #define ws_portd

  #define gpio_pin      5                         // Portpin an dem die LED angeschlossen ist
                                                  // hier dann PD4

/* #############################################################################
                               Ende Benutzerangaben
   ############################################################################# */

  /* -------------------------------------------------------------
       Makros zur die die Registeradressen den oben gemachten
       Angaben zuordnen
     ------------------------------------------------------------- */

  #define port_mask        ( 1 << gpio_pin )
  #if defined ws_porta

    #define ws_init()      {  PA_DDR |=   port_mask;    \
                              PA_CR1 |=   port_mask;    \
                              PA_CR2 &= ~(port_mask); }
    #define gpio_asm    0x5000                              // I/O Portadresse fuer Port A

  #elif defined ws_portb
    #define ws_init()      {  PB_DDR |=   port_mask;    \
                              PB_CR1 |=   port_mask;    \
                              PB_CR2 &= ~(port_mask); }
    #define gpio_asm    0x5005                              // I/O Portadresse fuer Port C

  #elif defined ws_portc
    #define ws_init()      {  PC_DDR |=   port_mask;    \
                              PC_CR1 |=   port_mask;    \
                              PC_CR2 &= ~(port_mask); }
    #define gpio_asm    0x500a                              // I/O Portadresse fuer Port C

  #elif defined ws_portd
    #define ws_init()      {  PD_DDR |=   port_mask;    \
                              PD_CR1 |=   port_mask;    \
                              PD_CR2 &= ~(port_mask); }
    #define gpio_asm    0x500f                              // I/O Portadresse fuer Port B

  #else
    #error "kein vorhandener Port des STM8S gewaehlt..."

  #endif

  void ws_reset(void);
  void ws_showarray(uint8_t *ptr, int anz);
  void ws_clrarray(uint8_t *ptr, int anz);

#endif
