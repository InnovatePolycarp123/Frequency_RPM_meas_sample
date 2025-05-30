
#define LPWM 4
#define RPWM 16
#define R_EN 19
#define L_EN 5
#define PWM_CHANNEL_0NE 0
#define PWM_CHANNEL_TWO 1
#define PWM_FREQUENCY 980
#define PWM_RESOLUTION 10
int time_constant_in_seconds = 3;
int time_elapsed = 0;
int duty = 0;
bool ramp_complete = false;
uint32_t previous_millis = 0;

const int MAX_DUTY_CYCLE = (int) (pow(2,10) - 1);

const int INITIAL_DUTY_CYCLE = 0.5 * MAX_DUTY_CYCLE;



void first_order_rise() {
  if (ramp_complete) return;

  while (!ramp_complete) {
    unsigned long current_millis = millis();

    if (current_millis - previous_millis >= 1000) {
      previous_millis = current_millis;

      duty = INITIAL_DUTY_CYCLE + (MAX_DUTY_CYCLE - INITIAL_DUTY_CYCLE) * (1 - exp(-time_elapsed / time_constant_in_seconds));
      ledcWrite(RPWM, duty);
      ledcWrite(LPWM , 0);

      time_elapsed += 1; // every increase of this is to show that one second passed. time elapsed =  time_constant_in_seconds at the third loop. at 5 times the time constant, 15s,  duty_cycle = 0.993.

      if (duty >= MAX_DUTY_CYCLE * 0.993) {
        duty = MAX_DUTY_CYCLE;
        ledcWrite(RPWM, duty);
       // ledcWrite(LPWM , 0);
        ramp_complete = true;
        
      }
    }
  }
}


void setup() {
  pinMode( R_EN , OUTPUT);
  pinMode( L_EN, OUTPUT);
  ledcAttachChannel(LPWM, PWM_FREQUENCY, PWM_RESOLUTION,  PWM_CHANNEL_0NE);
  ledcAttachChannel(RPWM, PWM_FREQUENCY, PWM_RESOLUTION,  PWM_CHANNEL_TWO );

  ledcWrite(LPWM , 0);
  ledcWrite(RPWM, 0);

  digitalWrite(R_EN , HIGH);
  digitalWrite(L_EN , HIGH);

}

void loop() {

 first_order_rise();

  // ledcWrite(LPWM , 0);
  // ledcWrite(RPWM, 1023);

  
  // for(int dutyCycle = 0; dutyCycle <= MAX_DUTY_CYCLE; dutyCycle++){   
  //   ledcWrite(RPWM,  dutyCycle);
  //   delay(100);
  // }

}
