
// Defina o modelo que deseja compilar
//#define USE_RANDOM_FOREST_5
// #define USE_RANDOM_FOREST_10
#define USE_RANDOM_FOREST_15
//#define USE_DECISION_TREE

#include <Arduino.h>

// Inclusão das bibliotecas do FreeRTOS para ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#ifdef USE_RANDOM_FOREST_5
#include "random_forest5_model.c"
#endif

#ifdef USE_RANDOM_FOREST_10
#include "random_forest10_model.c"
#endif

#ifdef USE_RANDOM_FOREST_15
#include "random_forest15_model.c"
#endif

#ifdef USE_DECISION_TREE
#include "decisionTree.c"
#endif

// Parâmetros para as tarefas FreeRTOS
#define STACK_SIZE 4096
#define TASK_PRIORITY 1

// Dados de entrada para predição
float x[] = { 1.79000000, 66.4000000, 2539.80767, 37.2699443, 32.3173639, 7.03200000 };

// Protótipos das funções
void evaluateModel(const char* modelName, float (*predictFunc)(float*));
int getFreeRam();
int getFlashUsage();

// Tarefas FreeRTOS
void modelEvaluationTask(void *pvParameters);
void monitorTask(void *pvParameters);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("Iniciando avaliação de modelos ML no ESP32 com FreeRTOS");
  
  // Criar tarefa para avaliação dos modelos
  xTaskCreate(
    modelEvaluationTask,    // Função da tarefa
    "ModelEvaluation",      // Nome da tarefa
    STACK_SIZE,             // Tamanho da pilha
    NULL,                   // Parâmetros da tarefa
    TASK_PRIORITY,          // Prioridade
    NULL                    // Handle da tarefa
  );
  
  // Criar tarefa para monitoramento (opcional)
  xTaskCreate(
    monitorTask,
    "Monitor",
    STACK_SIZE,
    NULL,
    TASK_PRIORITY,
    NULL
  );
}

void loop() {
  // O loop principal fica vazio, pois o FreeRTOS gerencia as tarefas
  vTaskDelay(1000 / portTICK_PERIOD_MS); // Evita watchdog timer reset
}

// Tarefa principal para avaliação dos modelos
void modelEvaluationTask(void *pvParameters) {
  // Executa uma vez e depois repete em intervalos regulares
  while (true) {
    Serial.println("\n--- Iniciando nova avaliação de modelos ---");
    
#ifdef USE_RANDOM_FOREST_5
    evaluateModel("Random Forest 5", predict_random_forest5);
#endif

#ifdef USE_RANDOM_FOREST_10
    evaluateModel("Random Forest 10", predict_random_forest10);
#endif

#ifdef USE_RANDOM_FOREST_15
    evaluateModel("Random Forest 15", predict_random_forest15);
#endif

#ifdef USE_DECISION_TREE
    evaluateModel("Decision Tree", predict_decision_tree);
#endif

    // Aguarda antes de uma nova execução (ajuste conforme necessário)
    vTaskDelay(10000 / portTICK_PERIOD_MS); // 10 segundos de intervalo
  }
}

// Tarefa para monitoramento do sistema
void monitorTask(void *pvParameters) {
  while (true) {
    // Monitoramento periódico de recursos do sistema
    Serial.println("\n--- Status do Sistema ---");
    Serial.print("Heap livre: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    
    Serial.print("Menor quantidade de heap livre: ");
    Serial.print(ESP.getMinFreeHeap());
    Serial.println(" bytes");
    
    Serial.print("CPU Freq: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    
    // Aguarda antes da próxima verificação
    vTaskDelay(30000 / portTICK_PERIOD_MS); // 30 segundos de intervalo
  }
}

void evaluateModel(const char* modelName, float (*predictFunc)(float*)) {
  Serial.print("Modelo: ");
  Serial.println(modelName);

  // Medir tempo de execução
  unsigned long startTime = micros();
  float prediction = predictFunc(x);
  unsigned long endTime = micros();
  
  Serial.print("Tempo de execução (us): ");
  Serial.println(endTime - startTime);

  // Saída do valor de predição
  Serial.print("Predição: ");
  Serial.println(prediction);

  // Uso de memória atual
  Serial.print("Heap livre (bytes): "); // o heap livre representa a quantidade de RAM dinâmica disponível para alocação em tempo de execução.
  Serial.println(ESP.getFreeHeap());

  // Outras estatísticas específicas do ESP32
  Serial.print("Tamanho do sketch (bytes): ");
  Serial.println(ESP.getSketchSize());
  
  Serial.println();
}

// Estas funções não são necessárias para o ESP32, pois o ESP.getFreeHeap() já fornece esta informação
// Mantidas para compatibilidade com código original
int getFreeRam() {
  return ESP.getFreeHeap();
}

int getFlashUsage() {
  return ESP.getSketchSize();
}
