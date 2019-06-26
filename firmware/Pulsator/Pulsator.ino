/* 
 *  Pulsator-Control v2.0
 *  
 *  (c) 2017 by latexdoc (https://fetlife.com/users/1444096)
 *  
 *  Features:
 *  - choosable frequency via potentiometer 
 *  - choosable duty-cycle
 *  - display status of device (Heartbeat) via LED
 * 
 *  
 *  Description hardware
 *  - outputpin
 *  
 *  Arduino-Pins
 *  D1:
 *  D2: Output valve for pulse hose
 *  D2: Output status-LED
 *  D4: 
 *  D5:
 *  D6:
 *  D7: Output heartbeat-LED 
 *  D8:
 *  D9:
 *  D10:
 *  D11:
 *  D12:
 *  D13: Output LED on PCB
 *  D14:
 *  A0:
 *  A2: Input Poti 1: Pulse-frequency (n/min)
 *  A1: Input Poti 2: Duty-Clycle in percent 
 *  
 */

// declaration constants
// config Pins
const int out_ventil = 2;       // Output magnet valve at Pin 2
const int out_LED_ventil = 3;   // Output LED-Magnet valve at Pin 2
const int out_hb = 7;           // Output Heartbeat-LED at Pin 7
const int out_led_pcb = 13;     // LED on PCB
const int in_f = A2;            // Input Poti1 (time) at Pin A1
const int in_dc = A1;           // Input Poti2 (Duty-Cycle) at Pin A2
const unsigned int min_f = 10;  // minimal time in ms (=0.5s)
const unsigned int max_f = 150; // maximum time in ms (=10s)
const unsigned int min_dc = 10; // minimal duty-cycle in percent(=10%)
const unsigned int max_dc = 90; // maximun duty-cycle in percent(=90%)

// global variables
int out_ventil_state = HIGH;    // init of status valve 
int out_LED_ventil_state = HIGH;    // init of status LED-valve
int out_led_pcb_state = LOW;    // init of HB-LED
unsigned int in_f_state = 0;    // value of analog input 1 - time
unsigned int in_dc_state = 0;   // value of analog input 2 - duty-cycle
unsigned int out_hb_state = 0;  // output state of HB-LED
unsigned int boot_millis = 0;   // keep time of system start

// needed stuff for heartbeat
int hb_interval = 1000;         // interval for heartbeat
int sample_interval = 200;      // interval for sampling the atem_input
long hb_previousMillis = 0;     // will store last time LED was updated
unsigned long f_verz_millis = 0;
unsigned long sample_currentMillis = 0;
unsigned long output_currentMillis = 0;
unsigned long output_previousMillis = 0;

void setup() {
  // set the pins
  pinMode(out_ventil, OUTPUT);  
  pinMode(out_LED_ventil, OUTPUT);    
  pinMode(out_hb, OUTPUT);
  pinMode(out_led_pcb, OUTPUT);
  boot_millis = millis();
  // config of UART
  Serial.begin(115200);  
}

void loop() //start the main loop
{

  // start of HB-function; toggle HB-LED every 1s 
  unsigned long hb_currentMillis = millis(); 
  if(hb_currentMillis - hb_previousMillis > hb_interval) {
    // save timestamp of last HB-LED toggle
    hb_previousMillis = hb_currentMillis;   
    // toggel state of HB-LED
    if (out_hb_state == LOW)
      out_hb_state = HIGH;
    else
      out_hb_state = LOW;
    // set hardware pin of HB-LED
   digitalWrite(out_hb, out_hb_state);
   digitalWrite(out_led_pcb, out_hb_state);
   // read analog inputs 
   int temp = 0;
   temp = analogRead(in_f);
   in_f_state = map (temp, 0, 850, min_f, max_f);
   delay(2);
   temp = analogRead(in_dc);
   in_dc_state = map(temp, 0, 850, min_dc, max_dc);
   delay(2);
   // end of analog read 
   // output some status information on the UART
   Serial.print("\n\nStatus:");
   Serial.print("\nFrequency: ");
   Serial.print(in_f_state);   
   Serial.print("\nDuty-Cycle (%: ");
   Serial.print(in_dc_state);      
  } // End of HB-function

 // calculate time and duty-cycle values and convert them into real on- and off-timing-values
 float tges = ((float)60/in_f_state)*1000; //total time in ms
 float ton = (float)in_dc_state*tges/100;  //t on in ms

 // switch the valve on and off
 unsigned long output_currentMillis = millis();
 if(output_currentMillis - output_previousMillis > (int)ton) {
  out_ventil_state = LOW;
  out_LED_ventil_state = LOW;
  digitalWrite(out_ventil, out_ventil_state);
  digitalWrite(out_LED_ventil, out_LED_ventil_state);
 }
 if(output_currentMillis - output_previousMillis > (int)tges) {
  output_previousMillis = output_currentMillis;  
  out_ventil_state = HIGH;
  out_LED_ventil_state = HIGH;
  digitalWrite(out_ventil, out_ventil_state);
  digitalWrite(out_LED_ventil, out_LED_ventil_state);
 }


  
}

