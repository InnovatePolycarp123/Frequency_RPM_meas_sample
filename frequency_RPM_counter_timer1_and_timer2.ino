
volatile uint32_t Timer_one_overflow_count = 0;
volatile uint32_t overflow_count_for_frequency_calculation = 0;  // a copy of Timer_one_overflow_count to avoid racing condition
volatile uint32_t count_period = 1000;
volatile uint32_t milliSecondsCount = 0;
volatile uint32_t frequency = 0; // frequency in this case represents CPS (counts per seconds). By count we mean number of pulses per second.
volatile bool freq_counting_finished = false;
bool counting_started = false;
uint32_t count_time = 0; // the variable used to measure the duration of all continuos total ferquency count resets some minutes after frequency equals zero.
uint32_t count_time_reset_duration =300000 ; // this is the set seconds that dictates how long we wait before resetting countime after frequency equals zero
/*
 60000ms = 1s
 60000 * 5 = 300000ms = 300 seconds
  60 seconds = 1 minute;
  300 seconds = 5 minutes
*/
uint32_t previous_millis_count_time_reset_duration = 0;




struct Time {
  uint8_t hour;
  uint8_t minutes;
  uint8_t seconds;
};



void startCount();
uint32_t RPM_Count(uint32_t frequency, uint16_t resolution);
struct Time displayTime(uint32_t elapsedTimeInMs);


void setup() {
  Serial.begin(115200);
  startCount();
}

void loop() {


  // if (!counting_started) {
  //   startCount();
  //   counting_started = true;
  // }


  if (freq_counting_finished) {

    uint32_t current_millis_count_time_reset_duration = millis();

    uint32_t RPM = RPM_Count(frequency, 1200);

    if(( frequency == 0 ) && ((current_millis_count_time_reset_duration - previous_millis_count_time_reset_duration) >  count_time_reset_duration ) ){

      previous_millis_count_time_reset_duration = current_millis_count_time_reset_duration;

      count_time  = 0;
    }

   

    struct Time time_to_display = displayTime(count_time);

    Serial.print("HH:" + String(time_to_display.hour));
    Serial.print(" MM:" + String(time_to_display.minutes));
    Serial.println(" SS:" + String(time_to_display.seconds));
  

    Serial.println("Frequency: " + String(frequency));
    Serial.println("RPM: " + String(RPM));

    Serial.println("done");

    freq_counting_finished = false;
    //   counting_started = false;
  }
}


void startCount() {

  Timer_one_overflow_count = 0;
  milliSecondsCount = 0;
  freq_counting_finished = false;

  cli();
  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  ////////////////////// RESET ALL REGISTERS

  TCCR1B = 0b00000111;  // set timer one for external clock on rising edge
  TIMSK1 = 0b00000001;  // enable overflow interrupt on timer one
  TCCR2A = 0b00000010;  // enable clear timer on compare wave generation (CTC) mode on timer 2
  TCCR2B = 0b00000101;  // set timer two prescalar to 128: this sets timer frequency to 125,000
  TIMSK2 = 0b00000010;  // enable timer 2 output compare interrupt A
  OCR2A = 124;
  sei();

  ///////////// RECONFIGURE REGISTERS
}

ISR(TIMER1_OVF_vect) {

  Timer_one_overflow_count++;
}

ISR(TIMER2_COMPA_vect) {

  milliSecondsCount++;
  overflow_count_for_frequency_calculation = Timer_one_overflow_count;

  if (milliSecondsCount > count_period) {

    frequency = (overflow_count_for_frequency_calculation * 65536) + TCNT1;

    if (frequency > 0){
      count_time += count_period;
    }
   
    TCNT1 = 0;
    freq_counting_finished = true;
    Timer_one_overflow_count = 0;
    milliSecondsCount = 0;
  }
}


uint32_t RPM_Count(uint32_t frequency, uint16_t resolution) {

  uint32_t RPM;  // revolution per minute
  uint32_t RPS;  // revolution per second

  RPS = frequency / resolution;  // calculate revoultion per secon

  RPM = RPS * 60UL;  // calculate revolution per minute

  return RPM;

  /* this RPM calculation can be done in one line: RPM = ((frequency* 60)/ resolution) */
}


struct Time displayTime(uint32_t elapsedTimeInMs) {

  static uint32_t time = 0;

  time = elapsedTimeInMs/1000UL; // convert from milliseconds to seconds

  struct Time time_to_display;

  time_to_display.hour = time / 3600;

  time_to_display.minutes = (time % 3600) / 60;

  time_to_display.seconds  = time % 60;

  return time_to_display;
}



