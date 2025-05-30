const int external_control = 4;

ISR(TIMER1_COMPA_vect){
 digitalWrite(external_control, !digitalRead(external_control));

}
 
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  cli();
  pinMode(external_control, OUTPUT);
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 10;

  TCCR1B = 0b00001111;
  TIMSK1 = 0b00000010;
  sei();
}

void loop() {
  Serial.println("counter: " + String(TCNT1));

}
