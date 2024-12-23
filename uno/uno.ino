#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 8
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int DO = 6;
int ledRedPin = 9;
int ledGreenPin = 5;
int buzzerPin = 10;
int button1Pin = 7;
int button2Pin = 4;
int pirPin = 11;
int rel1Pin = 12;
int rel2Pin = 3;
int rstESP = 13;

// Flag
int pirState = LOW;
bool relay1 = 0;
bool relay2 = 0;
bool flagPir = 0;
int flagS = 0;
bool flagSmartHome = 0;
bool alarm = 0;
bool Continue = 0;
// Data
int but1 = 0;
int but2 = 0;
int pot_t2 = 0;
int ldr_t2 = 0;
int temp_t2 = 0;
int hum_t2 = 0;
int mq9_t2 = 0;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(button1Pin, INPUT);
  pinMode(rel1Pin, OUTPUT);
  pinMode(rel2Pin, OUTPUT);
  pinMode(DO, INPUT);
  pinMode(rstESP, OUTPUT);
  dht.begin();
  digitalWrite(rel1Pin, HIGH);
  digitalWrite(rel2Pin, HIGH);
  digitalWrite(rstESP, LOW);
  delay(500);
  digitalWrite(rstESP, HIGH);
  digitalWrite(ledGreenPin, HIGH);
}

void loop() {
  ldr_t2 = LDR();
  temp_t2 = temp();
  hum_t2 = hum();
  mq9_t2 = Mq9();
  int but1 = digitalRead(button1Pin);
  int but2 = digitalRead(button2Pin);

  if (mq9_t2 > 204) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledRedPin, HIGH);
    delay(1000);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledRedPin, LOW);
    delay(1000);
  }

  if (but1 == 1) {
    flagPir = flagPir + 1;
    delay(200);
  }
  if (but2 == 1) {
    flagS = flagS + 1;
    delay(200);
  }

  if (flagPir == 1) {
    ServiceBurglarAlarm(5, 1000);
  }
  if (flagS == 1) {
    smartHomeService();
  } else if (flagS > 1) {
    if (relay1 == 1) {
      digitalWrite(rel1Pin, LOW);
    } else {
      digitalWrite(rel1Pin, HIGH);
    }
    if (relay2 == 1) {
      digitalWrite(rel2Pin, LOW);
    } else {
      digitalWrite(rel2Pin, HIGH);
    }
    flagS = 0;
  }
  delay(100);
}

int temp() {
  return dht.readTemperature();
}

int hum() {
  return dht.readHumidity();
}


int LDR() {
  int data = analogRead(A2);
  return data;
}

void pir() {
  int val = digitalRead(pirPin);
  if (val == HIGH && pirState == LOW) {
    pirState = HIGH;
  } else if (val == LOW && pirState == HIGH) {
    pirState = LOW;
  }
}
float Mq9() {
  float sensorValue = analogRead(A1);
  return sensorValue;
}

void ServiceBurglarAlarm(int n, int del) {
  pir();

  if (pirState == HIGH || Continue == 1) {
    if (pirState == HIGH) {
      Continue = 1;
    }
    if (flagPir == 1) {
      for (int i = 0; i < n; i++) {
        alarm = 1;
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(ledRedPin, HIGH);
        delay(del);
        digitalWrite(buzzerPin, LOW);
        digitalWrite(ledRedPin, LOW);
        delay(del);
      }
    } else {
      alarm = 0;
      digitalWrite(ledGreenPin, HIGH);
      delay(1000);
      digitalWrite(ledGreenPin, LOW);
      delay(1000);
    }
  }
}
void smartHomeService() {
  int lightLevel = ldr_t2;
  int currentTemp = temp_t2;
  int currentHum = hum_t2;

  if (flagSmartHome == 0) {
    // کنترل بر اساس LDR
    if (lightLevel < 70) {
      digitalWrite(rel1Pin, LOW);  // روشن کردن چراغ
    } else {
      digitalWrite(rel1Pin, HIGH);  // خاموش کردن چراغ
    }
  } else if (flagSmartHome == 1) {
    // کنترل بر اساس PIR
    pir();
    if (pirState == HIGH) {
      digitalWrite(rel1Pin, LOW);  // روشن کردن چراغ.
      delay(1500);
    } else {
      digitalWrite(rel1Pin, HIGH);  // خاموش کردن چراغ
    }
  }

  if (currentTemp > 30) {        // اگر دما بیشتر از 30 درجه باشد
    digitalWrite(rel2Pin, LOW);  // روشن کردن سیستم سرمایشی
  } else if (currentTemp < 18) {  // اگر دما کمتر از 18 درجه باشد
    digitalWrite(rel2Pin, HIGH);  // خاموش کردن سیستم سرمایشی
  }
}

void requestEvent() {
  String data = "T:" + String(temp_t2) + ",H:" + String(hum_t2) + ",L:" + String(ldr_t2) + ",G:" + String(mq9_t2) + ",A:" + String(alarm);
  Wire.write(data.c_str(), min(data.length(), 32));  // ارسال حداکثر 32 بایت
  Serial.println(data);
}

void receiveEvent(int howMany) {
  while (Wire.available()) {
    char command = Wire.read();
    if (command == 'L') {
      relay1 = 1;
      digitalWrite(rel1Pin, LOW);  // روشن کردن رله لامپ
    } else if (command == 'l') {
      relay1 = 0;
      digitalWrite(rel1Pin, HIGH);  // خاموش کردن رله لامپ
    } else if (command == 'C') {
      relay2 = 1;
      digitalWrite(rel2Pin, LOW);  // روشن کردن سیستم سرمایشی
      relay2 = 0;
    } else if (command == 'c') {
      digitalWrite(rel2Pin, HIGH);  // خاموش کردن سیستم سرمایشی
    } else if (command == 'A') {
      alarm = 0;
      flagPir = 1;  // فعال کردن دزدگیر
      Serial.println("Active Sevice");
    } else if (command == 'a') {
      alarm = 0;
      flagPir = 0;  // غیرفعال کردن دزدگیر
      Continue = 0;
      Serial.println("deactive Sevice");
    } else if (command == '1') {  // فعال کردن حالت LDR
      Serial.println("Smart Home: LDR Mode");
      flagSmartHome = 0;  // LDR فعال
      Serial.println(String(flagSmartHome));
      flagS = 1;
    } else if (command == '2') {  // فعال کردن حالت PIR
      Serial.println("Smart Home: PIR Mode");
      flagSmartHome = 1;  // PIR فعال
      Serial.println(String(flagSmartHome));
      flagS = 1;
    } else if (command == 's') {  // غیرفعال کردن اسمارت هوم
      Serial.println("Deactivating Smart Home");
      flagS = 2;
    }
  }
}