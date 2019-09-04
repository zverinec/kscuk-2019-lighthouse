#include <stdint.h>
#include "stm8s.h"
#include "stm8_init.h"
#include "ws2812b.h"

#define LED_COUNT 32
volatile uint8_t ledbuffer[LED_COUNT * 3];

int32_t abs( int32_t x ) {
    return x < 0 ? -x: x;
}

void set_hsv( uint8_t* leds, uint8_t index, unsigned char hue, unsigned char sat, unsigned char val ) {
    int32_t h = 6LL * (int32_t)hue;
    int32_t c = (int32_t)val * (int32_t)sat;
    int32_t x = c * ( 255LL - abs( h % ( 2LL << 8LL ) - 255LL ) );
    c >>= 8;
    h >>= 8;
    x >>= 16;
    int32_t m = val - c;
    uint8_t r, g, b;
    switch ( h ) {
        case 6:
        case 0: r = c + m; g = x + m; b = m;     break;
        case 1: r = x + m; g = c + m; b = m;     break;
        case 2: r = m;     g = c + m; b = x + m; break;
        case 3: r = m;     g = x + m; b = c + m; break;
        case 4: r = x + m; g = m;     b = c + m; break;
        case 5: r = c + m; g = m;     b = x + m; break;
    }

    leds[ 3 * index + 0 ] = g;
    leds[ 3 * index + 1 ] = r;
    leds[ 3 * index + 2 ] = b;
}

void set_rgb( uint8_t* leds, uint8_t index, uint8_t r, uint8_t g, uint8_t b ) {
    leds[ 3 * index + 0 ] = g;
    leds[ 3 * index + 1 ] = r;
    leds[ 3 * index + 2 ] = b;
}

void rainbow( unsigned int progress, uint8_t brightness ) {
    for ( uint8_t i = 0; i != LED_COUNT; i++ ) {
        set_hsv( ledbuffer, i, progress + 2 * i, 255, brightness );
    }
}

void red( unsigned int progress, uint8_t brightness ) {
    progress = progress;
    for ( uint8_t i = 0; i != LED_COUNT; i++ ) {
        set_rgb( ledbuffer, i, brightness, 0, 0 );
    }
}

void green( unsigned int progress, uint8_t brightness ) {
    progress = progress;
    for ( uint8_t i = 0; i != LED_COUNT; i++ ) {
        set_rgb( ledbuffer, i, 0, brightness, 0 );
    }
}

void blue( unsigned int progress, uint8_t brightness ) {
    progress = progress;
    for ( uint8_t i = 0; i != LED_COUNT; i++ ) {
        set_rgb( ledbuffer, i, 0, 0, brightness );
    }
}

void white( unsigned int progress, uint8_t brightness ) {
    progress = progress;
    for ( uint8_t i = 0; i != LED_COUNT; i++ ) {
        set_rgb( ledbuffer, i, brightness, brightness, brightness );
    }
}

void (*modes[]) ( unsigned int, uint8_t ) = {
    red,
    green,
    blue
};

int modeCount = sizeof(modes) / sizeof(modes[0]);

uint8_t bright[] = {
    8, 11, 17, 26, 38, 56, 84, 124, 184, 255, 0
};
int brightCount = sizeof(bright) / sizeof(bright[0]);

int main(void)
{
    sysclock_init(0);
    ws_init();
    ws_reset();
    ws_clrarray(&ledbuffer[0], LED_COUNT);

    PA_CR1 |= (1 << PA1) | (1 << PA2);

    unsigned int progress = 0;
    uint8_t brightness = 0;

    // Set brightness
    while( (PA_IDR & (1 << PA1) ) ) {
        if ( !(PA_IDR & (1 << PA2) ) ) {
            brightness++;
            if ( brightness == brightCount )
                brightness = 0;
            delay_ms( 300 );
        }
        progress++;
        red( progress, bright[brightness] );
        ws_showarray(ledbuffer, LED_COUNT);
        delay_ms( 10 );
    }

    delay_ms( 300 );

    // Brightness fixed, Wait for first group
    while( (PA_IDR & (1 << PA1)) && (PA_IDR & (1 << PA2)) ) {
        progress++;
        rainbow( progress, bright[brightness] );
        ws_showarray(ledbuffer, LED_COUNT);
        delay_ms( 10 );
    }

    delay_ms( 300 );
    uint8_t mode = 0; // red

    // Run
    while ( 1 ) {
        if ( !(PA_IDR & (1 << PA1) ) || !(PA_IDR & (1 << PA2) ) ) {
            mode++;
            if ( mode == modeCount )
                mode = 0;
            delay_ms( 300 );
        }
        progress++;
        modes[ mode ]( progress, bright[brightness] );
        ws_showarray(ledbuffer, LED_COUNT);
        delay_ms( 10 );
    }
}

