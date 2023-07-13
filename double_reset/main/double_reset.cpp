#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
static const char *TAG = "MyTask";

// Fonction de la tâche
void task(void *pvParameters)
{
    // Tâche en cours d'exécution
    printf("Tâche en cours...\n");

    // Simuler un travail
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Fin de la tâche
    printf("Tâche terminée.\n");


    // Supprimer la tâche
    vTaskDelete(NULL);
   //Faut rien mettre après ici, ce ne sera pas exécuté
}


//2023-07-12 19:37:40
TaskHandle_t jobHandle;
void mainJob(void *pvParameters)
{
    // Tâche en cours d'exécution
    printf("mainJob started\n");

    for(int i=0; i<14; i++)
    {
        ESP_LOGI(TAG,">%d",i);
        // Simuler un travail
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // Fin de la tâche
    printf("Job ended.\n");

    // Send notification to waiting task
    xTaskNotifyGive(0);

    // Supprimer la tâche
    vTaskDelete(NULL);  
}//mainJob

extern "C" void app_main(void)
{
    // Initialize logging
    esp_log_level_set(TAG, ESP_LOG_INFO);  // Set log level to INFO

      // Créer la tâche
    xTaskCreate(task, "myTask", 2048, NULL, 5, NULL);
    xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);

    // Attendre la fin de la tâche
   puts("Wait for job task to finish\n");
   xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    //vTaskDelete(NULL);    
    puts("End main!\n");
}
