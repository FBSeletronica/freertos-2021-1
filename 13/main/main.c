/*********************************************************
* Exemplo para demonstrar o uso de software timer
* Por: Fábio Souza
*********************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "esp_log.h"

/*definição dos pinos*/
#define LED1 2           /*LED ligado ao GPIO2*/
#define LED2 14          /*LED ligado ao GPIO14*/
#define BT1 12           /*Botão ligado ao pino GPIO12*/

TaskHandle_t xTask1;           /*hadle da task1*/
TimerHandle_t xTimer1,xTimer2; /*handle dos timers*/


/*protótipo da Task1*/
void vTask1( void * pvParameters );

/* Funções auxiliares */
void vInitHW(void);

void callBackTimer1(TimerHandle_t pxTimer );
void callBackTimer2(TimerHandle_t pxTimer );


void app_main() {
    /*Inicia Hardware */
  vInitHW();

  /*Cria o timer1 com 1000ms de periodo com auto-reaload*/
  xTimer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, 0, callBackTimer1);
  if( xTimer1 == NULL )
  {
    ESP_LOGE("xTimer1","Não foi possível criar o timer 1");
    while(1);
  }
  else{
    ESP_LOGI("xTimer1","Timer 1 Criado");
  }

  /*Cria o timer2 com 1000ms de periodo com one shot*/
  xTimer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(10000), pdFALSE, 0, callBackTimer2);
  
  /* Cria task1 */
  xTaskCreatePinnedToCore(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1,  &xTask1,APP_CPU_NUM);    

  /*Inicia o timer1*/
  xTimerStart(xTimer1, 0);

}


/* Task1 */
void vTask1( void * pvParameters )
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
          gpio_set_level(LED2 , 1);        /*inverte estado do LED*/
          debouncingTime = 0;
          xTimerStart(xTimer2, 0);
          ESP_LOGI("xTimer2","Timer 2 Start");
      }
    }
    else{
      debouncingTime = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

/* Função Init Hardware */
void vInitHW(void)
{
    gpio_pad_select_gpio(LED1 );
    gpio_set_direction(LED1 , GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(LED2 );
    gpio_set_direction(LED2 , GPIO_MODE_OUTPUT);

    gpio_set_direction(BT1,GPIO_MODE_INPUT);
    gpio_set_pull_mode( BT1, GPIO_PULLUP_ONLY); 
}

void callBackTimer1(TimerHandle_t pxTimer )
{
  static uint8_t x = 0;
  
  gpio_set_level(LED1 , x^=1);        /*inverte estado do LED*/
  ESP_LOGI("xTimer1","Timer 1 callback");  
}

void callBackTimer2(TimerHandle_t pxTimer )
{
  gpio_set_level(LED2 , 0);   
  ESP_LOGI("xTimer2","Timer 2 Stop");     
}