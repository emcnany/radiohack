
#define E_07 0
#define X_148 1
#define D_259 2
#define MSP_36 3
#define PERIOD_X 4
#define POWER_X 5

#define SIG_NUM 6
#define SIG_LENGTH 30
#define HISTORY_LENGTH 2

int signals[SIG_NUM][SIG_LENGTH];
int highCount[SIG_NUM];
int pointer = 0;
char prepKey[] = {'n', 'n', 'n'};
char currentKey = 'n';
int currentInt = -1;
int preInt = -1;
int postInt = -1;
char calOperator = 'n';

void setup() {
  resetSignals();
  
  Serial.begin(115200);
}


void loop() {
  dataUpdate();
  
  currentKey = 'n';
  currentInt = -1;
  if(pointer == SIG_LENGTH - 1){
    for(int i=0; i<SIG_NUM; i++){
      showButtonEvent(i);
    }
    
    matchButtons(currentKey);
  }
  
  
  pointer++;
  if(pointer >= SIG_LENGTH) {
    pointer = 0;
    resetSignals();
  }
  delay(1);
}

void resetSignals(){
  for(int i=0; i<SIG_NUM; i++){
    for(int j=0; j<SIG_LENGTH; j++){
      signals[i][j] = -1;
    }
  }
}


void showButtonEvent(int index){
  highCount[index] = 0;
  for(int j=0; j<SIG_LENGTH; j++){
    if(signals[index][j] > 256) highCount[index]++;      
  }
  
  float ratio = (float)highCount[index] / (float)SIG_LENGTH;
  //Serial.print(ratio);
  
  if (index == E_07){
    if (ratio > 0.15 && ratio < 0.35) {
      currentKey = '7';
      currentInt = 7;
    }
    else if (ratio > 0.4 && ratio < 0.6) {
      currentKey = '=';
      currentInt = -1;
    }
    //else if (ratio > 0.4 && ratio < 0.6) currentKey = '0';
  } 
  
  else if ( index == X_148){
    if (ratio > 0.15 && ratio < 0.35) {
      currentKey = '8';
      currentInt = 8;
    }
    else if (ratio > 0.4 && ratio < 0.6) {
      currentKey = '1';
      currentInt = 1;
    }
    else if (ratio > 0.65 && ratio < 0.85) {
      currentKey = '4';
      currentInt = 4;
    }
  } 
  
  else if ( index == D_259){
    if (ratio > 0.15 && ratio < 0.35) {
      currentKey = '9';
      currentInt = 9;
    }
    else if (ratio > 0.4 && ratio < 0.6) {
      currentKey = '2'; // divide
      currentInt = 2;
    }
    else if (ratio > 0.65 && ratio < 0.85) {
      currentKey = '5';
      currentInt = 5;
    }
  } 
  
  else if ( index == MSP_36){
    float powerFreq = (float)highCount[POWER_X] / (float)SIG_LENGTH;
    if (ratio > 0.15 && ratio < 0.35) {
      currentKey = '-';
      currentInt = -1;
    }
    else if (ratio > 0.4 && ratio < 0.6) {
      currentKey = '3'; // x
      currentInt = 3;
    }
    else if (ratio > 0.65 && ratio < 0.85) {
      currentKey = '6'; // +
      currentInt = 6;
    }
  } 
  
  else if ( index == PERIOD_X){
   // if (ratio > 0.5) currentKey = '.';
  } 
}

boolean printButton = true;
void matchButtons(char currentKey){
  boolean match = true;
  for(int i=0; i < HISTORY_LENGTH; i++){
    //Serial.print(prepKey[i]);
    
    if (currentKey != prepKey[i]){
      match = false;
    }
  }
  //Serial.print('-');
  //Serial.println(currentKey);
  
  if (match){
    if(currentKey != 'n'){
      if (printButton) {
        //Serial.println(currentInt);
        if (preInt == -1){
          if (currentInt != -1) preInt = currentInt;
        }else if (calOperator == 'n') {
          if (currentInt == 6) calOperator = '+';
          else if (currentInt == 3) calOperator = 'x';
          else if (currentInt == 2) calOperator = 'd';
          else if (currentKey == '-') calOperator = '-';
        }else if (postInt == -1){
          if (currentInt != -1) postInt = currentInt;
        }else{
          if(currentKey == '='){
            int signal = -1;
            if (calOperator == '+') signal = preInt + postInt;
            else if (calOperator == '-') signal = preInt - postInt;
            else if (calOperator == 'x') signal = preInt * postInt;
            else if (calOperator == 'd') signal = (float)preInt / (float)postInt;
            
            //send signal
            Serial.println(signal);
            preInt = -1;
            postInt = -1;
            calOperator = 'n';
          }
        }
        
        Serial.print(preInt);
        Serial.print(',');
        Serial.print(calOperator);
        Serial.print(',');
        Serial.println(postInt);
        
      }
      
      for(int i = 0; i<HISTORY_LENGTH; i++){
        prepKey[i] = 'n';    
      }
      printButton = false;
    }else{
      printButton = true;
    }
  }else {
    for(int i = 0; i<HISTORY_LENGTH-1; i++){
      prepKey[i] = prepKey[i+1];    
    }
    prepKey[HISTORY_LENGTH-1] = currentKey;
  }
}

void dataUpdate(){
  for(int i=0; i<SIG_NUM; i++){
    signals[i][pointer] = analogRead(i);
    highCount[i] = 0;
  }
}
