//============== SENSOR 1 (FILTERED) ========
#define trig1 18
#define echo1 19

//============ SENSOR 2 (RAW) ===========
#define trig2 21
#define echo2 22

const int tinggi_tangki = 54;
const int blindspot = 20;

// Buffer moving average SENSOR 1
int history[5] = {0};
int idx = 0;

void setup() {
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  Serial.begin(9600);
  Serial.println("=== Dual JSN-SR04T Status Monitor ===");
  delay(2000);
}

// ================= SENSOR 1 (FILTER) =================
int bacaJarakFiltered() {
  int samples[7];
  int valid = 0;

  for (int i = 0; i < 7; i++) {
    digitalWrite(trig1, LOW);
    delayMicroseconds(5);
    digitalWrite(trig1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig1, LOW);

    long dur = pulseIn(echo1, HIGH, 30000);
    int jarak = dur * 0.034 / 2;

    if (jarak > blindspot && jarak < tinggi_tangki) {
      samples[valid++] = jarak;
    }
    delay(70);
  }

  if (valid == 0) return -1;

  // Median
  for (int i = 0; i < valid - 1; i++) {
    for (int j = i + 1; j < valid; j++) {
      if (samples[j] < samples[i]) {
        int tmp = samples[i];
        samples[i] = samples[j];
        samples[j] = tmp;
      }
    }
  }
  int median = samples[valid / 2];

  // Moving average
  history[idx] = median;
  idx = (idx + 1) % 5;

  long total = 0;
  for (int i = 0; i < 5; i++) total += history[i];
  return total / 5;
}

// ================= SENSOR 2 (RAW) =================
int bacaJarakRaw() {
  digitalWrite(trig2, LOW);
  delayMicroseconds(5);
  digitalWrite(trig2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig2, LOW);

  long dur = pulseIn(echo2, HIGH, 30000);
  if (dur == 0) return -1;

  int jarak = dur * 0.034 / 2;
  if (jarak < blindspot || jarak > tinggi_tangki) return -1;

  return jarak;
}

// ================= STATUS AIR =================
String statusAir(int tinggi_air) {
  if (tinggi_air < 0) return "MINUS";
  if (tinggi_air >= 26) return "AIR PENUH";
  else if (tinggi_air >= 17) return "AIR TINGGI";
  else if (tinggi_air >= 9) return "AIR SEDANG";
  else if (tinggi_air >= 0) return "AIR RENDAH";
  else return "KOSONG";
}

void loop() {
  int jarak1 = bacaJarakFiltered();
  delay(120);
  int jarak2 = bacaJarakRaw();

  int tinggi_air1 = (jarak1 == -1) ? -1 : tinggi_tangki - jarak1;
  int tinggi_air2 = (jarak2 == -1) ? -1 : tinggi_tangki - jarak2;

  String status1 = statusAir(tinggi_air1);
  String status2 = statusAir(tinggi_air2);

  Serial.print("Sensor1 Filter=");
  if (tinggi_air1 == -1) Serial.print("ERR");
  else Serial.print(String(tinggi_air1) + "cm");
  Serial.print(" ");
  Serial.print(status1);

  Serial.print(" | ");

  Serial.print("Sensor2 Raw=");
  if (tinggi_air2 == -1) Serial.print("ERR");
  else Serial.print(String(tinggi_air2) + "cm");
  Serial.print(" ");
  Serial.print(status2);

  Serial.println(".");

  delay(10000);
}
