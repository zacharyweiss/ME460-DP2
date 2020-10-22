void setup() {
    pinMode(12,OUTPUT); // Dir
    pinMode(3,OUTPUT);  // Current
    pinMode(9,OUTPUT);  // Break
  
    pinMode(7,INPUT);
    Serial.begin(115200);

    digitalWrite(3,LOW);
    digitalWrite(9,LOW);
    digitalWrite(12,HIGH);    
}


int v2=0, v1=0;
unsigned long int t0=0, ct=0;
double w=0;
void loop() {
  analogWrite(3,180);
  delay(10000);
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
  while(true);

}
