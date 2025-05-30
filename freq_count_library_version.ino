
#include <FreqCount.h>

void setup() {
  Serial.begin(115200);
  FreqCount.begin(1000);

}

void loop() {
  if ( FreqCount.available()){
    uint32_t count = FreqCount.read();
    Serial.println("Frequency:" + String(count));

  }
}
