#include "State.h"

#define WAVE_SPEED (56)

//              0      1        10        11      100     101     110        111     1000   1001
enum protoc {NONE, P_RED, P_ORANGE, P_YELLOW, P_GREEN, P_CYAN, P_BLUE, P_MAGENTA, CLAIMED, RESET};

Color colors[] = {OFF, RED, BLUE, YELLOW, GREEN, MAGENTA, ORANGE, CYAN};
byte playerColor;
byte waveToSend[6];

Timer timer;
Timer waveToSendTimer[6];
Timer sleepColor[6];

STATE_DEC(initS);
STATE_DEC(idleS);
STATE_DEC(shoreOpenS);
STATE_DEC(shoreClaimedS);
STATE_DEC(sendWaveS);
STATE_DEC(resetBroadcastS);
STATE_DEC(resetIgnoreS);

STATE_DEF(idleS, 
  { //entry
  },
  { //loop
    byte emptyEdgeCount = 0;
    FOREACH_FACE(f) {
      if(isValueReceivedOnFaceExpired(f)) {
        emptyEdgeCount ++;
      } else {
        byte color = getLastValueReceivedOnFace(f);
        switch(color) {
          case RESET:
            changeState(resetBroadcastS::state);
            break;
          case CLAIMED:
            emptyEdgeCount ++;
            break;
          case NONE:
            break;
          default:
            if (sleepColor[color].isExpired()) { // face is not zero and is not an ignored color
              FOREACH_FACE(g) {
                if (g != f) {
                  waveToSend[g] = color;
                  waveToSendTimer[g].set(WAVE_SPEED << 2);
                  timer.set(1000);
                }
              }
              sleepColor[color].set(1024); //ignore this color for a time
            }
            break; 
        }
      }
    }
    byte waveToShow = 0;
    FOREACH_FACE(f) { //display one of the waves
      if (waveToSend[f] != 0) { waveToShow = waveToSend[f]; }
    }
  
    if (waveToShow != NONE) {
      byte waveColor = waveToShow;
      timer.set(WAVE_SPEED << 2);
      setColor(colors[waveColor]);
    }
  
    FOREACH_FACE(f) {
      if (waveToSendTimer[f].isExpired()) { //either setting face or clearing face
        if (waveToSend[f] != NONE) { //something to send set face
          setValueSentOnFace(waveToSend[f], f);
          waveToSend[f] = NONE; //sent no longer need to send
          waveToSendTimer[f].set(100); //reset face to NONE after 100ms;
        } else {  //nothing to send clear
          setValueSentOnFace(NONE, f);
        }
      }
    }
  
    if(timer.isExpired()) { //idle animation, randomly show one of the player colors
      byte showColor = random(6);
      if (showColor == 0) {
        playerColor = random(4) + 1;
      } else {
        playerColor = NONE;
      }
      setColor(OFF);
      setColorOnFace(colors[playerColor], 0);
      timer.set(2000);
    }

    if (buttonSingleClicked() || buttonDoubleClicked() && playerColor != NONE) changeState(sendWaveS::state);
    
    if (buttonLongPressed()) changeState(resetBroadcastS::state);

    if (emptyEdgeCount > 2) {
      changeState(shoreOpenS::state); //i'm on the edge become open shore
    }
  }
)

STATE_DEF(shoreOpenS, 
  { //entry
    setColor(WHITE);
    setValueSentOnAllFaces(NONE);
    timer.set(1024);
  },
  { //loop
    byte emptyEdgeCount = 0;
    FOREACH_FACE(f) {
      if(isValueReceivedOnFaceExpired(f)) {
        setColorOnFace(WHITE, f);
        emptyEdgeCount ++;
      } else {
        byte color = getLastValueReceivedOnFace(f);
        switch(color) {
          case RESET:
            changeState(resetBroadcastS::state);
            break;
          case CLAIMED:
            emptyEdgeCount ++;
            break;
          case NONE:
            setColorOnFace(OFF, f);
            break;
          default:
            if (timer.isExpired()) { //stay open for a bit before becoming claimable
              playerColor = color;
              changeState(shoreClaimedS::state);
            }
        }
      }
    }

    if (emptyEdgeCount < 3) {
      changeState(idleS::state); //i'm not on the edge go back idle
    }

    if (buttonLongPressed()) changeState(resetBroadcastS::state);
  }
)

STATE_DEF(shoreClaimedS, 
  { //entry
    setColor(colors[playerColor]);
    setValueSentOnAllFaces(CLAIMED);
  },
  { //loop
    FOREACH_FACE(f) {
      if(!isValueReceivedOnFaceExpired(f)) {
        if (getLastValueReceivedOnFace(f) == RESET) changeState(resetBroadcastS::state);
      }
    }
    if (buttonLongPressed()) changeState(resetBroadcastS::state);
  }
)

STATE_DEF(sendWaveS, 
  { //entry
    timer.set(512);
    setValueSentOnAllFaces(playerColor);
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
    playerColor = NONE;
    FOREACH_FACE(f) { waveToSend[f] = NONE; }
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
