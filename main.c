/**
 * @file   main.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @brief  Firmware for a talking robot toy.
 */

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "audio_player.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SAMPLE_FREQUENCY    (8000)
#define SAMPLE_PERIOD_US    (1000000 / SAMPLE_FREQUENCY)
#define PRESCALER_RATIO     (8)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct audio_sequence
{
    size_t length;
    uint8_t data[10];
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct audio_player player;
static volatile uint8_t sample_tick;
static struct audio_sequence sequences[] = {
    {
        .length = 4,
        .data = {
            AUDIOLIBRARY_I_ID,
            AUDIOLIBRARY_AM_ID,
            AUDIOLIBRARY_A_ID,
            AUDIOLIBRARY_ROBOT_ID
        }
    },
    {
        .length = 5,
        .data = {
            AUDIOLIBRARY_HI_ID,
            AUDIOLIBRARY_DELAY_ID(200),
            AUDIOLIBRARY_WHAT_ID,
            AUDIOLIBRARY_NAME_ID,
            AUDIOLIBRARY_YOU_ID
        }
    },
    {
        .length = 3,
        .data = {
            AUDIOLIBRARY_LIKES_ID,
            AUDIOLIBRARY_YOU_ID,
            AUDIOLIBRARY_ICECREAM_ID
        }
    },
    {
        .length = 3,
        .data = {
            AUDIOLIBRARY_I_ID,
            AUDIOLIBRARY_LIKES_ID,
            AUDIOLIBRARY_DOGS_ID
        }
    },
};
//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void SetupSampleInterrupt(void);
static inline void SetupPWM(void);
static inline void SetupButton(void);
static inline void SetupLED(void);
static inline void EnableLED(void);
static inline void DisableLED(void);
static inline void SetPWMDutyCycle(uint8_t duty_cycle);
static inline uint8_t GetPWMDutyCycle(void);
static void RampDutyCycle(uint8_t target_duty_cycle);
static void Beep(uint_least16_t frequency, uint_least16_t duration, uint8_t volume);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(TIMER0_COMPA_vect)
{
    if (AudioPlayer_IsPlaying(&player))
    {
        if (AudioPlayer_HasMoreSamples(&player))
        {
            uint8_t sample = AudioPlayer_GetNextSample(&player);
            SetPWMDutyCycle(sample);

            if (sample > 160)
            {
                    PORTD |= (1 << PD0);
            }
            else
            {
                    PORTD &= ~(1 << PD0);
            }
        }
        else
        {
            AudioPlayer_Stop(&player);
        }
    }

    sample_tick = !sample_tick;
}

ISR(PCINT0_vect)
{

}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(void)
{
    wdt_disable();

    AudioPlayer_Init(&player);

    SetupSampleInterrupt();
    SetupPWM();
    SetupButton();
    SetupLED();

    sei();

    RampDutyCycle(128);

    uint8_t counter = 0;
    while (1)
    {
        const size_t i = counter % (sizeof(sequences) / sizeof(sequences)[0]);

        AudioPlayer_PlaySequence(&player, sequences[i].data, sequences[i].length, 10);
        _delay_ms(300);

        RampDutyCycle(0);
        DisableLED();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();

        RampDutyCycle(128);
        ++counter;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void SetupSampleInterrupt(void)
{
    /* Set CTC-mode */
    TCCR0A |= (1 << WGM01);

    /* Set prescaler to 1:8 */
    TCCR0B |= (1 << CS01);

    /* Set compare value for the desired frequency */
    OCR0A = ((F_CPU / PRESCALER_RATIO) / SAMPLE_FREQUENCY);

    /* Enabled interrupt on compare match A. */
    TIMSK0 |= (1 << OCIE0A);
}

static inline void SetupPWM(void)
{
    /* Set OCRA1/PB1 as output */
    DDRB |= (1 << PB1);

    /* Set fast 8-bit PWM mode */
    TCCR1A |= (1 << WGM10);
    TCCR1B |= (1 << WGM12);

    /* Clear OCR1A on compare match */
    TCCR1A |= (1 << COM1A1);

    /* Set prescaler to 1:1 */
    TCCR1B |= (1 << CS10);

    /* Set PWM duty cycle to 0%*/
    OCR1A = 0;
}

static inline void SetupButton(void)
{
    /* Set PB0 as input with pull-up */
    DDRB &= ~(1 << PB0);
    PORTB |= (1 << PB0);

    /* Enable pin change interrupts */
    PCMSK0 |= (1 << PCINT0);
    PCICR |= (1 << PCIE0);
}

static inline void SetupLED(void)
{
    /* Set PD0 as output and pull low. */
    DDRD |= (1 << PD0);
    PORTD &= ~(1 << PD0);
}

static inline void EnableLED(void)
{
    PORTB |= (1 << PB2);
}

static inline void DisableLED(void)
{
    PORTB &= ~(1 << PB2);
}

static inline void SetPWMDutyCycle(uint8_t duty_cycle)
{
    OCR1A = duty_cycle;
}

static inline uint8_t GetPWMDutyCycle(void)
{
    return OCR1A;
}

static void RampDutyCycle(uint8_t target_duty_cycle)
{
    int8_t modifier = (target_duty_cycle > GetPWMDutyCycle()) ? 1 : -1;

    for (uint8_t duty_cycle = GetPWMDutyCycle(); duty_cycle != target_duty_cycle; duty_cycle += modifier)
    {
        SetPWMDutyCycle(duty_cycle);
        _delay_us(SAMPLE_PERIOD_US);
    }
    SetPWMDutyCycle(GetPWMDutyCycle() + modifier);
}

static void Beep(uint_least16_t frequency, uint_least16_t duration, uint8_t volume)
{
    assert(frequency <= (SAMPLE_FREQUENCY / 2));


    uint_least16_t ticks = 0;
    const uint_least16_t duration_ticks = duration * 8;
    const uint_least16_t period_ticks = SAMPLE_FREQUENCY / frequency / 2;

    uint8_t current_tick = sample_tick;

    bool state = false;
    while(ticks < duration_ticks)
    {
        if (sample_tick != current_tick)
        {
            current_tick = sample_tick;
            ++ticks;


            if (ticks % period_ticks == 0)
            {
                SetPWMDutyCycle((uint8_t)state * volume);
                state = !state;
            }
        }
    }
}
