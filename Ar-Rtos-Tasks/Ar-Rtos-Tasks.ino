#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 15       // Pin para DHT22 
#define DHTTYPE DHT22
#define MQ2PIN 34       // Pin analógico para MQ2

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float temperatura = 0;
float humedad = 0;
int gas = 0;
bool alarma = false;

void leerTemperaturaHumedad(void *pvParameters) {
  Serial.printf("[DHT22] Núcleo: %d\n", xPortGetCoreID());
  for (;;) {
    temperatura = dht.readTemperature();
    humedad = dht.readHumidity();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void leerGas(void *pvParameters) {
  Serial.printf("[MQ2] Núcleo: %d\n", xPortGetCoreID());
  for (;;) {
    gas = analogRead(MQ2PIN);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void actualizarLCD(void *pvParameters) {
  Serial.printf("[LCD] Núcleo: %d\n", xPortGetCoreID());
  for (;;) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("T:%.1f H:%.1f", temperatura, humedad);
    lcd.setCursor(0, 1);
    lcd.printf("Gas:%d", gas);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}

void verificarAlarmas(void *pvParameters) {
  Serial.printf("[Alarma] Núcleo: %d\n", xPortGetCoreID());
  for (;;) {
    alarma = (temperatura > 35 || gas > 3600);
    if (alarma) {
      Serial.println("¡ALERTA CRÍTICA!");
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void envioDiagnostico(void *pvParameters) {
  Serial.printf("[Diag] Núcleo: %d\n", xPortGetCoreID());
  for (;;) {
    Serial.println("====== DIAGNÓSTICO SISTEMA ======");
    Serial.printf("Tiempo: %lu ms\n", millis());
    Serial.printf("Núcleo actual: %d\n", xPortGetCoreID());
    Serial.printf("Temp: %.1f | Hum: %.1f | Gas: %d\n", temperatura, humedad, gas);
    Serial.printf("Alarma activa: %s\n", alarma ? "Sí" : "No");
    Serial.println("==================================");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(MQ2PIN, INPUT);

// Crear tareas en FreeRTOS - Asignando Nucleos
  xTaskCreatePinnedToCore(leerTemperaturaHumedad, "DHT22", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(leerGas, "MQ2", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(actualizarLCD, "LCD", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(verificarAlarmas, "Alarma", 2048, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(envioDiagnostico, "Diag", 2048, NULL, 1, NULL, 0);

  
  // Crear tareas en FreeRTOS - Nucleos Automaticos
 /* xTaskCreate(leerTemperatura, "Temp", 1000, NULL, 2, NULL);
  xTaskCreate(leerGas, "MQ2", 1000, NULL, 2, NULL);
  xTaskCreate(actualizarLCD, "LCD", 1000, NULL, 1, NULL);
  xTaskCreate(verificarAlarmas, "Alarma", 1000, NULL, 3, NULL);
  xTaskCreate(diagnostico, "Diag", 1500, NULL, 1, NULL);*/


  /*
  “Esta es la línea clave para crear una tarea. Los parámetros son:
        1. Función que contiene la tarea.
        2. Nombre para depuración.
        3. Stack size: cuánta memoria usará esta tarea (en palabras, no bytes).
        4. Parámetro (no usamos ninguno).
        5. Prioridad (más alto, más urgente).
        6. Handle (puntero para manipular la tarea si se requiere).
En este ejemplo damos prioridad 2 a lectura de sensores, y 3 a la alarma.*/


}

void loop() {
  // Solo solin solito
}