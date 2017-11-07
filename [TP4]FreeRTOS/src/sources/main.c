/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*System includes.*/
#include <stdio.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "trcBase.h"

#include "pin_mux.h"
#include "clock_config.h"

#include "fsl_port.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_SYSVIEW_APP_NAME "FRDMK64F System Viewer Demo"
#define APP_SYSVIEW_DEVICE_NAME "Cortex-M4"
#define APP_SYSVIEW_RAM_BASE (0x1FFF0000)

#define BOARD_LED_GPIO BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

#define BOARD_SW_GPIO BOARD_SW3_GPIO
#define BOARD_SW_PORT BOARD_SW3_PORT
#define BOARD_SW_GPIO_PIN BOARD_SW3_GPIO_PIN
#define BOARD_SW_IRQ BOARD_SW3_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW3_IRQ_HANDLER
#define BOARD_SW_NAME BOARD_SW3_NAME

/* Task priorities. */
#define first_task_PRIORITY		(configMAX_PRIORITIES)
#define led_task_PRIORITY		(configMAX_PRIORITIES - 1)
#define write1_PRIORITY     	(configMAX_PRIORITIES - 2)
#define write2_PRIORITY    		(configMAX_PRIORITIES - 2)
#define print_console_PRIORITY   (configMAX_PRIORITIES - 3)

/*******************************************************************************
* Globals
******************************************************************************/
/* Logger queue handle */
static QueueHandle_t log_queue = NULL;

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Whether the SW button is pressed */
volatile bool g_ButtonPress = false;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void first_task(void *pvParameters);
static void write1(void *pvParameters);
static void write2(void *pvParameters);
static void print_console(void *pvParameters);
static void interrupt_task(void *pvParameters);

/*
 * Definición de la estructura de datos a transmitir.
 */
/* Define the structure type that will be passed on the queue. */
typedef enum INFO_TYPE
{
	TipoValor,
	TipoString
} TipoDato;

typedef struct TASK_INFO
{
	TipoDato eTipoDato;
	void* pDato;
} QueueEstruct;


/* Define the init structure for the output LED pin*/
gpio_pin_config_t led_config = {
    kGPIO_DigitalOutput, 0,
};

/* Define the init structure for the input switch pin */
gpio_pin_config_t sw_config = {
    kGPIO_DigitalInput, 0,
};

void BOARD_SW_IRQ_HANDLER(void)
{
    /* Clear external interrupt flag. */
    GPIO_ClearPinsInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
    /* Change state of button. */
    g_ButtonPress = true;
}

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    vTraceInitTraceData();

    /* Init output LED GPIO. */
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);

    /* Init input switch GPIO. */
    PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN, kPORT_InterruptFallingEdge);
    EnableIRQ(BOARD_SW_IRQ);
    GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);

    if (!uiTraceStart()){
        vTraceConsoleMessage("Could not start recorder!");
    }

    xTaskCreate(first_task, "first_task", configMINIMAL_STACK_SIZE, NULL, first_task_PRIORITY, NULL);
    xTaskCreate(write1, "write1", configMINIMAL_STACK_SIZE + 166, NULL, write1_PRIORITY, NULL);
    xTaskCreate(write2, "write2", configMINIMAL_STACK_SIZE + 166, NULL, write2_PRIORITY, NULL);
    xTaskCreate(interrupt_task, "interrupt_task", configMINIMAL_STACK_SIZE + 166, NULL, led_task_PRIORITY, NULL);
    vTaskStartScheduler();
    while(1);
}



/*!
 * @brief first_task function
 */
static void first_task(void *pvParameters){
    traceLabel consoleLabel;
    consoleLabel = xTraceOpenLabel("Messages");

    PRINTF("Start address: 0x%x \r\n", RecorderDataPtr);
    PRINTF("Bytes to read: 0x%x \r\n", sizeof(*RecorderDataPtr));

    /* Init Queue. */
    log_queue = xQueueCreate(10, sizeof(struct xTaskInfo *));
    xTaskCreate(print_console, "print_console", configMINIMAL_STACK_SIZE + 166, NULL, print_console_PRIORITY, NULL);

    vTaskSuspend(NULL);
}


/*!
 * @brief interrupt_task function
 */
static void interrupt_task(void *pvParameters){
	static unsigned short i;
	static QueueEstruct Dato;
	static QueueEstruct* p = &Dato;
	traceLabel consoleLabel;
	consoleLabel = xTraceOpenLabel("Messages");
	i=0;
    for(;;){
    	 if (g_ButtonPress){
    		 i = i+1;
    		 p -> eTipoDato = TipoValor;
    		 p ->pDato = &i;
    		 xQueueSend(log_queue, (void *)&p, 0);
			 GPIO_TogglePinsOutput(BOARD_LED_GPIO, 1U << BOARD_LED_GPIO_PIN); // Toggle LED.
			 g_ButtonPress = false; // Reset state of button.
    	  }
        vTaskDelay(10); //delay de 10ms
    }
}


/*!
 * @brief write1 function
 */
static void write1(void *pvParameters){
    traceLabel consoleLabel;
    char log[20];
    char aux[4];
	uint8_t i;
	static QueueEstruct Dato;
	static QueueEstruct* p = &Dato;
	consoleLabel = xTraceOpenLabel("Messages");
	for(;;){
		i = 6;
		sprintf(log, "wite1: ");
		for (i = 6; i > 0; i--){
			sprintf(aux, "%d",(int)i);
			strcat(log, aux);
			p -> eTipoDato = TipoString;
			p ->pDato = &log;
			xQueueSend(log_queue, (void *)&p, 0);
			taskYIELD();
		}

		vTaskDelay(500); //delay 500ms
	}
}

/*!
 * @brief write2 function
 */
static void write2(void *pvParameters){
    traceLabel consoleLabel;
    consoleLabel = xTraceOpenLabel("Messages");
    char log[20];
    char aux[4];
    uint8_t i;
    static QueueEstruct Dato;
	static QueueEstruct* p = &Dato;

	for(;;){
		sprintf(log, "write2: ");
		for (i = 0; i < 10; i++){
			sprintf(aux, "%d",(int)i);
			strcat(log, aux);
			p -> eTipoDato = TipoString;
			p -> pDato = &log;
			xQueueSend(log_queue, (void *)&p, 0);
			taskYIELD();
		}

		vTaskDelay(500); //Delay de 500ms
	}
}

/*!
 * @brief print_console function
 */
static void print_console(void *pvParameters){
    traceLabel consoleLabel;
    consoleLabel = xTraceOpenLabel("Messages");
    uint32_t counter = 0;
    static unsigned short recibidoSensor;
    static char* recibidoUsuario;
    static QueueEstruct* log;

    for(;;){
        xQueueReceive(log_queue, &log, portMAX_DELAY);
        if (log ->eTipoDato == TipoValor){
        	recibidoSensor = *((unsigned short*) log -> pDato);
        	PRINTF("LOG[%d]: Cantidad de interrupciones por %s: %u \r\n", counter, BOARD_SW_NAME, recibidoSensor);
        }
        else if (log -> eTipoDato == TipoString){
        	recibidoUsuario = ((char*) log ->pDato);
        	PRINTF("LOG[%d]: %s\r\n",counter, recibidoUsuario);
        }
        counter++;
    }
}

