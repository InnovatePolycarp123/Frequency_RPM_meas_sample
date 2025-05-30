volatile uint32_t captured = 0;


ISR(TIMER1_CAPT_vect) {
  captured = ICR1;
}
void setup() {
  Serial.begin(115200);
  noInterrupts();
  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;
  TCCR1B = 0b11000101;
  TIMSK1 = 0b00100000;
   interrupts();
}

void loop() {
  noInterrupts();
  Serial.println("Captured: " + String(captured));
    interrupts();
}
