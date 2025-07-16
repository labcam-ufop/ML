// Defina o modelo que deseja compilar
//#define USE_RANDOM_FOREST_5
// #define USE_RANDOM_FOREST_10
#define USE_RANDOM_FOREST_15
//#define USE_DECISION_TREE

#include <Arduino.h>

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

float x[] = { 1.79000000, 66.4000000, 2539.80767, 37.2699443, 32.3173639, 7.03200000 };

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

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
}

void loop() {
  // Não faz nada no loop
}

void evaluateModel(const char* modelName, float (*predictFunc)(float*)) {
  Serial.print("Model: ");
  Serial.println(modelName);

  // Measure execution time
  unsigned long startTime = micros();
  float prediction = predictFunc(x);
  unsigned long endTime = micros();
  Serial.print("Execution time (us): ");
  Serial.println(endTime - startTime);

  // Output prediction value
  Serial.print("Prediction: ");
  Serial.println(prediction);

  // Measure RAM usage
  int freeRam = getFreeRam();
  Serial.print("Free RAM (bytes): ");
  Serial.println(freeRam);

  // Measure Flash usage
  Serial.print("Flash used (bytes): ");
  Serial.println(freeFlash());

  Serial.println();
}

int getFreeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

int freeFlash() {
  extern int __bss_end;
  return (int)SP - (int)&__bss_end;
}