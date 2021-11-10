/*********************************************************
* Exemplo para demonstrar o uso de grupo de eventos
* Por: Fábio Souza
*********************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "esp_log.h"


/*definição dos pinos*/
#define LED1 2           /*LED ligado ao GPIO2*/
#define BT1 12           /*Botão ligado ao pino GPIO12*/

/* define event bits */
#define TASK_1_BIT        ( 1 << 0 ) //1
#define TASK_2_BIT        ( 1 << 1 ) //10


TaskHandle_t xTask1,xTask2;    /*hadle das tasks*/
TimerHandle_t xTimer1,xTimer2; /*handle dos timers*/

/* create event group */
EventGroupHandle_t xEventos;

/* Funções auxiliares */
void vInitHW(void);

//Protótipo de funções
void callBackTimer1(TimerHandle_t pxTimer );

void vTask1( void * pvParameters );
void vTask2( void * pvParameters );
void vTask3( void * pvParameters );

int8_t count = 0;

void app_main() {

/*Inicia Hardware*/
  gpio_pad_select_gpio(LED1 );
  gpio_set_direction(LED1 , GPIO_MODE_OUTPUT);

  gpio_set_direction(BT1,GPIO_MODE_INPUT);
  gpio_set_pull_mode( BT1, GPIO_PULLUP_ONLY); 

  xEventos = xEventGroupCreate();

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
    

 /* Cria tasks */
  xTaskCreatePinnedToCore(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1,  &xTask1,APP_CPU_NUM);    
  xTaskCreatePinnedToCore(vTask2,  "Task2",  configMINIMAL_STACK_SIZE + 1024,  NULL, 1,  &xTask2,APP_CPU_NUM);    

  /*Inicia o timer1*/
  xTimerStart(xTimer1, 0);

}

void vTask1( void * parameter )
{

  while(1)
  {
    /* wait forever until event bit of task 1 is set */
    EventBits_t xbit = xEventGroupWaitBits(xEventos, TASK_1_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI("TASK1","Task 1 saiu do estado de bloqueio");
  }
}

/* this task is similar to sendTask1 */
void vTask2( void * parameter )
{

  while(1)
  {
    /* wait forever until event bit of task 2 is set */
    EventBits_t xbit = xEventGroupWaitBits(xEventos, TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    ESP_LOGI("TASK1","Task 2 saiu do estado de bloqueio");
  }
}

//callback do timer
void callBackTimer1(TimerHandle_t pxTimer )
{
  
  count++;
  gpio_set_level(LED1 , count%2);        /*inverte estado do LED*/
  if(count == 5){
    xEventGroupSetBits(xEventos,TASK_1_BIT);
  }else if(count == 10){
    xEventGroupSetBits(xEventos,TASK_2_BIT);
  }
  else if(count == 15){
      count = 0;
      xEventGroupSetBits(xEventos,TASK_1_BIT|TASK_2_BIT);
  }
}