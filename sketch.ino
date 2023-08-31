#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27
#define LCD_COLUMNS 20
#define LCD_LINES 4
#include "HTTPClient.h"
#include "WiFi.h"
#include "ThingSpeak.h"

WiFiClient client;

const char* ssid = "Wokwi-GUEST"; // Nome da rede
const char* senha = ""; // Senha da rede
const int DHT_PIN = 15;
int rele = 12;
int led = 2;

const char* WriteKey = "FTAFD1P7ALS7FK6J";

DHTesp dhtSensor;
HTTPClient http;

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setup() {

  Serial.begin(115200);
  Serial.print("Conectando a rede Wifi ");
  WiFi.begin(ssid, senha); // para conectar na rede
  while(WiFi.status() != WL_CONNECTED){ // Ficar verificando até conectar
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado a rede");
  pinMode(rele, OUTPUT);
  pinMode(led, OUTPUT);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  lcd.init();
  lcd.backlight();

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);

}

void loop() {


  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  Serial.println("Temperatura: " + String(data.temperature, 1) + "°C");
  Serial.println("Umidade: " + String(data.humidity, 1) + "%");
  Serial.println("---");

  ThingSpeak.setField(1, data.temperature);
  ThingSpeak.setField(2, data.humidity);

  if (data.humidity > 70){
    digitalWrite(rele, HIGH); // Luz relé acende
  }
  else {
    digitalWrite(rele, LOW); // Luz relé apaga
  }

  if (data.temperature > 35){ 
    digitalWrite(led, HIGH); // LED acende
  }
  else{
    digitalWrite(led, LOW); // LED apaga
  }

  lcd.setCursor(0, 0);
  lcd.print(" Temp: " + String(data.temperature, 1) + "\xDF"+"C ");
  lcd.setCursor(0, 1);
  lcd.print(" Umidade: " + String(data.humidity, 1) + "% ");

  ThingSpeak.writeFields(1, WriteKey);

  delay(5000); // Intervalo de 5 segundos conforme a SA
}

