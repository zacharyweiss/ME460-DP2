/* Zachary Weiss
** 23 Oct 2020
** Linear Rail DC Closed-Loop Control fo ME460 */
#define PITCH 8; // mm per rev

// global settings
// k is coefficient for proportional control
double k          = 0.05; 
int    targetRPM1 = 35, // mm per s
       targetRPM2 = 20,
       targetRPM3 = 10, // slowest speed possible
       targetRPM4 = 10,
       dist2      = 10, // mm
       dist4      = 30;

// global vars
volatile long int pulses = 0;
double v_motor           = 0, 
       current_rpm       = 0,
       dist_ct           = 0,
       err               = 0;

void setup() {
    attachInterrupt(digitalPinToInterrupt(2),count,RISING);
    
    pinMode(12,OUTPUT); // Dir
    pinMode(3,OUTPUT);  // Current
    pinMode(9,OUTPUT);  // Break
  
    pinMode(7,INPUT);

    // limit switch
    pinMode(10,INPUT_PULLUP);

    digitalWrite(3,LOW); // Motor off
    digitalWrite(9,LOW); // Disable break
    digitalWrite(12,HIGH);  // Set direction
    Serial.begin(115200);

    while(digitalRead(10)); // wait for user input
    delay(1000); // wait one second before beginning sequence
}

void loop() {

  // Output voltage to motor, wait for response, measure new speed
  analogWrite(3,int(v_motor));
  current_rpm=getSpeed(10000); 

  // Print time [mcs], voltage [0-255], speed [rpm]
  Serial.println(String(millis())+"\t"+String(int(v_motor))+"\t"+String(current_rpm));

  err = target_rpm - current_rpm;
  v_motor += k*err;

  v_motor = (v_motor>255)?255:((v_motor<0)?0:v_motor);
  delay(100);

  /*
  t0=micros();
  ct=0;
  while(micros()-t0<1E+6){
    v2=digitalRead(7);
    if (v2>v1)
      ct++;
    v1=v2;
  }
  w=ct/12*60;  // Cycles per minute, or r.p.m.         
 
  Serial.println(w);
  analogWrite(3,0);
  digitalWrite(9,HIGH);
  while(true);*/

}

void count() {
  // Called on rising pulse on pin 2 (Encoder A)
  if (digitalRead(7)==HIGH)
    pulses--;
  else
    pulses++;
}

double getSpeed(long int mcs) {
  unsigned long int t0=0, initialPulses=0, delta_pulses=0;
  double rpm=0;
  
  initialPulses=pulses;
  t0=micros();
  while(micros()-t0<mcs){} // hold for sample time
  delta_pulses = pulses - initialPulses;

  rpm=double(delta_pulses)/double(mcs)*1E+6 /374.0 * 60.0; // 374 pulses/rev, 60 seconds per min

  return(rpm);
}

double mmToRPM(int mmPerS) {
  // converts mm/s to RPM
  double rpm = mmPerS*60/PITCH
  return rpm
}
