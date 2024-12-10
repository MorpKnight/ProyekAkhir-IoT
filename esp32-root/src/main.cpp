#define BLYNK_TEMPLATE_ID "TMPL63SlNIGYO"
#define BLYNK_TEMPLATE_NAME "Finpro Iot"
#define BLYNK_AUTH_TOKEN "RAaA6R1RBTvdOT5o6VUgJPOpLGB2BgBB"
#define BLYNK_PRINT Serial

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>

// #define WIFI_SSID "DESKTOP-5AJS436 4673"
// #define WIFI_PASSWORD "081281083891"

#define WIFI_SSID "Giovan Christoffel's A54"
#define WIFI_PASSWORD "ebmagab2kyeavhb"


#define BUZZER_PIN 16
#define LED_PIN 4

String serverName = "http://192.168.212.220:5000";

BlynkTimer timer;
HTTPClient http;
TaskHandle_t alarmHandler = NULL;

int alarm_state = 0;
int detectetion_state = 0;

typedef struct response {
    String imageUrl;
    bool isDetected;
} Response;


String getNewResponse();
Response parsePayload(String payload);
void alarmTask(void *param);
void sendNewImage();

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

BLYNK_WRITE(V1) {
    if(detectetion_state){
        alarm_state = param.asInt();
        Blynk.virtualWrite(V1, alarm_state);

        Serial.print("Alarm State: ");
        if(alarm_state == 1){
            Serial.println("ON");
            xTaskCreate(alarmTask, "Alarm Task", 1024, NULL, 1, &alarmHandler);
        }

        else {
            Serial.println("OFF");
            vTaskDelete(alarmHandler);
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.disconnect();

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
    timer.setInterval(5000L, sendNewImage); 
}

void loop() {
    Blynk.run();
    timer.run();
}

void alarmTask(void *param){
    while(1){
        tone(BUZZER_PIN, 562, 200);
        digitalWrite(LED_PIN, HIGH);

        vTaskDelay(250 / portTICK_PERIOD_MS);

        tone(BUZZER_PIN, 262, 200);
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void sendNewImage(){
    String payload = getNewResponse();
    Response data = parsePayload(payload);

    Blynk.setProperty(V2, "url", 1, data.imageUrl);
    Blynk.virtualWrite(V0, data.isDetected);

    detectetion_state = data.isDetected;
}

String getNewResponse(){
    String payload = "";

    Serial.println("[HTTP] begin...");
    if(http.begin(serverName + "/access")){
        Serial.print("[HTTP] GET...");
        int httpCode = http.GET();

        if(httpCode > 0){
            Serial.printf(" code: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payload = http.getString();
            }
        }
        else {
            Serial.printf( "failed, error : %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        Serial.println("[HTTP] closed...");
    }
    else {
        Serial.printf("[HTTP] Unable to connect\n");
    }

    return payload;
}

Response parsePayload(String payload){
    StaticJsonDocument<512> jsonBuffer;
    deserializeJson(jsonBuffer, payload);

    Response newResponse;

    newResponse.imageUrl = jsonBuffer["url"].as<String>();  
    newResponse.isDetected = jsonBuffer["detected"].as<int>();

    return newResponse;
}