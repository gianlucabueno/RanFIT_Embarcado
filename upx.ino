#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Dinizista";
const char* password = "gigante12";
const int pinoSensor1 = 35;
const int pinoSensor2 = 36;
const unsigned long interval = 1000;  // Intervalo de 1 segundo
unsigned long previousMillis = 0;     // Variável para rastrear o tempo decorrido
String valueMachine;
String userID;
String apiUrlGet;
String apiUrlPost;
String idUsuario;
float tensaoEntrada; 
float potenciaGerada;
float potenciaEnvio;
int sendValue;

// Fazendo uma solicitação GET para a API
HTTPClient http;
HTTPClient httpClient;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }

  Serial.println("Conectado ao WiFi!");
}

void loop() {
  unsigned long currentMillis = millis();
  sendValue = 0;
  potenciaEnvio = 0;
  
  // Verifica se já passou o intervalo de 1 segundo
  if (currentMillis - previousMillis >= 1000 || previousMillis == 0 ) {
    previousMillis = currentMillis;
    apiUrlGet = "https://ranfit-gateway.onrender.com/activity/teste1"; // Substitua pela URL da API que você deseja acessar

    http.begin(apiUrlGet);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String jsonResponse = http.getString();

      // Cria um objeto DynamicJsonDocument para armazenar e processar o JSON
      const size_t capacity = JSON_OBJECT_SIZE(10);
      DynamicJsonDocument jsonDoc(capacity);

      // Faz o parse do JSON recebido
      DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

      if (error) {
        Serial.println("Erro ao fazer parse do JSON");
        Serial.println(error.c_str());
      } else {
        // Processa os dados do JSON
        // Exemplo: lendo um valor específico do JSON
        valueMachine = jsonDoc["user_id"].as<String>();
        userID = jsonDoc["user_id"].as<String>();
        Serial.print("userID: ");
        Serial.println(userID);
      }
      
    }
    else {
      Serial.println("Erro na solicitação GET");
    }

    http.end();
    
    while(userID == valueMachine && sendValue == 0 && valueMachine != ""){
      
      tensaoEntrada = (analogRead(pinoSensor1) * (25/1023.00))/10; 
      potenciaGerada = tensaoEntrada * 0.03;
      Serial.print("potenciaGerada: ");
      Serial.println(potenciaGerada);
      if(potenciaGerada > potenciaEnvio){
        potenciaEnvio = potenciaGerada; 
      }
      Serial.print("potenciaEnvio: ");
      Serial.println(potenciaEnvio);

      http.begin(apiUrlGet);
      int httpCode = http.GET();
      if (httpCode > 0) {
        String jsonResponse = http.getString();

        // Cria um objeto DynamicJsonDocument para armazenar e processar o JSON
        const size_t capacity = JSON_OBJECT_SIZE(10);
        DynamicJsonDocument jsonDoc(capacity);

        // Faz o parse do JSON recebido
        DeserializationError error = deserializeJson(jsonDoc, jsonResponse);

        if (error) {
          Serial.println("Erro ao fazer parse do JSON");
          Serial.println(error.c_str());
        } else {
          // Processa os dados do JSON
          // Exemplo: lendo um valor específico do JSON
          valueMachine = jsonDoc["user_id"].as<String>();
          Serial.print("valueMachine interno: ");
          Serial.println(valueMachine);
          if(userID != valueMachine){
            sendValue = 1;
            Serial.print("sendValue: ");
            Serial.println(sendValue);
          }
          else{
            Serial.print("valueMachine: ");
            Serial.println(valueMachine);
            Serial.println("userID: ");
            Serial.println(userID);
          }
        }
      }
      else {
        Serial.println("Erro na solicitação GET");
      }

      http.end();
    }

    if(sendValue == 1){
      apiUrlPost = "https://ranfit-gateway.onrender.com/activity/";
      http.begin(apiUrlPost);
      http.addHeader("Content-Type", "application/json");         
      
      StaticJsonDocument<200> doc;
      // Add values in the document
      
      doc["points"] = potenciaEnvio  * 300;
      doc["power"] = potenciaEnvio;
          
      String requestBody;
      serializeJson(doc, requestBody);
      
      int httpResponseCode = http.POST(requestBody);
        
      if(httpResponseCode>0){
              
        //String response = http.getString();                       
              
        Serial.println(httpResponseCode);   
        //Serial.println(response);
            
      }
      else {
        Serial.printf("Error occurred while sending HTTP POST: %s\n", httpClient.errorToString(httpResponseCode).c_str());
      }  
      http.end();

      sendValue = 0;
    }
  }
}