
#define led_control 4

volatile uint32_t last_interrupt_time;
double pulse_to_distance_ratio;
volatile uint32_t revolution_count;

struct timerOneMeasurementResults {

  uint32_t revolution_count;
  uint32_t number_of_pulses_in_current_revolution;
};

void TimerStartUp(uint32_t encoder_resolution);
double getPulseToDistanceRatio(uint16_t encoder_gear_ratio, uint16_t encoder_resolution, double wheel_diameter_in_meter);
double getDistance(double pulse_to_distance_ratio, uint16_t encoder_resolution);
double getDistanceFromRevolutionCountOnly(double pulse_to_distance_ratio, uint16_t encoder_resolution);
struct timerOneMeasurementResults getTimerCountOneCurrentValue();
void resetRevolutionCountIfWheelHasStopped(uint32_t revolution_reset_time_after_system_stops_in_micro_seconds);




void setup() {
  Serial.begin(2000000);
  pinMode(led_control, OUTPUT);

  TimerStartUp(8);
  pulse_to_distance_ratio = getPulseToDistanceRatio(48, 8, 0.06);
  Serial.print("pulse_to_distance_ratio: ");
  Serial.println(pulse_to_distance_ratio, 8);
}

void loop() {

  static uint32_t last_print_time = 0;

  uint32_t current_millis = millis();

  if ((current_millis - last_print_time) >= 300) {

    last_print_time = current_millis;

    double distance_one = getDistance(pulse_to_distance_ratio, 8);
    // double distane_two = getDistanceFromRevolutionCountOnly(pulse_to_distance_ratio, 8);

    Serial.print("distance_one: ");
    Serial.println(distance_one, 8);

    if (distance_one  >= 5){

      digitalWrite(led_control, 1);
    }

    // Serial.print("distane_two: ");
    // Serial.println(distane_two, 8);
  }




  resetRevolutionCountIfWheelHasStopped(300000000);
}




void TimerStartUp(uint32_t encoder_resolution) {

  TCCR1B = 0;
  TCCR1A = 0;
  TCNT1 = 0;
  /*    reset all previous timers */

  TCCR1B = 0b00001111;         // set timer one to ctc mode and and external clock
  TIMSK1 = 0b00000010;         // enable output compare interrupt on output compare channel A;
  OCR1A = encoder_resolution;  // set the output compare number to resolution,
                               //so each compare interrupt equals one revolution
}

ISR(TIMER1_COMPA_vect) {
  last_interrupt_time = micros();
  revolution_count++;
}

struct timerOneMeasurementResults getTimerCountOneCurrentValue() {

  uint8_t status_register;
  struct timerOneMeasurementResults temporal_timer_one_results;

  status_register = SREG;

  cli();
  temporal_timer_one_results.number_of_pulses_in_current_revolution = TCNT1;
  temporal_timer_one_results.revolution_count = revolution_count;

  SREG = status_register;

  return temporal_timer_one_results;
}

double getDistance(double pulse_to_distance_ratio, uint16_t encoder_resolution) {


  struct timerOneMeasurementResults timer_one_measurement_results = getTimerCountOneCurrentValue();

  uint32_t total_number_of_pulses = (timer_one_measurement_results.revolution_count * encoder_resolution) + timer_one_measurement_results.number_of_pulses_in_current_revolution;


  double distance_in_meters = double(total_number_of_pulses) * pulse_to_distance_ratio;

  Serial.print("total_number_of_pulses: ");
  Serial.println(double(total_number_of_pulses), 8);

  return distance_in_meters;
}

double getDistanceFromRevolutionCountOnly(double pulse_to_distance_ratio, uint16_t encoder_resolution) {

  cli();
  uint32_t safe_to_use_resolution_count = revolution_count;
  sei();

  uint32_t number_of_pulses = safe_to_use_resolution_count * encoder_resolution;
  Serial.print("number_of_pulses: ");
  Serial.println(number_of_pulses, 8);
  double distance = double(number_of_pulses) * pulse_to_distance_ratio;


  return distance;
}

void resetRevolutionCountIfWheelHasStopped(uint32_t revolution_reset_time_after_system_stops_in_micro_seconds) {

  cli();
  uint32_t safe_to_use_last_interrupt_time = last_interrupt_time;
  uint32_t current_micros = micros();
  sei();

  if (current_micros - safe_to_use_last_interrupt_time >= revolution_reset_time_after_system_stops_in_micro_seconds) {
    cli();
    revolution_count = 0;
    TCNT1 = 0;
    sei();

    Serial.println("Wheel stopped. Revolution count reset.");
  }
}



double getPulseToDistanceRatio(uint16_t encoder_gear_ratio, uint16_t encoder_resolution, double wheel_diameter_in_meter) {

  uint16_t wheel_revolution_to_encoder_pulse_ratio;  // this tells us how many encoder pulses equal one wheel revolution

  /*

    NOTE: by wheel, i mean the wheel of the motor tire  we want to measure distance from. encoder_gear_ratio represents the gear ratio of our encoder to the wheel. if encoder ratio equals 48, it means:

    wheel revolution = 1, encoder_gear_ratio = 48. for every wheel revolution, we have 48 encoder revolutions. 

    Now, if each encoder revolution produces 8 pulses, one wheel revolution causes 48 encoder revolutions, which means we'll get 8 * 48 pulses.

    for example:

      if one encoder revolution equal (encoder resolution) = 8, and for every wheel revolution we have 48 encoder revolution ( encoder_gear_ratio). 

      then, one wheel revolution equal = encoder resolution * encoder_gear_ratio. = 48 * 8 = 384.

      that's to say; if each encoder revolution equals eight pulses. one wheel revolution = 384 pulse, which is the wheel_revolution_to_encoder_pulse_ratio 
    
    */

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  wheel_revolution_to_encoder_pulse_ratio = encoder_gear_ratio * encoder_resolution;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // now we'll calculate cicumference of the wheel.

  double wheel_circumference = PI * wheel_diameter_in_meter;

  // the circumference tell us the distance covered by each wheel revolution. for example, assuming diameter of 60mm, circumference is 185mm. so each wheel revolution equals 185mm distance covered.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // now we'll calculate what each encoder pulse means in terms of distance covered. that's pulse_to_distance ratio;


  double pulse_to_distance_ratio = wheel_circumference / (double)wheel_revolution_to_encoder_pulse_ratio;

  /*          Think of it this way:
                       
              using our previous examples, each wheel revolution equals the movement of a complete wheel_circumference (distance_covered). 
              As a result, every time we have one wheel revolution (185mm covered), the encoder produces wheel_revolution_to_encoder_pulse_ratio ( 384 pulses).

              if 185mm = 384 pulses, 
                 x_in_mm = 1 pulse 

                 x_in_mm  =  185/384

            NOTE:     i just used mm in this example, in can be Meters. in fact, I am using meters
                                                             
  */


  return pulse_to_distance_ratio;
}
