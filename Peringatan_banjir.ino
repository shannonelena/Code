#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "Galaxy A55 5G E13D";
const char* password = "jangan dibaca";
String BOTtoken = "8436242263:AAHmIECqjYe137bJBp_gEy71_i5v-_cDqSA";
String chat_id = "1489756383";


const int trigPin = 13;
const int echoPin = 12;


const int ledHijau = 19;
const int ledKuning = 18;
const int ledMerah = 5;


const int buzzerPin = 2;

long duration;
float distance;
bool sudahKirimBahaya = false;
bool sudahKirimSiaga = false; 

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledHijau, OUTPUT);
  pinMode(ledKuning, OUTPUT);
  pinMode(ledMerah, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void sendTelegram(String pesan) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + BOTtoken +
                 "/sendMessage?chat_id=" + chat_id +
                 "&text=" + pesan;
    http.begin(url);
    int httpResponseCode = http.GET();
    http.end();
    Serial.println("Pesan terkirim ke Telegram");
  }
}

void loop() {
  // Baca sensor ultrasonik
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  Serial.print("Jarak air: ");
  Serial.print(distance);
  Serial.println(" cm");

// Matikan semua indikator terlebih dahulu
  digitalWrite(ledHijau, LOW);
  digitalWrite(ledKuning, LOW);
  digitalWrite(ledMerah, LOW);
  digitalWrite(buzzerPin, LOW);

  // Kondisi 1: Aman (> 19 cm)
  if (distance > 19) {
    digitalWrite(ledHijau, HIGH);
    sudahKirimBahaya = false;
    sudahKirimSiaga = false;
  }
  // Kondisi 2: Siaga (10 - 19 cm)
  else if (distance > 10 && distance <= 19) { 
    digitalWrite(ledKuning, HIGH);

    if (!sudahKirimSiaga) {
      sendTelegram("🟠 Siaga! Jarak air: " + String(distance) + " cm");
      sudahKirimSiaga = true;
    }

    sudahKirimBahaya = false;
  }
  // Kondisi 3: Bahaya (≤ 10 cm)
  else { // <-- Kondisi ini otomatis mencakup (distance <= 15)
    digitalWrite(ledMerah, HIGH);
    digitalWrite(buzzerPin, HIGH);

    if (!sudahKirimBahaya) {
      sendTelegram("⚠️ Bahaya! Jarak air: " + String(distance) + " cm");
      sudahKirimBahaya = true;
    }

    sudahKirimSiaga = false;
  }

  delay(200);
}
