// Exam 2024, question 5

/*
- Timer 2B triggers an IRQ every 250ms
- Port E, pins 0-7 are the output of the ADC value (ADC outputs 12 bits, but use 8 LSB, where pin 0 corresponds to the LSB)
- Port D
    - pin 7 GPIO output controlled by a clock (assert every 500ms)
    - pin 6 analog input
    - pin 5 GPIO output indicating ADC value available or not
        - if FIFO is empty, set the GPIO to 0; else 1
    - pin 4 unused
    - pins 0-3 parallel output of the ADC value (ADC outputs 12 bits, but use 4 MSB, where pin 3 corresponds to the MSB)
- empty main(void) { config(); while(1) { };}
- ISR of the timer 2B
    - first execution (rising edge) triggers the ADC process
    - second execution (falling edge) reads the 12-bit ADC result and transfers it using the two GPIO ports (E & D)
    - these two steps get repeated every 250ms
*/

int main(void) {
    init_system();

    while(1) { }
}

void init_system(void) {
    init_port_d();
    init_port_e();
    init_timer();
    init_adc();
}

void timerIsr(void) {
    if (TIMER2_MIS_R & (1<<8)) {
        TIMER2_ICR_R = (1<<8);

        if ((GPIO_PORTD_AHB_DATA_R & (1<<7)) == 0) {    // case S, rising edge
            GPIO_PORTD_AHB_DATA_R |= (1<<7);            // drive pin 7 high

            ADC0_PSSI_R |= (1<<0);                      // trigger the SS0
        } else {
            GPIO_PORTD_AHB_DATA_R &= ~(1<<7);           // drive pin 7 low

            if (ADC0_SSFSTAT0_R & (1<<8)) {             // if ADC FIFO is empty
                GPIO_PORTD_AHB_DATA_R &= ~(1<<5);       // drive pin low, informing invalid ADC data (dont read yet)
            } else {
                GPIO_PORTD_AHB_DATA_R |= (1<<5);        // drive pin high, informing ADC data is available

                uint16_t adc_data = 0;
                uint8_t adc_lower_nibble = 0;
                uint8_t adc_higher_nibble = 0;

                adc_data = (uint16_t)(ADC0_SSFIFO0_R & 0x0FFF); // 12 bits
                adc_higher_nibble = (uint8_t)((adc_data & 0xF00) >> 8);
                adc_lower_nibble  = (uint8_t)(adc_data & 0xFF);

                GPIO_PORTD_AHB_DATA_R &= ~0xF;
                GPIO_PORTD_AHB_DATA_R |= adc_higher_nibble & 0xF;

                GPIO_PORTE_AHB_DATA_R &= ~0xFF;
                GPIO_PORTE_AHB_DATA_R |= adc_lower_nibble;
            }
        }
    }
}

void init_port_d(void) {
    SYSCTL_RCGCGPIO_R |= (1<<3);
    while(!(SYSCTL_PRGPIO_R & (1<<3)));

    // Output pins
    GPIO_PORTD_AHB_DEN_R |= (0xAF);
    GPIO_PORTD_AHB_DIR_R |= (0xAF);
    GPIO_PORTD_AHB_DATA_R = 0x00;

    // ADC input pin
    GPIO_PORTD_AHB_AMSEL_R |= (1<<6);
}

void init_port_e(void) {
    SYSCTL_RCGCGPIO_R |= (1<<4);
    while(!(SYSCTL_PRGPIO_R & (1<<4)));

    GPIO_PORTE_AHB_DEN_R |= (0xFF);
    GPIO_PORTE_AHB_DIR_R |= (0xFF);
    GPIO_PORTE_AHB_DATA_R = 0x00;
}

void init_timer(void) {
    SYSCTL_RCGCTIMER_R |= (1<<2);
    while(!(SYSCTL_PRTIMER_R & (1<<2)));

    TIMER2_CTL_R &= ~(1<<8);        // stop
    TIMER2_CFG_R = 0x4;             // 16 bits
    TIMER2_TBMR_R = 0x02;           // periodic
    TIMER2_TBPR_R = 256 - 1;        // no prescaler
    TIMER2_TBILR_R = 29297 - 1;     // Load value to wait 250ms at 30MHz/256

    TIMER2_IMR_R = (1<<8);          // enable timeout IRQs
    TIMER2_ICR_R = (1<<8);          // clear flag

    NVIC_EN0_R |= INT_TIMER2B;

    TIMER2_CTL_R |= (1<<8);
}

void init_adc(void) {
    SYSCTL_RCGCADC_R |= (1<<0);
    while(!(SYSCTL_PRADC_R & (1<<0)));

    ADC0_ACTSS_R &= ~0x0F;      // disable all sequencers

    /* Magic code 
     * ...
     */
    
    ADC0_SSMUX0_R = 0x5;        // nibble AIN5
    ADC0_SSEMUX0_R = 0;         // no need for this, no nibble is above 16
    ADC0_SSCTL0_R = (1<<1);     // end at first nibble

    ADC0_ACTSS_R |= (1<<0);     // enable sequencer 0
}

