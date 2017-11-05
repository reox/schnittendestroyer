// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sensmin = 9999;
int sensmax = -1;
int compensation = 0;

// this constant is pi * (12.5 / 2)^2 * 1/10000
const double c_force = 0.0122718463;

int readSensorMbar(){
  // Honeywell 40PC250G pressure sensor
  // resolution of analog pin ~4.9mV
  // sensor gives 16mV/psi...
  
  // Analog input
  // 5V ... 1023
  // 0V ... 0
  
  // Sensor signal
  // 4V   ... 250 psi = 819 analog value
  // 0.5V ... 0   psi = 102 analog value
  
  // and with 1 mbar = psi * 68.9476:
  // (sensorValue - 102) / (819 - 102) * 250 * 68.9476 = mbar
  // = (sensorValue - 102) * 24.0403 ~= 24 * (sensorValue - 102)
  
  long res = 0L;
  
  for(int i = 0; i < 128; i++){
    res += analogRead(A0);
  }
  
  // divide by 128
  res = res >> 7;
  
  return (24 * (res - 102)) - compensation;
}
  
  

int getCompensation(){
  // basically the same as readSensorMbar, but a lot more times...
  
  long res = 0L;
  for (int i = 0; i < 256; i++){
    res += readSensorMbar();
  }
  res = res >> 8;
  
  return res & 0xFFFF;
}



void setup() {
    lcd.begin(16, 2);
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  lcd.setCursor(1,0);
  lcd.print("calibrating...");
  compensation = getCompensation();
  
  lcd.setCursor(0,1);
  lcd.print(String("comp.: ") + String(compensation));
  
  delay(2000);
  
  lcd.clear();
  
}

void loop() {
  // It takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times a second. 
  int sens = readSensorMbar();
  
  if (sens > sensmax) {
    sensmax = sens;
  }
  if (sens < sensmin) {
    sensmin = sens;
  }
  
  Serial.println(readSensorMbar());
  // calculate the force
  lcd.setCursor(0, 0);
  lcd.print("F cur: ");
  lcd.print(sens * c_force);
  lcd.print("      ");
  lcd.setCursor(15, 0);
  lcd.print("N");

  lcd.setCursor(0, 1);
  lcd.print("F max: ");
  lcd.print(sensmax * c_force);
  lcd.print("      ");
  lcd.setCursor(15, 1);
  lcd.print("N");



  delay(1);        // delay in between reads for stability
}


