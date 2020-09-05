#include "State.h"

#define NONE 0
#define RESET 56

Timer animationTimer;
Timer clearTimer;

Color colors[] = {WHITE, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
byte playerColor;
byte waveSpeed;
Timer timer;
byte lives;
bool sending;
byte waveToSend[6] = {0, 0, 0, 0, 0, 0};

Timer waveToSendTimer[6];
Timer sleepColor[6];

STATE_DEC(initS);
STATE_DEC(idleS);
STATE_DEC(sendWaveS);
STATE_DEC(resetBroadcastS);
STATE_DEC(resetIgnoreS);

STATE_DEF(idleS, 
  { //entry
  },
  { //loop
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {
        byte waveData = getLastValueReceivedOnFace(f);
        if (waveData == RESET) changeState(resetBroadcastS::state);
        byte color = waveData & 7;
        if (waveData != 0 && sleepColor[color].isExpired()) { // face is not zero and is not an ignored color
          FOREACH_FACE(g) {
            if (g != f) {
              waveToSend[g] = waveData;
              int waveSpeed = waveData & 56;//8 + 16 + 32
              waveToSendTimer[g].set(waveSpeed << 2);
              animationTimer.set(1000);
            }
          }
        }
        sleepColor[color].set(1024); //ignore this color for a time
      }
    }
    byte waveToShow = 0;
    FOREACH_FACE(f) { //display one of the waves
      if (waveToSend[f] != 0) { waveToShow = waveToSend[f]; }
    }
  
    if (waveToShow != 0) {
      byte waveColor = waveToShow & 7;
      int waveSpeed = waveToShow & 56;//8 + 16 + 32
      animationTimer.set(waveSpeed << 2);
      setColor(colors[waveColor]);
    }
  
    FOREACH_FACE(f) {
      if (waveToSendTimer[f].isExpired()) { //either setting face or clearing face
        if (waveToSend[f] != NONE) { //something to send set face
          setValueSentOnFace(waveToSend[f], f);
          waveToSend[f] = NONE; //sent no longer need to send
          waveToSendTimer[f].set(100); //reset face to zero after 100ms;
        } else {  //nothing to send clear
          setValueSentOnFace(NONE, f);
        }
      }
    }
  
    if(animationTimer.isExpired()) { //idle animation
      setColor(OFF);
      byte offset = random(5);
      for(uint8_t x = 0; x < lives ; ++ x) {
        setColorOnFace(colors[playerColor], (x + offset) % 6);
      }
      animationTimer.set(1000);
    }

    if (buttonSingleClicked() || buttonDoubleClicked()) changeState(sendWaveS::state);
    
    if (buttonLongPressed()) changeState(resetBroadcastS::state);
  }
)

STATE_DEF(sendWaveS, 
  { //entry
    timer.set(256);
    setValueSentOnAllFaces(waveSpeed + playerColor);
    setColor(colors[playerColor]);
  },
  { //loop
    if(timer.isExpired()) {
      setValueSentOnAllFaces(NONE);
      changeState(idleS::state);
    }
  }
)

STATE_DEF(resetBroadcastS, 
  { //entry
    timer.set(500);
    setValueSentOnAllFaces(RESET);
    setColor(RED);
  },
  { //loop
    if(timer.isExpired()) {
      changeState(resetIgnoreS::state);
    }
  }
)

STATE_DEF(resetIgnoreS,
  { //entry
    timer.set(500);
    setValueSentOnAllFaces(NONE); //stop broadcasting gameOver wave
    setColor(BLUE);
  },
  { //loop
    if(timer.isExpired()) { //stop ignoring
      changeState(initS::state);
    }
  }
)

STATE_DEF(initS,
  { //entry
    setValueSentOnAllFaces(NONE);
    randomize();
    playerColor = random(7)+1;
    lives = 1;
    waveSpeed = 56;
    sending = false;
    setColor(GREEN);
  },
  { //loop
    changeState(idleS::state);
  }
)

void setup() {
  changeState(initS::state);
}

void loop() {
  stateFn();
}
