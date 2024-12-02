#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h> // Para generar números aleatorios

// Pines para motores
const int pwmMotorA = D1;
const int pwmMotorB = D2;
const int dirMotorA = D3;
const int dirMotorB = D4;

// Pines para sensores
#define FLAME_PIN D5
#define GAS_PIN A0  // Pin analógico A0 para sensor de gas (lectura analógica)
#define GAS_DIGITAL_PIN D0 // Pin digital D0 para sensor de gas
#define TRIG_PIN D6
#define ECHO_PIN D7

// Configuración de la red WiFi
const char *ssid = "Familia_Nativi";
const char *passwd = "le260206N";

// Configuración de MQTT
char *server = "192.168.1.27"; // Dirección de tu broker MQTT
int port = 1883;
const char *mqtt_user = "bomberos";
const char *mqtt_password = "le260206N";

WiFiClient wlanclient;
PubSubClient mqttClient(wlanclient);

// Función para medir la distancia con el sensor ultrasónico
float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout de 30ms
  if (duration == 0) {
    return -1; // Sin eco
  }
  return (duration * 0.034) / 2; // Conversión a cm
}

// Control de los motores
void moveForward(int speed) {
  digitalWrite(dirMotorA, HIGH);
  digitalWrite(dirMotorB, LOW);
  analogWrite(pwmMotorA, speed);
  analogWrite(pwmMotorB, speed);
}

void moveBackward(int speed) {
  digitalWrite(dirMotorA, LOW);
  digitalWrite(dirMotorB, HIGH);
  analogWrite(pwmMotorA, speed);
  analogWrite(pwmMotorB, speed);
}

void stopMotors() {
  analogWrite(pwmMotorA, 0);
  analogWrite(pwmMotorB, 0);
}

// Girar aleatoriamente
void turnRandom() {
  int randomTurn = random(0, 2); // Genera 0 (derecha) o 1 (izquierda)
  if (randomTurn == 0) {
    Serial.println("Girando a la derecha");
    digitalWrite(dirMotorA, HIGH);
    digitalWrite(dirMotorB, HIGH);
    analogWrite(pwmMotorA, 200);
    analogWrite(pwmMotorB, 100); // Gira más hacia la derecha
  } else {
    Serial.println("Girando a la izquierda");
    digitalWrite(dirMotorA, LOW);
    digitalWrite(dirMotorB, LOW);
    analogWrite(pwmMotorA, 100); // Gira más hacia la izquierda
    analogWrite(pwmMotorB, 200);
  }
  delay(1000);
  stopMotors();
}

// Función para publicar mensajes en el broker MQTT
void logToSerialAndMQTT(const char* message) {
  Serial.println(message);
  mqttClient.publish("/robot/log", message); // Publicar mensaje en MQTT
}

// Función de configuración inicial
void setup() {
  // Configuración de pines para motores
  pinMode(pwmMotorA, OUTPUT);
  pinMode(pwmMotorB, OUTPUT);
  pinMode(dirMotorA, OUTPUT);
  pinMode(dirMotorB, OUTPUT);

  // Configuración de pines para sensores
  pinMode(FLAME_PIN, INPUT);
  pinMode(GAS_DIGITAL_PIN, INPUT);  // Configuración para pin digital
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Conectar a WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado a WiFi");

  mqttClient.setServer(server, port);
  if (mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password)) {
    Serial.println("Conectado al Broker MQTT");
  } else {
    Serial.println("Error al conectar al Broker MQTT");
  }
}

// Bucle principal
void loop() {
  // Verificar la conexión Wi-Fi y MQTT
  if (WiFi.status() != WL_CONNECTED) WiFi.begin(ssid, passwd);
  if (!mqttClient.connected()) mqttClient.connect("ESP8266Client", mqtt_user, mqtt_password);
  mqttClient.loop();

  // Sensores
  int flameState = digitalRead(FLAME_PIN);
  int gasState = analogRead(GAS_PIN);  // Lectura analógica del sensor de gas
  int gasDigitalState = digitalRead(GAS_DIGITAL_PIN);  // Lectura digital (opcional)
  float distance = measureDistance();

  // Publicar datos a MQTT
  char msgFlame[50], msgGas[50], msgDistance[50];
  snprintf(msgFlame, 50, "Flama: %s", (flameState == LOW) ? "Detectada" : "No detectada");
  snprintf(msgGas, 50, "Gas: %d", gasState);
  snprintf(msgDistance, 50, "Distancia: %.2f cm", distance);
  mqttClient.publish("/sensor/flame", msgFlame);
  mqttClient.publish("/sensor/gas", msgGas);
  mqttClient.publish("/sensor/distance", msgDistance);

  // Si se detecta flama, retroceder y esperar 40 segundos
  if (flameState == LOW) {
    logToSerialAndMQTT("Flama detectada: Retrocediendo y esperando 40 segundos");
    moveBackward(175);
    delay(1000); // Retroceder durante 1 segundo
    stopMotors();
    delay(40000); // Esperar 40 segundos
    return; // Saltar el resto del loop y volver a empezar
  }

  // Si se detecta algo a 20 cm o menos, detenerse por 2 segundos
  if (distance <= 20 && distance > 0) {
    logToSerialAndMQTT("Objeto cercano detectado a 20 cm o menos: Deteniéndose por 2 segundos");
    stopMotors();
    delay(2000); // Detenerse por 2 segundos

    // Verificar la distancia de nuevo
    distance = measureDistance();
    if (distance <= 20) {
      // Si sigue a 20 cm o menos, retroceder y girar aleatoriamente
      logToSerialAndMQTT("Objeto aún cercano: Retrocediendo y girando aleatoriamente");
      moveBackward(175);
      delay(1000); // Retrocede durante 1 segundo
      stopMotors();
      turnRandom(); // Girar aleatoriamente
    }
  } else {
    // Lógica de movimiento si no se detecta un objeto cercano
    logToSerialAndMQTT("Sin obstáculos cercanos: Avanzando");
    moveForward(175);
  }

  delay(200); // Retardo para evitar sobrecargar el procesador
}
