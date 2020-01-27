
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "audio_player.h"

#define F_CPU 8000000

static struct audio_player player;

ISR(TIMER0_COMPA_vect)
{
    if (AudioPlayer_IsPlaying(&player))
    {
        if (AudioPlayer_HasMoreSamples(&player))
        {
            uint8_t sample = AudioPlayer_GetNextSample(&player);
            //PWM_SetDutyCycle(sample);
        }
        else
        {
            AudioPlayer_Stop(&player);
        }
    }
}

static inline void SetupSampleInterrupt(void)
{
    /* Set CTC-mode */
    TCCR0A |= (1 << WGM01);

    /* Set prescaler to 1:8 */
    TCCR0B |= (1 << CS01);

    /* Set compare value */
    OCR0A = 125;

    /* Enabled interrupt on compare match A. */
    TIMSK0 |= (1 << OCIE0A);
}

static inline void SetupPWMInterrupt(void)
{
    /* Set fast 8-bit PWM mode */
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    /* Clear OCR1A on compare match */
    TCCR1A |= (1 << COM1A1);

    /* Set prescaler to 1:1 */
    TCCR1B |= (1 << CS10);

    /* Start at 50 % duty cycle */
    OCR1A = 128;
}

static inline void SetPWMDutyCycle(uint8_t duty_cycle)
{
    OCR1A = duty_cycle;
}

int main()
{
    AudioPlayer_Init(&player);

    SetupSampleInterrupt();
    SetupPWMInterrupt();

    sei();

    while(1)
    {

    }
}
