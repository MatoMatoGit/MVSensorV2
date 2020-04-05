#include "ext_int.h"

#include "avr/io.h"
#include "avr/interrupt.h"

#include <string.h> /* For memset. */

typedef struct {
    uint8_t pin;
    volatile uint8_t *port;
    ExtIntType_t type;
    ExtIntCallback_t cb;
}ExtIntDesc_t;


static ExtIntDesc_t ExtInts[NUM_EXT_INTS];
static ExtIntDesc_t *ActiveInt = NULL;


static ExtIntDesc_t *IExtInt(void);
static void IExtIntInit(ExtIntDesc_t *ext_int);
static void ISenseControlInit(ExtIntType_t type);

void ExtIntInit(void)
{
    memset(ExtInts, 0, sizeof(ExtInts));
    ActiveInt = NULL;
}

void ExtIntDeinit(void)
{
}


ExtInt_t ExtIntRegister(uint8_t pin, volatile uint8_t *port, ExtIntType_t type, ExtIntCallback_t cb)
{
    ExtIntDesc_t *ext_int = NULL;

    ext_int = IExtInt();
    if(ext_int != NULL) {
        *port &= ~(1 << pin);
        ext_int->pin = pin;
        ext_int->port = port;
        ext_int->type = type;
        ext_int->cb = cb;
    }

    return (ExtInt_t)ext_int;
}

int ExtIntEnable(ExtInt_t ext_int)
{
    if(ActiveInt == NULL) {
        IExtIntInit((ExtIntDesc_t *)ext_int);
        return 0;
    }

    return -1;
}

void ExtIntDisable(void)
{
    if(ActiveInt == NULL) {
        return;
    }

    cli();
    PCMSK |= (1 << ActiveInt->pin);
    GIMSK |= (1 << PCIE);
    ActiveInt = NULL;
    sei();
}

static ExtIntDesc_t *IExtInt(void)
{
    for(uint8_t i = 0; i < NUM_EXT_INTS; i++) {
        if(ExtInts[i].port == NULL) {
            return &ExtInts[i];
        }
    }

    return NULL;
}

static void IExtIntInit(ExtIntDesc_t *ext_int)
{
    cli();

    /* Clear External Interrupt Flag 0 and the
     * Pin Change Interrupt Flag. */
    GIFR &= ~(1 << INTF0);
    GIFR &= ~(1 << PCIF);

    switch(ext_int->type) {
    case EXT_INT_PIN_CHANGE:
        /* Set the bit in the Pin Change Mask Register
         * corresponding to the pin. */
        PCMSK |= (1 << ext_int->pin);

        /* Enable Pin Change Interrupts. */
        GIMSK |= (1 << PCIE);
        break;

    case EXT_INT_PIN_LOW:
    case EXT_INT_PIN_FALLING:
    case EXT_INT_PIN_RISING:
        ISenseControlInit(ext_int->type);

        /* Enable External Interrupt Requests 0. */
        GIMSK |= (1 << INT0);
        break;
    }

    ActiveInt = ext_int;
    sei();
}

static void ISenseControlInit(ExtIntType_t type)
{
    switch(type) {
    case EXT_INT_PIN_LOW:
        /* Set ISC0-0 and ISC1-0. */
        MCUCR &= ~(1 << ISC00);
        MCUCR &= ~(1 << ISC01);
        break;

    case EXT_INT_PIN_FALLING:
        /* Set ISC0-0 and ISC1-1. */
        MCUCR &= ~(1 << ISC00);
        MCUCR |= (1 << ISC01);
        break;

    case EXT_INT_PIN_RISING:
        /* Set ISC0-1 and ISC1-1. */
        MCUCR |= (1 << ISC00);
        MCUCR |= (1 << ISC01);
        break;

    /* Handles EXT_INT_PIN_CHANGE and invalid types. */
    default:
        break;
    }
}

ISR(PCINT0_vect)
{
    volatile uint8_t state = 0;

    if(ActiveInt != NULL) {
        state = *ActiveInt->port & (1 << ActiveInt->pin);
        ActiveInt->cb(state);
    }

}

