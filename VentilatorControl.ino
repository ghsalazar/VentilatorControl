/// @author Gastón H. Salazar Silva <ghsalazar@ipn.mx>
/// @brief  VentilatorControl is a timer for controlling electrovalves.

enum Mode {TEST, PRODUCTION};
enum AutomatonState {INHALE,PAUSE_INHALE,EXHALE,PAUSE_EXHALE};
enum ElectrovalveState {OFF, ON};

const Mode      MODE    = TEST;
const unsigned  SECOND  = 0xC2F7;

const uint8_t OFF_TIME     = A0;
const uint8_t ON_TIME      = A1;
const uint8_t EXHALE_VALVE = 12;
const uint8_t INHALE_VALVE = 11;

AutomatonState automaton;
bool onTimerEnd;

void setup() {
  int value;
  
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(EXHALE_VALVE,OUTPUT);
  pinMode(INHALE_VALVE,OUTPUT);

  automaton = INHALE;
  value = readPotentiometer(ON_TIME);
  timer(value*SECOND);
  electrovalve(INHALE_VALVE, ON);
}

void loop() {
  int value;
  
  if (automaton==INHALE && onTimerEnd) {
    automaton = PAUSE_INHALE;
    value = readPotentiometer(OFF_TIME);
    timer(value*SECOND);
    electrovalve(INHALE_VALVE, OFF);
  }
  else if (automaton==PAUSE_INHALE && onTimerEnd) {
    automaton = EXHALE;
    value = readPotentiometer(ON_TIME);
    timer(value*SECOND);
    electrovalve(EXHALE_VALVE, ON);
  }
  else if (automaton==EXHALE && onTimerEnd) {
    automaton = PAUSE_EXHALE;
    value = readPotentiometer(OFF_TIME);
    timer(value*SECOND);
    electrovalve(EXHALE_VALVE, OFF);
  }
  else if (automaton==PAUSE_EXHALE && onTimerEnd) {
    automaton = INHALE;
    value = readPotentiometer(ON_TIME);
    timer(value*SECOND);
    electrovalve(INHALE_VALVE, ON);
  }
}

void timer(unsigned time) {
  onTimerEnd = false;
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1<<CS10)|(1 << CS12);
  TCNT1 = time*SECOND;
  TIMSK1 |= (1 << TOIE1);
}

ISR(TIMER1_OVF_vect) {
  onTimerEnd = true;
}

void electrovalve(uint8_t valve, ElectrovalveState state) {
  if (state == ON) {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(valve, LOW);
  } 
  else if (state == OFF) {
    digitalWrite(LED_BUILTIN, HIGH);    
    digitalWrite(valve, HIGH);    
  }
}

int readPotentiometer(uint8_t pin) {
  int value;
  if (MODE == TEST) {
    if (pin == A0)
      value = 1;
    else if (pin == A1)
      value = 2;
  }
  else if (MODE == PRODUCTION)
    value = analogRead(pin);
  return value;
}
