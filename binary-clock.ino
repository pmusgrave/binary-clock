#include <RTClib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <FastLED.h>

#define NUM_LEDS 20
CRGB leds[NUM_LEDS];

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define DHTPIN 2
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
DateTime now;
sensors_event_t temp_reading;
int LED_counter = 0;

void setup () {
  FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS);
  
  Serial.begin(9600);

  delay(3000); // wait for console opening

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {
    Handle_DS3231();
    Handle_AM2302();
    Handle_LEDs();
    delay(100);
}

void Handle_DS3231(){
    now = rtc.now();

    if(1){//if(now.minute() % 15 == 0){
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(") ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();

      //Serial.print("Temperature (DS3231): ");
      //Serial.print(rtc.getTemperature());
      //Serial.println(" C");
  
      //Serial.println();
    }
}

void Handle_AM2302(){
  if(1){//if(now.minute() % 15 == 0){
    // Delay between measurements.
    //delay(delayMS);
    // Get temperature event and print its value.
    sensors_event_t event;
    temp_reading = event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
    }
    else {
      Serial.print(F("Temperature (AM2301): "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
    }
  
    Serial.println();
  }
}

void Handle_LEDs(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  
  CRGB color;// = CHSV(temp_reading.temperature + temp_reading.relative_humidity, 255, 255);
  double val;
  val = ((temp_reading.temperature - 18)*30);
  if(val > 255) val = 255;
  color.r = val;

  val = ((temp_reading.relative_humidity - 60)* 11);
  if(val > 255) val = 255;
  color.g = val;
  
  color.b = 255 - now.hour()*10;

  int binary_digits[4] = {0,0,0,0};
  BinaryDigitsFromNumber(binary_digits, (int)now.hour()/10);
  Serial.println(binary_digits[1]);
  leds[0] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[1] = (binary_digits[1] == 1) ? color : CRGB::Black;

  BinaryDigitsFromNumber(binary_digits, (int)now.hour()%10);
  leds[2] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[3] = (binary_digits[1] == 1) ? color : CRGB::Black;
  leds[4] = (binary_digits[2] == 1) ? color : CRGB::Black;
  leds[5] = (binary_digits[3] == 1) ? color : CRGB::Black;

  BinaryDigitsFromNumber(binary_digits, (int)now.minute()/10);
  leds[6] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[7] = (binary_digits[1] == 1) ? color : CRGB::Black;
  leds[8] = (binary_digits[2] == 1) ? color : CRGB::Black;

  BinaryDigitsFromNumber(binary_digits, (int)now.minute()%10);
  leds[9] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[10] = (binary_digits[1] == 1) ? color : CRGB::Black;
  leds[11] = (binary_digits[2] == 1) ? color : CRGB::Black;
  leds[12] = (binary_digits[3] == 1) ? color : CRGB::Black;

  BinaryDigitsFromNumber(binary_digits, (int)now.second()/10);
  leds[13] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[14] = (binary_digits[1] == 1) ? color : CRGB::Black;
  leds[15] = (binary_digits[2] == 1) ? color : CRGB::Black;

  BinaryDigitsFromNumber(binary_digits, (int)now.second()%10);
  leds[16] = (binary_digits[0] == 1) ? color : CRGB::Black;
  leds[17] = (binary_digits[1] == 1) ? color : CRGB::Black;
  leds[18] = (binary_digits[2] == 1) ? color : CRGB::Black;
  leds[19] = (binary_digits[3] == 1) ? color : CRGB::Black;
  
  FastLED.show(); //delay(30);
}

void BinaryDigitsFromNumber(int* output, int number){
  output[0] = 0;
  output[1] = 0;
  output[2] = 0;
  output[3] = 0;
  switch(number){
  case 1:
    output[0] = 1;
    break;
  case 2:
    output[1] = 1;
    break;
  case 3:
    output[0] = 1;
    output[1] = 1;
    break;
  case 4:
    output[2] = 1;
    break;
  case 5:
    output[0] = 1;
    output[2] = 1;
    break;
  case 6:
    output[1] = 1;
    output[2] = 1;
    break;
  case 7:
    output[0] = 1;
    output[1] = 1;
    output[2] = 1;
    break;
  case 8:
    output[3] = 1;
    break;
  case 9:
    output[0] = 1;
    output[3] = 1;
    break;
  }
}
