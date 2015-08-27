void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(57600);
  Serial.println(WGM12, HEX);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Timer part
  OCR1A = 30000;  // 8Mhz/256/2Hz
  TCCR1B |= (1 << CS11); // prescaler 8
  TIMSK1 |= (1 << OCIE1A);

  // External interrupt part
  TCCR1B |= (1 << ICES1); // Rising edge
  TIMSK1 |= (1 << ICIE1); // Input Capture

  interrupts();
}

volatile int part = -1;
bool reading = false;

void loop() {
  // put your main code here, to run repeatedly:

}

ISR(TIMER1_COMPA_vect) {
  TCNT1 = 0;
  digitalWrite(13, digitalRead(13) ^ 1);
}

ISR(TIMER1_CAPT_vect) {
  Serial.println("HOI");
}

