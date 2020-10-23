/* Zachary Weiss
** 23 Oct 2020
** Linear Rail DC Closed-Loop Control fo ME460 */
#define PITCH     8 // mm per rev
#define PULSES  374 // pulses per rev

// global settings and flags
// k_prop is coeff for proportional term
// k_deriv is coeff for derivative term
double k_prop     =  0.2,
       k_deriv    =  0.1;
bool   use_deriv  = true; // flag
int    targetSpd1 =   35, // mm per s
       targetSpd2 =   20,
       targetSpd3 =    6, // slowest speed possible
       targetSpd4 =   10,
       dist2      =   10, // mm
       dist4      =   30;

// global vars
volatile long int pulses = 0;
double v_motor           = 0, 
       current_rpm       = 0,
       err               = 0,
       err_old           = 0,
       d_err             = 0;

void setup() {
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(2),count,RISING);
  
  pinMode(12,OUTPUT); // Dir
  pinMode(3,OUTPUT);  // Current
  pinMode(9,OUTPUT);  // Break

  pinMode(7,INPUT);

  // limit switch
  pinMode(10,INPUT_PULLUP);

  digitalWrite(3,LOW);   // Motor off
  digitalWrite(9,LOW);   // Disable brake
  digitalWrite(12,LOW); // Set direction

  while(digitalRead(10)); // wait for user input
  delay(1000); // wait one second before beginning sequence
}

void loop() {
  int ct0 = 0;

  // towards motor
  digitalWrite(12,HIGH);
  while(digitalRead(10)) {
    setSpeed(mmToRPM(targetSpd1));
  }

  // away from motor
  ct0 = pulses;
  digitalWrite(12,LOW);
  while((ct0-pulses)<(dist2*PULSES/PITCH)) {
    setSpeed(mmToRPM(targetSpd2));
  }
  
  digitalWrite(12,HIGH);
  while(digitalRead(10)) {
    setSpeed(mmToRPM(targetSpd3));
  }
  
  ct0 = pulses;
  digitalWrite(12,LOW);
  while((ct0-pulses)<(dist4*PULSES/PITCH)) {
    setSpeed(mmToRPM(targetSpd4));
  }

  shutdown();
  while(1); // hold until reset
}

void count() {
  // Called on rising pulse on pin 2 (Encoder A)
  if (digitalRead(7)==HIGH)
    pulses--;
  else
    pulses++;
}

void setSpeed(double target_rpm) {
  // Output voltage to motor, wait for response, measure new speed
  analogWrite(3,int(v_motor));
  delay(100);
  current_rpm = getSpeed(100000); 

  // Print time [ms], voltage [0-255], speed [rpm], pulses
  Serial.println(String(millis())
    +"\t"
    +String(int(v_motor))
    +"\t"
    +String(current_rpm)
    +"\t"
    +String(pulses));

  err = target_rpm - current_rpm;
  // only calculate d_err if use_deriv is true, else 0
  d_err = use_deriv?(err - err_old):0;
  v_motor += k_prop*err + k_deriv*d_err;
  err_old = err;

  // cap voltage between 0 and 255
  v_motor = int((v_motor>255)?255:((v_motor<0)?0:v_motor));
}

double getSpeed(long int mcs) {
  unsigned long int t0           = 0,
                    delta_pulses = 0;
  // should never be neg, but to do math where the
  // result is neg, must make signed else result will
  // overflow, despite abs().
  long int initialPulses         = 0;
  double rpm                     = 0;
  
  initialPulses = pulses;
  t0 = micros();
  while(micros()-t0 < mcs){} // hold for sample time
  delta_pulses = abs(initialPulses-pulses);

  rpm = double(delta_pulses)/double(mcs)*1E+6 /double(PULSES) * 60.0;

  return(rpm);
}

double mmToRPM(int mmPerS) {
  // converts mm/s to RPM
  double rpm = mmPerS*60/PITCH;
  return(rpm);
}

void shutdown() {
  digitalWrite(3,LOW);
  digitalWrite(9,LOW);
  digitalWrite(12,LOW);
}
