const int pinBuzzer = 4;
const int pinEcho = 5;
const int pinTrigger = 6;
const int pinButton = 7;
const int pinLed = 9;

const int delayInit = 500;
const int delayAlarm = 100;
const int distanciaMinima = 150;

// const int tonos[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};
const int tonos[] = {261, 261, 294, 261, 331, 311};
const int countTonos = 10;

int delayLed = delayInit;
long distancia = 9999;

int val = 0;
int state = 0;
int oldVal = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinTrigger, OUTPUT);
  pinMode(pinButton, INPUT);
}

void loop() {
  distancia = measure(pinTrigger, pinEcho);
  if (distancia > 0 && distancia < distanciaMinima) {
    delayLed = delayAlarm;
    ring(pinBuzzer, 200);
  }
  if (delayLed == delayAlarm) {
    val = digitalRead(pinButton);
    if ((val == HIGH)) {
      Serial.println("Alarma desconectada");
      delayLed = delayInit;
      noTone(pinBuzzer);
      delay(5000);
    }
  }
  blink(pinLed, delayLed);
}

void blink(int port, int sleepTime) {
  digitalWrite(port, HIGH);
  delay(sleepTime);
  digitalWrite(port, LOW);
  delay(sleepTime);   
}

int measure(int TriggerPin, int EchoPin) {
  long duration, distanceCm;
  digitalWrite(TriggerPin, LOW); //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH); //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);
  duration = pulseIn(EchoPin, HIGH); //medimos el tiempo entre pulsos, en microsegundos
  distanceCm = duration * 10 / 292 / 2; //convertimos a distancia, en cm
  Serial.println(distanceCm);
  return distanceCm;
}

int ring(int pinBuzzer, int delayRing) {
  tone(pinBuzzer, tonos[0]);
  // for (int iTono = 0; iTono < countTonos; iTono++) {
  //   tone(pinBuzzer, tonos[iTono]);
  //   delay(delayRing);
  // }
}
