#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

# define piezoPin A0
# define micPin A1
# define variance_sample_size 1000
# define data_sample_size 3000
# define piezo_variance_threshold 50000
# define mic_variance_threshold 5000
# define send_data_threshold 3// number of consecutive gestures to trigger an action

// wifi module
const char* ssid     = "UW MPSK";
const char* password = "X[gfy]{77["; // Replace with your password received from UW MPSK

// serverURL
const char* serverURL = "https://omnisurface.azurewebsites.net/api/esp_rawdata_process";
String jsonPayload;


// sensor data
int piezoData[variance_sample_size]; // store the piezo sensor data
int micData[variance_sample_size]; // store the bone conduction microphone data
// int combinedData[2][data_sample_size]; // store the combined data
int validdataCount = 0; // number of stored data
bool gestureDetected = false; // flag to indicate if a gesture is detected

// JSON object size: Adjust according to your data size
const size_t jsonCapacity = JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(data_sample_size) * 2;
StaticJsonDocument<jsonCapacity> jsonDoc;


double computeVariance(int values[], int length) {
  double sum = 0.0;
  double sumSquare = 0.0;
  for (int i = 0; i < length; i++) {
    sum += values[i];
    sumSquare += values[i] * values[i];
  }
  double mean = sum / length;
  double variance = (sumSquare / length) - (mean * mean);
  return variance;
}


void setup() {
  Serial.begin(115200);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);

  Serial.println("Setup complete, loading model...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

}

void loop() {

  for (int i = 0; i < variance_sample_size; i++) {
    int piezo = analogRead(piezoPin);
    int mic = analogRead(micPin);
    piezoData[i] = piezo;
    micData[i] = mic;
  }

  // compute the variance of the data
  double variancePiezo = computeVariance(piezoData, variance_sample_size);
  double varianceMic = computeVariance(micData, variance_sample_size);
  // Serial.print("Variance of piezo sensor data: ");
  // Serial.println(variancePiezo);
  // Serial.print("Variance of bone conduction microphone data: ");
  // Serial.println(varianceMic);



  // 检测方差是否连续超过阈值
  if (variancePiezo > piezo_variance_threshold && varianceMic > mic_variance_threshold) {
    gestureDetected = true;
    

    // Serial.println("Gesture detected!");
    
  } 

  // 当连续超过阈值的次数达到设定次数时触发手势检测
  if (gestureDetected && validdataCount < send_data_threshold) {
    // Serial.println("Recording data!");
    digitalWrite(A2, HIGH); 

    JsonArray piezoArray = jsonDoc["piezo"].isNull() ? jsonDoc.createNestedArray("piezo") : jsonDoc["piezo"];
    JsonArray micArray = jsonDoc["mic"].isNull() ? jsonDoc.createNestedArray("mic") : jsonDoc["mic"];
            
    
    for (int i = 0; i < variance_sample_size; i++) {
      // combinedData[0][validdataCount*1000 + i] = piezoData[i];
      // combinedData[1][validdataCount*1000 + i] = micData[i];
      // Serial.print(validdataCount*1000 + i);
      // Serial.print(": ");
      // Serial.print(combinedData[0][validdataCount*1000 + i]);
      // Serial.print(" - ");
      Serial.print(piezoData[i]);
      Serial.print(",");
      // Serial.println(combinedData[1][validdataCount*1000 + i]);
      // Serial.print(combinedData[1][validdataCount*1000 + i]);
      // Serial.print(" - ");
      Serial.println(micData[i]);

      piezoArray.add(piezoData[i]);
      micArray.add(micData[i]);
    }
    validdataCount ++;
  } 
  else if (gestureDetected && validdataCount == send_data_threshold) {
    // Make the POST request
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Start the HTTP connection
        http.begin(serverURL);
        http.addHeader("Content-Type", "application/json");

        String jsonPayload;
        serializeJson(jsonDoc, jsonPayload);

        // Send POST request and get the response
        int httpResponseCode = http.POST(jsonPayload);
        Serial.println("Sending POST request...");
        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        } else {
            Serial.println("Error on sending POST: " + String(httpResponseCode));
        }

        // End the connection
        http.end();

    } else {
        Serial.println("WiFi not connected");
    }
      
    validdataCount = 0;
    gestureDetected = false;
    jsonDoc.clear();
    digitalWrite(A2, LOW);  // Turn on the LED

  }
  // else {
  //   Serial.println("No gesture.");
  //   validdataCount = 0;
  //   gestureDetected = false;
  // }
}


// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
// #include <Arduino.h>
// #include <WiFi.h>

// void setup(){
//   Serial.begin(115200);
//   while(!Serial);
//   delay(1000);
//   Serial.println();
//   Serial.print("ESP Board MAC Address:  ");
//   Serial.println(WiFi.macAddress());
// }
 
// void loop(){
// 	Serial.println(WiFi.macAddress());
// 	delay(1000);
// }