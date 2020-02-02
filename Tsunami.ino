#include "functionheader.h"
void Ocean();
void ColorSelect();
void ColorSelected();
void Player();
void PlayerHasWave();

State state;
Timer animationTimer;
Timer clearTimer;

Color colors[] = {WHITE, RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
byte playerColor;
byte waveSpeed;
byte lives;
bool sending;
byte waveToSend[6] = {0, 0, 0, 0, 0, 0};

Timer waveToSendTimer[6];
Timer sleepColor[6];

void Ocean() {

  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      byte waveData = getLastValueReceivedOnFace(f);
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
      if (waveToSend[f] != 0) { //something to send set face
        setValueSentOnFace(waveToSend[f], f);
        waveToSend[f] = 0; //sent no longer need to send
        waveToSendTimer[f].set(100); //reset face to zero after 100ms;
      } else {  //nothing to send clear
        setValueSentOnFace(0, f);
      }
    }
  }

  if(animationTimer.isExpired()) { //idle ocean animation
    setColor(OFF);
    setColorOnFace(makeColorRGB(0, 0, 32), random(5));
    animationTimer.set(1000);
  }

  if (buttonLongPressed()) {
    state = ColorSelect;
  }
}

void ColorSelect() {
  setColor(colors[playerColor]);
  if (buttonSingleClicked()) {
    //advance color
    playerColor = (playerColor + 1) % 7 + 1; //unintentional interleave but i am keeping it, i was planning to interleave anyway
  }
  if (buttonLongPressed()) {
    state = ColorSelected;
    animationTimer.set(1000);
  }
}

void ColorSelected() {
  if (!animationTimer.isExpired()) {
    setColor(dim(colors[playerColor], sin8_C(animationTimer.getRemaining() % 255)));
  } else {
  setColor(colors[playerColor]);
    state = Player;
  }
}

void Player() {
  if (buttonLongPressed()) {
    state = Ocean;
  }
  if (buttonSingleClicked()) { //clicked to early, lose life
    lives -= 1;
  }
  if (buttonDoubleClicked()) {
    //send wave
    setValueSentOnAllFaces(waveSpeed + playerColor);
    sending = true;
    clearTimer.set(256);
  }
  if (sending && clearTimer.isExpired()) {
    setValueSentOnAllFaces(0);
    sending = false;
  }
  
  //process incoming wave
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      byte waveData = getLastValueReceivedOnFace(f);
      byte color = waveData & 7;
      if (waveData != 0 && sleepColor[color].isExpired()) { // face is not zero and is not an ignored color
        // wave arrived, player must deal with it
        state = PlayerHasWave;
        int waveSpeed = waveData & 56;//8 + 16 + 32
        clearTimer.set(waveSpeed << 3); // twice the propagation speed
      }
      sleepColor[color].set(1024); //ignore this color for a time
    }
  }
  
  FOREACH_FACE(f) { //animate
    if ((f + (millis() >> 8) % 5) < lives) { // TODO phase shift on millis
      setColorOnFace(colors[playerColor], f);
    } else {
      setColorOnFace(OFF, f);
    }
  }
}

void PlayerHasWave() {
  setColor(WHITE);//TODO animate
  if (clearTimer.isExpired()) { //didn't catch wave, lose life
    lives -= 1;
    state = Player;
  }
  if (buttonSingleClicked()) { //caught wave, increase speed
    state = Player;
    waveSpeed = max(waveSpeed - 8, 8);
  }
}

void setup() {
  state = Ocean;
  playerColor = 1;
  lives = 5;
  waveSpeed = 56;
  sending = false;
}

void loop() {
  state();

}
