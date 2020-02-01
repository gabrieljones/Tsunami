#include "functionheader.h"
void Ocean();
void PlayerSelect();
void Player();

State state;
Timer oceanAmbientOrClear;
Timer waveTimer;
Timer playerSelectedAnim;

Color colors[] = {WHITE, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
byte playerColor;
byte waveData;
byte waveSpeed;
byte lives;
bool sending;
byte waveToSend;

void Ocean() {
  waveData = 0;

  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f) && waveData == 0) { //get the first not 0 face
      waveData = getLastValueReceivedOnFace(f);
    }
  }
  
  if (waveData != 0) {
    byte waveColor = waveData & 7;
    waveSpeed = waveData & 56;//8 + 16 + 32
    waveTimer.set(waveSpeed << 2);
    setColor(colors[waveColor]);
    waveToSend = waveData;
  }

  if (waveToSend != 0 && waveTimer.isExpired()) {
      setValueSentOnAllFaces(waveData);
      waveToSend = 0;
  }

  if(waveTimer.isExpired() && oceanAmbientOrClear.isExpired()) { //idle ocean animation
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
    oceanAmbientOrClear.set(100);
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
