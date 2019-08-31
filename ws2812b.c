/* -------------------------------------------------------
                           ws2812b.c

     Softwaremodul zur Ansteuerung von WS2812b Leucht-
     dioden, Leuchtdiodenreihen

     MCU      :  STM8S103F3
     Hardware :  WS2812b Leuchtdiodenreihe
     Takt     :  interner Takt 16 MHz
     Compiler :  SDCC 3.5

     88.05.2017  R. Seelig
   ------------------------------------------------------ */

#include "ws2812b.h"

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

/* ----------------------------------------------------------
                          ws_reset

     setzt die Leuchtdiodenreihe (fuer einen folgenden
     Datentransfer zu den Leuchtdioden) zurueck
   ---------------------------------------------------------- */
void ws_reset(void)
{
  __asm
    bres gpio_asm, #gpio_pin            // Datenpin des LED-Strangs auf 0 setzen
  __endasm;

  delay_us(100);
}

/* -------------------------------------------------------------
      ws_showarray

      (leider) in Maschinensprache, weil ich keinen anderen
      Weg gefunden habe, mittels SDCC 350 Nanosekundenimpulse
      zu erzeugen.

      Der SDCC legt seine Parameter auf den Stack in der
      Reihenfolge von hinten nach vorne ab, hier also:

      anz (2 Byte), *ptr (2 Byte), Ruecksprungadresse (2 Byte)

      In wiefern andere Compiler den Stack belegen weiss ich in
      Ermangelung eines anderen Compilers nicht und von daher
      wird diese Funktion wohl nur mit SDCC (Version 3.5)
      funktionieren.

      Uebergabe:
                *ptr  : Zeiger auf ein Array, das angezeigt
                        werden soll
                anz   : Anzahl der anzuzeigenden LED's
   ------------------------------------------------------------- */
void ws_showarray(uint8_t *ptr, int anz)
{
  ptr;
  anz;
  __asm

    push a
    pushw x
    pushw y                   // auf dem Stack liegt (in dieser Reihenfolge)
                              // int anz                         ( 2 Byte )
                              // Zeiger ptr                      ( 2 Byte )
                              // Ruecksprungadresse              ( 2 Byte )
                              // a                               ( 1 Byte )
                              // x                               ( 2 Byte )
                              // y                               ( 2 Byte )

    ldw y,(0x0a, sp)          // 10. Position auf dem Stack = int anz

    // anz= (anz*3)-1         => jede LED 3 Byte Speicherbedarf erfordert n*3
    //                           Sendevorgaenge;
    addw y,(0x0a, sp)
    addw y,(0x0a, sp)
    decw y

    ldw x,(0x08, sp)          //  8. Position = *ptr

    array_loop:

      pushw y                 // Anzahl der noch zu sendenden Bytes auf den Stack

      ld a,(x)                // a enthaellt den ersten Wert, auf den PTR zeigt
      ldw y,#0x0007           // 8 Shifts fuer 1 Byte

      loop0:

        rlc a                 // ist hoechstwertigstes gesetzt
        jrc sendhi            // dann eine 1 senden
      sendlo:
        // 0 - senden
        bset gpio_asm, #gpio_pin
        nop
        nop
        bres gpio_asm, #gpio_pin
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop

        jra loop0_end

      sendhi:
        // 1 - senden
        bset gpio_asm, #gpio_pin
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        bres gpio_asm, #gpio_pin
        nop
        nop

      loop0_end:
        decw y
      jrpl loop0

      incw x
      popw y
      decw y
    jrpl array_loop

    popw y
    popw x
    pop a

  __endasm;
}

/* ----------------------------------------------------------
                          ws_clrarray
     loescht ein LED Anzeigearray

     Parameter:
                   *ptr : Zeiger auf ein Array, das
                          geloescht werden soll
                    anz : Anzahl der LEDs, die angezeigt
                          werden sollen
   ---------------------------------------------------------- */
void ws_clrarray(uint8_t *ptr, int anz)
{
  int i;

  for (i= 0; i< (anz*3); i++)
  {
    *ptr = 0x00;
    ptr++;
  }
}
