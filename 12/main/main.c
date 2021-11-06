/*********************************************************
* Exemplo para demonstrar o uso de semaforo binário
* Por: Fábio Souza
*********************************************************/

/*inclusão de Bibliotecas*/
#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

/*inclusão das Biblioteca do FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/*log*/
#include "esp_system.h"
#include "esp_log.h"

#define DEBUG 1
#define LED_BUILDING    2    
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<LED_BUILDING)

#define BUTTON_1    12 
#define GPIO_INPUT_PIN_SEL  (1ULL<<BUTTON_1)

/*
 * Variável Global. 
 */
static const char * TAG = "Trata ISR: ";

QueueHandle_t xQueue;
SemaphoreHandle_t xCountingSemaphore;

/**
 * Protótipos
 */
void task_led( void *pvParameter ); 


/**
 * Função de callback ISR da interrupção externa (GPIO-17)
 */
static void IRAM_ATTR gpio_isr_handler( void * pvParameter )
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if( BUTTON_1 == (uint32_t) pvParameter  )
    {

        if( gpio_get_level( BUTTON_1 ) == 0 ) 
        {
            xSemaphoreGiveFromISR( xCountingSemaphore, &xHigherPriorityTaskWoken );

            if( xHigherPriorityTaskWoken == pdTRUE )
            {
                portYIELD_FROM_ISR();
            }

        }   
    
    }   
        
}
 
void app_main( void )
{   

    /* Cria semáforo contador com 10 posições*/
  xCountingSemaphore = xSemaphoreCreateCounting( 255, 0 );
    
    /* comando necessário pois o semáforo começa em zero */
    xSemaphoreGive(xCountingSemaphore);

    if( xTaskCreate( task_led, "task_led", 2048, NULL, 2, NULL )!= pdTRUE )
    {
        if( DEBUG )
            ESP_LOGI( TAG, "error - nao foi possivel alocar task_led.\n" ); 
        return;     
    }
    
}

void task_led( void *pvParameter )
{
    uint32_t x = 0;

    if( DEBUG )
        ESP_LOGI( TAG, "task_led run...\n" );
    /**
     * Led Building
     */
    gpio_config_t io_conf = {
        .intr_type = GPIO_PIN_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = GPIO_OUTPUT_PIN_SEL
    };  
    gpio_config(&io_conf);  
    
    /**
     * Button (por interrupção externa)
     */
    gpio_config_t io_conf2 = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = GPIO_INPUT_PIN_SEL,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf2); 

    /**
     * Habilita a interrupção externa das GPIOs;
     */
    gpio_install_isr_service(0);
    /**
     * Registra o pino que irá gerar acionar a interrupção por borda de descida 
     * e informa qual a função de callback que será chamada. 
     * O número da GPIOs será passado no parametro da função de callback.
     */
    gpio_isr_handler_add( BUTTON_1, gpio_isr_handler, (void*) BUTTON_1 ); 
    
    for(;;) 
    {         

         if( xSemaphoreTake( xCountingSemaphore, portMAX_DELAY ) == pdTRUE )
         {  
            x = uxSemaphoreGetCount(xCountingSemaphore);
            gpio_set_level( LED_BUILDING, x%2 );
                  
            if( DEBUG ) 
                ESP_LOGI( TAG, "x= %d.\r\n", x );

            vTaskDelay(100);

         }
       
    }
}