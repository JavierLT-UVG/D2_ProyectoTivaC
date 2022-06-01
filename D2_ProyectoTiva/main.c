#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#define XTAL 16000000

// Variables Globales
uint32_t i = 0;
bool sensor0;
bool sensor1;
bool sensor2;
bool sensor3;
uint8_t activos;
uint8_t sensorUART;

// Funciones
void delay(uint32_t msec);
void delay1ms(void);

int main(void)
{
    //-------------------------------------- OSCILADOR ---------------------------------------
    // Configurar oscilador externo en 16MHz
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // Configurar el reloj al puerto B (UART)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configurar el reloj al puerto C (LEDS)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    // Configurar el reloj al puerto D (SENSORES)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    // Configurar el reloj al puerto E (LEDS)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    // Configurar el reloj al UART1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    //---------------------------------------- GPIOS -----------------------------------------
    // Configurar como salidas los pines 4, 5, 6 y 7 del puerto C (LEDs)
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    // Configurar como salidas los pines 1, 2, 3 y 4 del puerto E (LEDs)
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
    // Configurar como entrada los pines 0, 1, 2 y 3 del puerto D (Sensores digitales)
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // Configurar como UART los pines 0 y 1 del puerto B
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);

    //----------------------------------------- UART -----------------------------------------
    // Activar comunicación UART a 115200
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    // Activar UART1
    UARTEnable(UART1_BASE);


    while (1)
    {
        sensor0 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0);
        sensor1 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1);
        sensor2 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2);
        sensor3 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3);

        activos = 4 - (sensor0 + sensor1 + sensor2 + sensor3);
        sensorUART = sensor0 << 7 | sensor1 << 6 | sensor2 << 5 | sensor3 << 4;

        switch(sensor0){
            case 0:     // Rojo
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0b01000000);
                break;
            case 1:     // Verde
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0b10000000);
                break;
            default:    // Apagado
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0b11000000);
        }

        switch(sensor1){
            case 0:     // Rojo
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0b00010000);
                break;
            case 1:     // Verde
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0b00100000);
                break;
            default:    // Apagado
                GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, 0b00110000);
        }

        switch(sensor2){
            case 0:     // Rojo
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, 0b00001000);
                break;
            case 1:     // Verde
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, 0b00010000);
                break;
            default:    // Apagado
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, 0b00011000);
        }

        switch(sensor3){
            case 0:     // Rojo
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0b00000010);
                break;
            case 1:     // Verde
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0b00000100);
                break;
            default:    // Apagado
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, 0b00000110);
        }

        // Enviar datos por UART
        UARTCharPut(UART1_BASE, sensorUART);

        delay(200);
    }
}


// Funcion de delay de 1 milisegundo
void delay1ms(void)
{
    SysTickDisable();
    SysTickPeriodSet(16000-1);
    SysTickEnable();

    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); //Pg. 138

}
// Funcion de delay en millis
void delay(uint32_t msec)
{
    for (i = 0; i < msec; i++)
    {
        delay1ms();
    }
}

