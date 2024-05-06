#include <Arduino.h>

# define piezoPin A0
# define micPin A1
# define variance_sample_size 1000
# define data_sample_size 3000
# define piezo_variance_threshold 50000
# define mic_variance_threshold 5000
# define send_data_threshold 3// number of consecutive gestures to trigger an action



int piezoData[variance_sample_size]; // store the piezo sensor data
int micData[variance_sample_size]; // store the bone conduction microphone data
int combinedData[2][data_sample_size]; // store the combined data
int validdataCount = 0; // number of stored data
bool gestureDetected = false; // flag to indicate if a gesture is detected


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
    
    for (int i = 0; i < variance_sample_size; i++) {
      combinedData[0][validdataCount*1000 + i] = piezoData[i];
      combinedData[1][validdataCount*1000 + i] = micData[i];
      // Serial.print(validdataCount*1000 + i);
      // Serial.print(": ");
      Serial.print(combinedData[0][validdataCount*1000 + i]);
      // Serial.print(" - ");
      // Serial.print(piezoData[i]);
      Serial.print(",");
      Serial.println(combinedData[1][validdataCount*1000 + i]);
      // Serial.print(combinedData[1][validdataCount*1000 + i]);
      // Serial.print(" - ");
      // Serial.println(micData[i]);
    }
    validdataCount ++;
  } 
  else if (gestureDetected && validdataCount == send_data_threshold) {
    // sendData();
    // for (int i = 0; i < data_sample_size; i++) {
    //   Serial.print(combinedData[0][i]);
    //   Serial.print(",");
    //   Serial.println(combinedData[1][i]);
    // }

    // Serial.println("Data sent!");
    
    validdataCount = 0;
    gestureDetected = false;
    digitalWrite(A2, LOW);  // Turn on the LED

  }
  // else {
  //   Serial.println("No gesture.");
  //   validdataCount = 0;
  //   gestureDetected = false;
  // }
}
