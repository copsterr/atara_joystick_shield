/* Pin Declaration ---------- */
#define PIN_ANALOG_X 0
#define PIN_A        2
#define PIN_B        3
#define PIN_C        4
#define PIN_D        5
#define PIN_E        6
#define PIN_F        7
#define PIN_K        8
#define PIN_PWM      9

#define PWM_MIDPOINT 127

/* PV ----------------------- */
uint8_t drive_state = 0x00;
uint8_t pwm_out     = PWM_MIDPOINT;
uint8_t pwm_ceil[3] = {40, 70, 126}; // used for speed limiting
uint8_t *ceil_ptr   = pwm_ceil;
uint16_t pwm_steer  = 0;
//uint16_t steer_ang = 0;

void setup() {
  Serial.begin(115200);

  // Initializes Peripherals
  pinMode(PIN_A, INPUT_PULLUP); // reverse
  pinMode(PIN_B, INPUT_PULLUP); // brake
  pinMode(PIN_C, INPUT_PULLUP); // accelerate
  pinMode(PIN_D, INPUT_PULLUP); // brake
  pinMode(PIN_E, INPUT_PULLUP); // change gear up
  pinMode(PIN_F, INPUT_PULLUP); // change gear down
  pinMode(PIN_K, INPUT_PULLUP); // not used
}

void loop() {
  // read drive btns
  if (!digitalRead(PIN_C)) drive_state |= 0x01; // foward
  if (!digitalRead(PIN_A)) drive_state |= 0x02; // reverse
  if (!digitalRead(PIN_D) || !digitalRead(PIN_B)) drive_state |= 0x03; // brake

  // read change gear btns
  if (!digitalRead(PIN_E)) drive_state |= 0x10; // gear up
  if (!digitalRead(PIN_F)) drive_state |= 0x20; // gear down

  // read x analog
  pwm_steer = analogRead(PIN_ANALOG_X);

/* Process Drive State Begin ---------- */

  /* Process Main Powertrain --------- */
if ((drive_state & 0x0F) == 0x01) { // fwd
  pwm_out++; // accelerate
  if (pwm_out > (PWM_MIDPOINT + *ceil_ptr)) 
    pwm_out = (PWM_MIDPOINT + *ceil_ptr); // speed limiting
}
else if ((drive_state & 0x0F) == 0x02) { // rev
  pwm_out--; // decelerate
  if (pwm_out < (PWM_MIDPOINT - *ceil_ptr)) 
    pwm_out = (PWM_MIDPOINT - *ceil_ptr); // speed limiting
}
else if (((drive_state & 0x0F) >= 0x03 && pwm_out != PWM_MIDPOINT) || drive_state == 0x00) { // brake
  if (pwm_out < PWM_MIDPOINT) {
    pwm_out++;
  } else if (pwm_out > PWM_MIDPOINT) {
    pwm_out--;
  }
}

  /* Process Gear Changing ---------- */
if ((drive_state & 0x10) == 16) { // gear up
  if (ceil_ptr != (&pwm_ceil[2])) ceil_ptr++;
  delay(200);
}
else if ((drive_state & 0x20) >= 32) { // gear down
  if (ceil_ptr != pwm_ceil) ceil_ptr--;
  delay(200);
}

/* Process Drive State End ---------- */

  Serial.print("Drive State: ");
  Serial.print(drive_state);
  Serial.print("  pwm_out: ");
  Serial.print(pwm_out);
  Serial.print("  pwm_ceil: ");
  Serial.print(*ceil_ptr);
  Serial.print("  pwm_steer: ");
  Serial.print(pwm_steer);
  Serial.println();

  // pwm output pin
  analogWrite(PIN_PWM, pwm_out);  
  
  // reset drive_state
  drive_state = 0;

  delay(40);
}
