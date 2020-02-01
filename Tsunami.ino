#include "functionheader.h"
void Ocean();
void PlayerSelect();
void Player();

State state;
Timer oceanAmbientOrClear;
Timer playerSelectedAnim;

Color colors[] = {WHITE, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
byte playerColor;
byte waveSpeed;
byte lives;
bool sending;
byte waveToSend[] = {0, 0, 0, 0, 0, 0};

Timer waveToSendTimer[6];
Timer sleepFace[6];

void Ocean() {

  FOREACH_FACE(f) {
    if (sleepFace[f].isExpired() && !isValueReceivedOnFaceExpired(f)) {
      byte waveData = getLastValueReceivedOnFace(f);
      if (waveData != 0) {
        FOREACH_FACE(g) {
          if (g != f) {
            waveToSend[g] = waveData;
            int waveSpeed = waveData & 56;//8 + 16 + 32
            waveToSendTimer[g].set(waveSpeed << 2);
            oceanAmbientOrClear.set(1000);
          }
        }
        sleepFace[f].set(100); //sleep receive on face for 100ms
      }
    }
  }
  byte waveToShow = 0;
  FOREACH_FACE(f) { //display one of the waves
    if (waveToSend[f] != 0) { waveToShow = waveToSend[f]; }
  }

  if (waveToShow != 0) {
    byte waveColor = waveToShow & 7;
    int waveSpeed = waveToShow & 56;//8 + 16 + 32
    oceanAmbientOrClear.set(waveSpeed << 2);
    setColor(colors[waveColor]);
  }

  FOREACH_FACE(f) {
    if (waveToSendTimer[f].isExpired()) { //either setting face or clearing face
      if (waveToSend[f] != 0) { //something to send set face
        setValueSentOnFace(waveToSend[f], f);
        waveToSend[f] = 0; //sent no longer need to send
        waveToSendTimer[f].set(100); //reset face to zero after 100ms;
      } else {  //nothing to send clear
        setValueSentOnFace(0, f);
      }
    }
  }

  if(oceanAmbientOrClear.isExpired()) { //idle ocean animation
    setColor(OFF);
    setColorOnFace(makeColorRGB(0, 0, 32), random(5));
    oceanAmbientOrClear.set(1000);
  }

  if (buttonLongPressed()) {
    state = PlayerSelect;
  }
}

void PlayerSelect() {
  setColor(colors[playerColor]);
  if (buttonSingleClicked()) {
    //advance color
    playerColor = (playerColor + 1) % 7 + 1; //unintentional interleave but i am keeping it, i was planning to interleave anyway
  }
  if (buttonLongPressed()) {
    state = Player;
    playerSelectedAnim.set(1000);
  }
}

void Player() {
  if (!playerSelectedAnim.isExpired()) {
    setColor(dim(colors[playerColor], sin8_C(playerSelectedAnim.getRemaining() % 255)));
  } else {
    setColor(colors[playerColor]);
  }
  if (buttonLongPressed()) {
    state = Ocean;
  }
  if (buttonSingleClicked()) {
    //catch wave
  }
  if (buttonDoubleClicked()) {
    //send wave
    setValueSentOnAllFaces(waveSpeed + playerColor);
    sending = true;
    oceanAmbientOrClear.set(256);
  }
  if (sending && oceanAmbientOrClear.isExpired()) {
    setValueSentOnAllFaces(0);
    sending = false;
  }
  //process incoming wave
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f) && didValueOnFaceChange(f)) {
      //set wave present timer
      // if wave not caught decrement life
      // if catch attempted but no wave present decrement life
//      waveData = getLastValueReceivedOnFace(f);
//      if (waveData != 0) {
//        waveColor = waveData & 7;
//        waveSpeed = waveData & 56;//8 + 16 + 32
//        waveTimer.set(waveSpeed >> 2);
//      }
    }
  }
}

void setup() {
  state = Ocean;
  playerColor = 1;
  lives = 6;
  waveSpeed = 56;
  sending = false;
}

void loop() {
  state();

}
