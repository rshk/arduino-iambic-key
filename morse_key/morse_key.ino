// Arduino morse key
// Control speed via 100k pot on pin A0
// Control tone via 100k pot on pin A1
// Connect DOT key to pin 7
// Connect DASH key to pin 8
// Connect buzzer to pin X
// Connect relay to pin 13

#define BTN_DOT 7
#define BTN_DASH 8

#define S_DOT 1
#define S_DASH 2

unsigned int SENDING = 0;
unsigned int SEND_HIGH_TIME = 0;
unsigned int SEND_LOW_TIME = 0;

unsigned int btn_dot_prev_st = 0;
unsigned int btn_dash_prev_st = 0;
unsigned int btn_dot_st = 0;
unsigned int btn_dash_st = 0;

unsigned int BTN_DOT_ST = 0;
unsigned int BTN_DASH_ST = 0;

unsigned int DOT_LENGTH = 200; // ms
#define DASH_LENGTH (DOT_LENGTH * 3)
#define SPACE_LENGTH (DOT_LENGTH * 7)

unsigned long prev_loop_len = 0;
unsigned long prev_millis = 0;
unsigned long new_millis = 0;

unsigned int tone_freq = 800;


unsigned int btn_dot_status() {
  return !digitalRead(BTN_DOT);
}

unsigned int btn_dash_status() {
  return !digitalRead(BTN_DASH);
}


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(BTN_DOT, INPUT_PULLUP);
  pinMode(BTN_DASH, INPUT_PULLUP);

  Serial.begin(9600);
}


void send_dot() {
  SENDING = S_DOT;
  SEND_HIGH_TIME = DOT_LENGTH;
  SEND_LOW_TIME = DOT_LENGTH;
}

void send_dash() {
  SENDING = S_DASH;
  SEND_HIGH_TIME = DASH_LENGTH;
  SEND_LOW_TIME = DOT_LENGTH;
}

#define decrement(var, amount) var = amount > var ? 0 : var - amount


void loop() {

  // Calculate duration of previous loop
  new_millis = millis();
  prev_loop_len = new_millis - prev_millis;
  prev_millis = new_millis;

  DOT_LENGTH = 10 + analogRead(0);

  // Get new tone frequency
  tone_freq = 200 + analogRead(1);

  // Keep sending..
  if (SENDING) {
    //Serial.println("SENDING");
    if (SEND_HIGH_TIME) {
      digitalWrite(LED_BUILTIN, HIGH);
      decrement(SEND_HIGH_TIME, prev_loop_len);
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
      if (SEND_LOW_TIME) {
        decrement(SEND_LOW_TIME, prev_loop_len);
      }
      else {
        SENDING = 0;
      }
    }
  }
  else {
    btn_dot_st = btn_dot_status();
    btn_dash_st = btn_dash_status();

    if (btn_dot_st) {
      Serial.println("DOT");
      send_dot();
      tone(4, tone_freq, DOT_LENGTH);
    }
    else if (btn_dash_st) {
      Serial.println("DASH");
      send_dash();
      tone(4, tone_freq, DASH_LENGTH);
    }

  }

  delay(1);

}



