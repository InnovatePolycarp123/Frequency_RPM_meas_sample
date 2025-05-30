#define OVERFLOW_OUT 4

void setup() {
  pinMode(OVERFLOW_OUT, OUTPUT);


  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1B = 0b00000101;
  TIMSK1 = 0b00000001;
}

void loop() {
  // put your main code here, to run repeatedly:
}

ISR(TIMER1_OVF_vect) {
  digitalWrite(OVERFLOW_OUT, !digitalRead(OVERFLOW_OUT));

}
