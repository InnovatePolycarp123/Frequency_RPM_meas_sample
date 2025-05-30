#include <avr/wdt.h>
#include <EEPROM.h>

#define LED_TEST 4
bool watchdogTriggered = false;
int eeprom_reset_flag = 0;
volatile uint32_t increasing_value = 0;
volatile uint32_t eeprom_recovery_address = 2;
volatile uint32_t eeprom_reset_flag_address = 0;




enum watchDogTime {

  TIME_16_MILLISECONDS,
  TIME_32_MILLISECONDS,
  TIME_64_MILLISECONDS,
  TIME_125_MILLISECONDS,
  TIME_250_MILLISECONDS,
  TIME_500_MILLISECONDS,
  TIME_1_SECOND,
  TIME_2_SECONDS,
  TIME_4_SECONDS,
  TIME_8_SECONDS

};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void turnOffWatchDogTimer();
void WatchDogTimerWithInterruptSetup(enum watchDogTime TIME);
////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup() {
  Serial.begin(115200);
  pinMode(LED_TEST, OUTPUT);
  WatchDogTimerWithInterruptSetup(TIME_8_SECONDS);
  //turnOffWatchDogTimer();
  
  // EEPROM.get(eeprom_reset_flag_address, eeprom_reset_flag);
  // uint32_t store_temp_recovery;

  // if(eeprom_reset_flag == 1){

  //    eeprom_reset_flag = 0;
  //    EEPROM.get(eeprom_recovery_address, store_temp_recovery);
  //    EEPROM.put(eeprom_reset_flag_address,  eeprom_reset_flag);
  //    increasing_value =  store_temp_recovery;

  // }


  Serial.println("length of eeprom: " + String(EEPROM.length()));
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("not working");
  //wdt_reset();
  increasing_value++;

  Serial.println("Increasing Value" + String(increasing_value));


  if (watchdogTriggered) {
    Serial.println("Watchdog interrupt triggered");

    while (1);
  }
}


ISR(WDT_vect) {

  watchdogTriggered = true;
  //eeprom_reset_flag = 1;
  digitalWrite(LED_TEST, !digitalRead(LED_TEST));

  // EEPROM.put(eeprom_recovery_address, increasing_value++);
  // EEPROM.put(eeprom_reset_flag_address, eeprom_reset_flag);
}


void WatchDogTimerWithInterruptSetup(enum watchDogTime TIME) {

  cli();
  wdt_reset();
  MCUSR &= ~(1 << WDRF);

  WDTCSR |= (1 << WDCE) | (1 << WDE);  // set WDCE and WDE to one
  //- this allows you to change watchdog timer prescalar within 4 clock cycles;
  switch (TIME) {

    case TIME_16_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE);
      break;

    case TIME_32_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP0);
      break;

    case TIME_64_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP1);
      break;

    case TIME_125_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP0) | (1 << WDP1);
      break;

    case TIME_250_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2);
      break;

    case TIME_500_MILLISECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP0);
      break;

    case TIME_1_SECOND:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP1);
      break;

    case TIME_2_SECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
      break;

    case TIME_4_SECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3);
      break;

    case TIME_8_SECONDS:
      WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3) | (1 << WDP0);
      break;
  }

  sei();
}


void turnOffWatchDogTimer() {

  cli();
  wdt_reset();

  MCUSR &= ~(1 << WDRF);  // reset the wdrf flag to allow safe turn off of watchdog timer

  WDTCSR |= (1 << WDCE) | (1 << WDE);  //  set WDCE and WDE to one
  //- this allows you to change watchdog timer prescalar within 4 clock cycles

  WDTCSR = 0x00;  // turnoff watchdog timer

  sei();
}