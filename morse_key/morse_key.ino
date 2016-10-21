// Arduino morse key
// Control speed via 100k pot on pin A0
// Control tone via 100k pot on pin A1
// Connect DOT key to pin 7
// Connect DASH key to pin 8
// Connect buzzer to pin 4
// Connect relay to pin 13

// ============================================================
// Configuration

// T (ms) = 1200 / WPM

const unsigned int WPM_MIN = 5;
const unsigned int WPM_MAX = 40;

const unsigned int TONE_MIN = 400;
const unsigned int TONE_MAX = 1000;

// ============================================================

#define BTN_DOT 7
#define BTN_DASH 8

#define S_DOT 1
#define S_DASH 2

unsigned int SENDING = 0;
unsigned int LAST_SENT_SYMBOL = 0;
unsigned int SEND_HIGH_TIME = 0;
unsigned int SEND_LOW_TIME = 0;

unsigned int btn_dot_prev_st = 0;
unsigned int btn_dash_prev_st = 0;
unsigned int btn_dot_st = 0;
unsigned int btn_dash_st = 0;

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

unsigned long int rescale_analog_value(unsigned long int value, unsigned long int minv, unsigned long int maxv) {
  return minv + (value * (maxv - minv) / 1024UL);
}


/**
 * Read the speed potentiometer
 */
unsigned int read_speed_pot() {
  unsigned int current = analogRead(0);
  return rescale_analog_value(current, WPM_MIN, WPM_MAX);
}


/**
 * Read the tone potentiometer
 */
unsigned int read_tone_pot() {
  unsigned long int current = analogRead(1);
  return rescale_analog_value(current, TONE_MIN, TONE_MAX);
}


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(BTN_DOT, INPUT_PULLUP);
  pinMode(BTN_DASH, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("=====================");
  Serial.println("  Morse keyer is UP  ");
  Serial.println("=====================");
}


void send_dot() {
  SENDING = S_DOT;
  SEND_HIGH_TIME = DOT_LENGTH;
  SEND_LOW_TIME = DOT_LENGTH;
  LAST_SENT_SYMBOL = S_DOT;
  tone(4, tone_freq, DOT_LENGTH);
}

void send_dash() {
  SENDING = S_DASH;
  SEND_HIGH_TIME = DASH_LENGTH;
  SEND_LOW_TIME = DOT_LENGTH;
  LAST_SENT_SYMBOL = S_DASH;
  tone(4, tone_freq, DASH_LENGTH);
}

#define decrement(var, amount) var = amount > var ? 0 : var - amount


/**
 * Get the time elapsed during previous loop, in ms
 */
void update_prev_loop_len() {
  new_millis = millis();
  prev_loop_len = new_millis - prev_millis;
  prev_millis = new_millis;
}

void update_speed() {
  unsigned int new_speed = read_speed_pot();
  unsigned int new_dot_length = 1200 / new_speed;
  
  if (new_dot_length != DOT_LENGTH) {
    Serial.print("New speed: ");
    Serial.print(new_speed);
    Serial.print(" WPM (dot length: ");
    Serial.print(new_dot_length);
    Serial.println(" ms)");
  }
  DOT_LENGTH = new_dot_length;
}


void loop() {
  update_prev_loop_len();
  update_speed();

  // Get new tone frequency
  tone_freq = read_tone_pot();

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

    if (btn_dot_st && btn_dash_st) {
      // Both keys down: alternate dots and dashes
      if (LAST_SENT_SYMBOL == S_DOT) {
        send_dash();
      }
      else {
        send_dot();
      }
    }
    else if (btn_dot_st) {
      send_dot();
    }
    else if (btn_dash_st) {
      send_dash();
    }

  }

  delay(1);

}



