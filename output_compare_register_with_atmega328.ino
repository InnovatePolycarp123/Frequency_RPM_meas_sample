
#define LED_PIN 4
ISR(TIMER1_COMPA_vect){

 digitalWrite(LED_PIN, !digitalRead(LED_PIN));

}

void setup() {

 pinMode(LED_PIN, OUTPUT);
 Serial.begin(9600);

noInterrupts();
 TCCR1A = 0;
 TCCR1B = 0;
 TCNT1 = 0;
 OCR1A = 7812;
 TCCR1B = 0b00001100;
 TIMSK1 = 0b00000010;
 interrupts();
 

}

void loop() {
  Serial.println("HIGH");
  delay(1000);
  Serial.println("LOW");
   delay(1000);

}
