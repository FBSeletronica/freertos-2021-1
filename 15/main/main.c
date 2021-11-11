/*********************************************************
* Exemplo para demonstrar o uso de Task Notifications
* Por: Fábio Souza
*********************************************************/

#include <stdio.h>
#include <stdint.h>  
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define LED	2    
#define BT	12

static const char * TAG = "Main: ";
static TaskHandle_t xTask1Handle;

void vTask1( void *pvParameter ); 

/**
 * Tratamento da ISR
 */
static void IRAM_ATTR gpio_isr_handler( void * pvParameter)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if( BT == (uint32_t) pvParameter )
	{
		vTaskNotifyGiveFromISR( xTask1Handle, &xHigherPriorityTaskWoken );
        
        if( xHigherPriorityTaskWoken == pdTRUE )
        {
            portYIELD_FROM_ISR();
        }
	
	}   
		
}
 
void vTask1( void * pvParameter )
{
    uint32_t count = 0;
    uint32_t quantNotify = 0;

  
    ESP_LOGI( TAG, "Task 1 running.\n"); 

    while(1) 
    {	

        quantNotify = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
            
        if(quantNotify > 0){
        ESP_LOGI( TAG, "Notify = %d, LED = %d\n", quantNotify, count % 2 ); 
        gpio_set_level( LED, count++ % 2 );
        }
		vTaskDelay( pdMS_TO_TICKS(500));
    }
}

void app_main( void )
{	
    /*
    Configuração do LED
    */
    gpio_pad_select_gpio(LED );
    gpio_set_direction(LED , GPIO_MODE_OUTPUT);

    /*
    Configuração do Botão
    */
    gpio_set_direction(BT,GPIO_MODE_INPUT);
    gpio_set_pull_mode( BT, GPIO_PULLUP_ONLY); 
    gpio_set_intr_type(BT, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BT, gpio_isr_handler, (void*) BT);

    xTaskCreate( vTask1, "Task1", configMINIMAL_STACK_SIZE + 1024, NULL, 2, &xTask1Handle );
    
}



