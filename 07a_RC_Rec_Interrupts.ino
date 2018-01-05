#include "RC_Rec_Interrupts.h"

void setup() {
  // choices: 2 (PortB) || 11 (PortK)
  // PortB: Ard 10 to 13, 50 to 53 (total 8 pins)
  // PortK: Ard A8 to A15 (total 8 pins)
  
  RC_Rec_Int_Enable(11); 
  
  Serial.begin(9600);
}

void loop () {
  // RC cmds are entered via interrupts
  RC_Rec_Int_Print_Inputs_Pk();

}


void RC_Rec_Int_PortB_Enable() {
  PCICR |= (1 << PCIE0);  // enable interrupt

  // Inform Mask Register which Pin on the physical board will cause an interrupt
  PCMSK0 |= (1 << PCINT0); 
  PCMSK0 |= (1 << PCINT1);
  PCMSK0 |= (1 << PCINT2);
  PCMSK0 |= (1 << PCINT3);
  PCMSK0 |= (1 << PCINT4);  
}

void RC_Rec_Int_PortK_Enable() {
  // enable interrupt
  PCICR |= (1 << PCIE2);

 // Inform Mask Register which Pin on the physical board will cause an interrupt
  PCMSK2 |= (1 << PCINT16); // Ard A8
  PCMSK2 |= (1 << PCINT17); // Ard A9
  PCMSK2 |= (1 << PCINT18); // Ard A10
  PCMSK2 |= (1 << PCINT19); // Ard A11
  PCMSK2 |= (1 << PCINT20); // Ard A12  
}

void RC_Rec_Int_Enable(byte whichPort) {
  switch(whichPort) {
    case 2:
              RC_Rec_Int_PortB_Enable();
              break;
    case 11:
              RC_Rec_Int_PortK_Enable();              
              break;
    default:
              break;
  }
}

ISR(PCINT0_vect) {
  // this is the interrupt vector that will run when PortB pins (PCINT0 to 7)

  RC_Rec_Int_Timer_Pb[0] = micros();    // THE Reference timer whenever this ISR is triggered (time stamp)
  
  // now we need to check which pin changed its state
  // "RC_Rec_Int_Last_Channel[0] == 0" - Last channel value was LOW ,  // "PINB & B00000001" - New Channel value is HIGH
  // i.e. did Pin state changed from LOW to HIGH ?
  // PINB method is similar to using digitalRead() but it is a lot faster
  
  // Channel 1 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pb[0] == 0 && PINB & B00000001) {
    // yes, channel has risen to HIGH state
    RC_Rec_Int_Last_Channel_Pb[0] = 1; 
    RC_Rec_Int_Timer_Pb[1] = RC_Rec_Int_Timer_Pb[0];
  } 
  else if(RC_Rec_Int_Last_Channel_Pb[0] == 1 && !(PINB & B00000001)) { 
    // last channel is HIGH and New Channel is !HIGH(i.e.LOW)
    RC_Rec_Int_Last_Channel_Pb[0] = 0;
    // time elapsed since channel changed from HIGH to LOW
    RC_Rec_Int_Input_Pb[0] = RC_Rec_Int_Timer_Pb[0] - RC_Rec_Int_Timer_Pb[1]; 
  }

  // Channel 2 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pb[1] == 0 && PINB & B00000010) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pb[1] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pb[2] = RC_Rec_Int_Timer_Pb[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pb[1] == 1 && !(PINB & B00000010)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pb[1] = 0;
    RC_Rec_Int_Input_Pb[1] = RC_Rec_Int_Timer_Pb[0] - RC_Rec_Int_Timer_Pb[2];
  }

  // Channel 3 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pb[2] == 0 && PINB & B00000100) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pb[2] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pb[3] = RC_Rec_Int_Timer_Pb[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pb[2] == 1 && !(PINB & B00000100)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pb[2] = 0;  // set to LOW
    RC_Rec_Int_Input_Pb[2] = RC_Rec_Int_Timer_Pb[0] - RC_Rec_Int_Timer_Pb[3];
  }

  // Channel 4 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pb[3] == 0 && PINB & B00001000) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pb[3] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pb[4] = RC_Rec_Int_Timer_Pb[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pb[3] == 1 && !(PINB & B00001000)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pb[3] = 0;  // set to LOW
    RC_Rec_Int_Input_Pb[3] = RC_Rec_Int_Timer_Pb[0] - RC_Rec_Int_Timer_Pb[4];
  }

  // Channel 5 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pb[4] == 0 && PINB & B00010000) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pb[4] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pb[5] = RC_Rec_Int_Timer_Pb[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pb[4] == 1 && !(PINB & B00010000)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pb[4] = 0;  // set to LOW
    RC_Rec_Int_Input_Pb[4] = RC_Rec_Int_Timer_Pb[0] - RC_Rec_Int_Timer_Pb[5];
  }
}

ISR(PCINT2_vect) {
  // this is the interrupt vector that will run when PortK pins PCINT16 (ArdPin 8) to PCTINT23 (ArdPin 15)
  RC_Rec_Int_Timer_Pk[0] = micros();
  
  // Channel 1 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pk[0] == 0 && PINK & B00000001) {
    // yes, channel has risen to HIGH state
    RC_Rec_Int_Last_Channel_Pk[0] = 1; 
    RC_Rec_Int_Timer_Pk[1] = RC_Rec_Int_Timer_Pk[0];
  } 
  else if(RC_Rec_Int_Last_Channel_Pk[0] == 1 && !(PINK & B00000001)) { 
    // last channel is HIGH and New Channel is !HIGH(i.e.LOW)
    RC_Rec_Int_Last_Channel_Pk[0] = 0;
    // time elapsed since channel changed from HIGH to LOW
    RC_Rec_Int_Input_Pk[0] = RC_Rec_Int_Timer_Pk[0] - RC_Rec_Int_Timer_Pk[1]; 
  }

  // Channel 2 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pk[1] == 0 && PINK & B00000010) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pk[1] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pk[2] = RC_Rec_Int_Timer_Pk[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pk[1] == 1 && !(PINK & B00000010)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pk[1] = 0;
    RC_Rec_Int_Input_Pk[1] = RC_Rec_Int_Timer_Pk[0] - RC_Rec_Int_Timer_Pk[2];
  }

  // Channel 3 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pk[2] == 0 && PINK & B00000100) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pk[2] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pk[3] = RC_Rec_Int_Timer_Pk[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pk[2] == 1 && !(PINK & B00000100)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pk[2] = 0;  // set to LOW
    RC_Rec_Int_Input_Pk[2] = RC_Rec_Int_Timer_Pk[0] - RC_Rec_Int_Timer_Pk[3];
  }

  // Channel 4 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pk[3] == 0 && PINK & B00001000) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pk[3] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pk[4] = RC_Rec_Int_Timer_Pk[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pk[3] == 1 && !(PINK & B00001000)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pk[3] = 0;  // set to LOW
    RC_Rec_Int_Input_Pk[3] = RC_Rec_Int_Timer_Pk[0] - RC_Rec_Int_Timer_Pk[4];
  }

  // Channel 5 ----------------------------------
  if(RC_Rec_Int_Last_Channel_Pk[4] == 0 && PINK & B00010000) {
    // from LOW to HIGH
    RC_Rec_Int_Last_Channel_Pk[4] = 1;  // set to HIGH
    RC_Rec_Int_Timer_Pk[5] = RC_Rec_Int_Timer_Pk[0];
  }
  else if(RC_Rec_Int_Last_Channel_Pk[4] == 1 && !(PINK & B00010000)) {
    // from HIGH to LOW i.e. !(HIGH)
    RC_Rec_Int_Last_Channel_Pk[4] = 0;  // set to LOW
    RC_Rec_Int_Input_Pk[4] = RC_Rec_Int_Timer_Pk[0] - RC_Rec_Int_Timer_Pk[5];
  }
}
 
void RC_Rec_Int_Print_Inputs_Pb() {
  Serial.print("Port B");
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pb[0]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pb[1]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pb[2]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pb[3]);
  Serial.print(" - ");
  Serial.println(RC_Rec_Int_Input_Pb[4]);
}

void RC_Rec_Int_Print_Inputs_Pk() {
  Serial.print("Port K");
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pk[0]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pk[1]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pk[2]);
  Serial.print(" - ");
  Serial.print(RC_Rec_Int_Input_Pk[3]);
  Serial.print(" - ");
  Serial.println(RC_Rec_Int_Input_Pk[4]);
}


