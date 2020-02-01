#include "functionheader.h"
void Ocean();
void PlayerSelect();
void Player();

State state;
Timer oceanAmbient;
Timer waveTimer;

Color colors[] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE};
byte playerColor;
byte waveData;
byte waveSpeed;
byte waveColor;
byte lives;

void Ocean() {
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {
      waveData = getLastValueReceivedOnFace(f);
      waveColor = waveData & 7;
      waveSpeed = waveData & 56;//8 + 16 + 32
      waveTimer.set(waveSpeed >> 2);
    }
  }
  setValueSentOnAllFaces(waveData);

  if (!waveTimer.isExpired()) {
    setColor(colors[waveColor]);
  } else if(oceanAmbient.isExpired()) {
    setColor(OFF);
    setColorOnFace(makeColorRGB(0, 0, 32), random(5));
    oceanAmbient.set(1000);
  }
  if (buttonLongPressed()) {
    state = PlayerSelect;
  }
}

void PlayerSelect() {
  setColor(RED);
  if (buttonLongPressed()) {
    state = Player;
  }
}

void Player() {
  setColor(GREEN);
  if (buttonLongPressed()) {
    state = Ocean;
  }
  if (buttonDoubleClicked()) {
    //send wave
    setValueSentOnAllFaces(waveSpeed + playerColor);
  }
}

void setup() {
  state = Ocean;
  playerColor = 0;
  lives = 6;
  waveSpeed = 56;
}

void loop() {
  state();

}
