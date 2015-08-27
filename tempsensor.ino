void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(8, INPUT);
  pinMode(13, OUTPUT);
  pinMode(9, INPUT);
  digitalWrite(9, LOW);
  Serial.begin(57600);
  Serial.println(WGM12, HEX);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Timer part
  TCCR1B |= (1 << CS11); // prescaler 8
  TIMSK1 |= (1 << OCIE1A);

  // External interrupt part
  TCCR1B |= (1 << ICES1); // Rising edge
  TCCR1B |= (1 << ICNC1); // Noise canceller
  TIMSK1 |= (1 << ICIE1); // Input Capture

  interrupts();
}

volatile int part = -1;
uint8_t reading = 0;
uint8_t expected[8] = {HIGH, LOW, LOW, HIGH, LOW, HIGH, LOW, HIGH};

void loop() {
  // put your main code here, to run repeatedly:

}

volatile uint8_t D1 = 0x00;
volatile uint8_t D2 = 0x00;
volatile uint8_t D3 = 0x00;
volatile uint8_t D4 = 0x00;

// Timer elapsed interrupt (read next bit)
ISR(TIMER1_COMPA_vect) {
  if (reading == 0) {
    if (digitalRead(8) == HIGH) {
      reading = 2;
      OCR1A = bitTime(reading);
      digitalWrite(9, HIGH);
    }
  }
  else if (reading < 8)  {
    if (digitalRead(8) == expected[reading-1]) {
      // Process data
      processData(reading);
      // continue
      reading++;
      OCR1A = bitTime(reading);
      digitalWrite(9, digitalRead(8));
    } else {
      reading = 0; // lost sync
    }
  }
  else if (reading == 8 && digitalRead(8) == expected[reading-1]) {
    Serial.print(seg2decimal(D1), DEC);
    Serial.print(seg2decimal(D2), DEC);
    Serial.print(seg2decimal(D3), DEC);
    if (D3 & 0x01) {
      Serial.print(".");
    }
    Serial.println(seg2decimal(D4), DEC);
    D1 = 0x00;
    D2 = 0x00;
    D3 = 0x00;
    D4 = 0x00;
    reading = 1;
    OCR1A = bitTime(reading);
    digitalWrite(9, digitalRead(8));
  } else {
    reading = 0;
  }
}

ISR(TIMER1_CAPT_vect) {
  if (reading == 0) { // not tracking anything
    TCNT1 = 0;
    OCR1A = bitTime(1);
  }
  if (reading == 8) { // resync
    TCNT1 = 0;
    OCR1A = bitTime(8);
  }
}

uint16_t bitTime(uint8_t nr){
  if (nr == 8) {
    nr = 0;
  }
  return 1000 + nr * 1900;
}

void processData(uint8_t nr) {
  uint8_t R2 = digitalRead(2);
  uint8_t R3 = digitalRead(3);
  uint8_t R4 = digitalRead(4);
  uint8_t R5 = digitalRead(5);
  uint8_t R6 = digitalRead(6);
  uint8_t R7 = digitalRead(7);
  uint8_t R8 = digitalRead(9);
  if (nr == 1) {
    if (R3 == LOW) { D4 |= (1 << 4); }
    if (R5 == LOW) { D3 |= (1 << 4); }
    if (R7 == LOW) { D2 |= (1 << 4); }
    //
    if (R4 == LOW) { D3 |= (1 << 0); } // decimal point
  }
  if (nr == 3) {
    if (R2 == LOW) { D4 |= (1 << 5); }
    if (R4 == LOW) { D3 |= (1 << 5); }
    if (R6 == LOW) { D2 |= (1 << 5); }
    if (R8 == LOW) { D1 |= (1 << 5); }
    //
    if (R3 == LOW) { D4 |= (1 << 3); }
    if (R5 == LOW) { D3 |= (1 << 3); }
    if (R7 == LOW) { D2 |= (1 << 3); }
  }
  if (nr == 5) {
    if (R2 == LOW) { D4 |= (1 << 6); }
    if (R4 == LOW) { D3 |= (1 << 6); }
    if (R6 == LOW) { D2 |= (1 << 6); }
    if (R8 == LOW) { D1 |= (1 << 6); }
    //
    if (R3 == LOW) { D4 |= (1 << 1); }
    if (R5 == LOW) { D3 |= (1 << 1); }
    if (R7 == LOW) { D2 |= (1 << 1); }
  }
  if (nr == 7) {
    if (R2 == LOW) { D4 |= (1 << 7); }
    if (R4 == LOW) { D3 |= (1 << 7); }
    if (R6 == LOW) { D2 |= (1 << 7); }
    if (R8 == LOW) { D1 |= (1 << 7); }
    //
    if (R3 == LOW) { D4 |= (1 << 2); }
    if (R5 == LOW) { D3 |= (1 << 2); }
    if (R7 == LOW) { D2 |= (1 << 2); }
  }
}

uint8_t seg2decimal(uint8_t hex) {
  switch (hex & 0xFE) { // mask to get rid of the point
    case 0xFC: return 0;
    case 0x60: return 1;
    case 0xDA: return 2;
    case 0xF2: return 3;
    case 0x66: return 4;
    case 0xB6: return 5;
    case 0xBE: return 6;
    case 0xE4: return 7;
    case 0xFE: return 8;
    case 0xF6: return 9;
    default: return 0;
  }
}

