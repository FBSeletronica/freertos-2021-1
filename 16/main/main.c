/*********************************************************
* Exemplo para demonstrar o uso de task Notification entre
* tarefas
* Por: Fábio Souza
*********************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

/*definição dos pinos*/
#define LED1 2           /*LED ligado ao GPIO2*/
#define BT1 12           /*Botão ligado ao pino GPIO12*/


/* Variáveis para armazenar o handle da Task */
TaskHandle_t xTask1,xTask2;


/* Protótipo das Tasks*/
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* Funções auxiliares */
void vInitHW(void);


void app_main() {
    /*Inicia Hardware */
  gpio_pad_select_gpio(LED1 );
  gpio_set_direction(LED1 , GPIO_MODE_OUTPUT);

  gpio_set_direction(BT1,GPIO_MODE_INPUT);
  gpio_set_pull_mode( BT1, GPIO_PULLUP_ONLY); 

  /* Cria task 1 */
  xTaskCreatePinnedToCore(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL,  1,  &xTask1,APP_CPU_NUM);    

  /* Cria task 2 */
  xTaskCreatePinnedToCore(vTask2,  "Task2",  configMINIMAL_STACK_SIZE + 1024,  NULL,  2,  &xTask2,APP_CPU_NUM);    

}

/* Task 1*/
void vTask1( void *pvParameters )
{
  (void) pvParameters;

  while(1)
  {
    /* Aguarda a notificação*/
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    ESP_LOGI("Task1","Invertendo LED");
    gpio_set_level(LED1,!gpio_get_level(LED1));
  }
}

/* Task2 */
void vTask2( void * pvParameters )
{
  (void) pvParameters;

   uint8_t debouncingTime = 0;

  /* loop forever */
  while(1)
  {
    if(gpio_get_level( BT1 ) == 0){
      debouncingTime++;
      if(debouncingTime >= 10)
      {
        debouncingTime = 0;
        ESP_LOGI("Task2","Notifica Task 1");
        /*Notifica Task1*/
        xTaskNotifyGive(xTask1);
      }
    }
    else{
      debouncingTime = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

