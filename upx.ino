
/**
 * Created by K. Suwatchai (Mobizt)
 *
 * Email: k_suwatchai@hotmail.com
 *
 * Github: https://github.com/mobizt/Firebase-ESP-Client
 *
 * Copyright (c) 2023 mobizt
 *
 */

// This example shows how to get a document from a document collection. This operation required Email/password, custom or OAUth2.0 authentication.

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Dinizista"
#define WIFI_PASSWORD "gigante12"

/* 2. Define the API Key */
#define API_KEY "BOAJ4YjiGY-tEmxEygowLwY9cCeKU-u_KfYhNcWl06sWfO4x7ui6EgcusuIbKVm8pTqAPax7gkNo6uS_D1m57QA"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "PROJECT_ID"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "gianbuenor@gmail.com"
#define USER_PASSWORD "Gi@n1598"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool taskCompleted = false;

unsigned long dataMillis = 0;


const int pinoSensorTensao = 35; //PINO ANALÓGICO EM QUE O SENSOR ESTÁ CONECTADO
const int pinoSensorCorrente = 36;
float tensaoEntrada = 0.0; 


void setup()
{

    pinMode(pinoSensorTensa, INPUT); //DEFINE O PINO COMO ENTRADA
    pinMode(ppinoSensorCorrente, INPUT); //DEFINE O PINO COMO ENTRADA

    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);


    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // The WiFi credentials are required for Pico W
    // due to it does not have reconnect feature.


    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h



    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);
}

void loop()
{

  tensaoEntrada = (analogRead(pinoSensor) * (25/1023.00))/10; //FAZ A LEITURA DO PINO ANALÓGICO E ARMAZENA NA VARIÁVEL O VALOR LIDO
  Serial.print("Tensão DC medida: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(tensaoEntrada,2); //IMPRIME NA SERIAL O VALOR DE TENSÃO DC MEDIDA E LIMITA O VALOR A 2 CASAS DECIMAIS
  Serial.println("V"); //IMPRIME O TEXTO NA SERIAL
  delay(1000); //INTERVALO DE 500 MILISSEGUNDOS

    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready() && (millis() - dataMillis > 10000 || dataMillis == 0))
    {
        dataMillis = millis();
        bool envio = 0
        if (!taskCompleted && envio == 0)
        {
            

            // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
            FirebaseJson content;

            content.set("fields/Japan/mapValue/fields/time_zone/integerValue", "9");
            content.set("fields/Japan/mapValue/fields/population/integerValue", "125570000");

            content.set("fields/Belgium/mapValue/fields/time_zone/integerValue", "1");
            content.set("fields/Belgium/mapValue/fields/population/integerValue", "11492641");

            content.set("fields/Singapore/mapValue/fields/time_zone/integerValue", "8");
            content.set("fields/Singapore/mapValue/fields/population/integerValue", "5703600");

            // info is the collection id, countries is the document id in collection info.
            String documentPath = "info/countries";

            Serial.print("Create document... ");

            if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
                Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
            else
                Serial.println(fbdo.errorReason());
        }

        String documentPath = "info/countries";
        String mask = "Singapore";

        // If the document path contains space e.g. "a b c/d e f"
        // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

        Serial.print("Get a document... ");

        if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), mask.c_str()))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());
    }
}

