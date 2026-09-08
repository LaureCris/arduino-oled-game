#include <Wire.h>
#include <U8g2lib.h>

// OLED 128x64
U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled(U8G2_R0);

// Joystick
const int VRX = A1;
const int VRY = A0;
const int SW  = 2;

// =========================
// NAVE
// =========================

int jogadorX = 64;
int jogadorY = 32;

const int larguraNave = 8;
const int alturaNave = 7;

const unsigned char nave[] PROGMEM = {
  0b00011000,
  0b00111100,
  0b01111110,
  0b11111111,
  0b11111111,
  0b00111100,
  0b00011000
};


// =========================
// METEOROS
// =========================

const int MAX_METEOROS = 4;

int meteoroX[MAX_METEOROS];
int meteoroY[MAX_METEOROS];

const int tamanhoMeteoro = 7;


// =========================
// TIROS
// =========================

const int MAX_TIROS = 3;

bool tiroAtivo[MAX_TIROS];

int tiroX[MAX_TIROS];
int tiroY[MAX_TIROS];

const int tamanhoTiro = 3;
const int velocidadeTiro = 5;


// =========================
// EXPLOSÃO
// =========================

bool explosaoAtiva = false;

int explosaoX;
int explosaoY;

int tamanhoExplosao = 1;


// =========================
// JOGO
// =========================

bool gameOver = false;

int score = 0;

int quantidadeMeteoros = 2;

int velocidadeInimigo = 1;


// =========================
// BOTÃO
// =========================

bool botaoAnterior = HIGH;


// =========================
// SETUP
// =========================

void setup() {

  pinMode(SW, INPUT_PULLUP);

  oled.begin();

  randomSeed(analogRead(A5));


  // Inicializa meteoros

  for (int i = 0; i < MAX_METEOROS; i++) {

    meteoroX[i] = 128 + (i * 55);

    meteoroY[i] = random(12, 58);
  }


  // Inicializa tiros

  for (int i = 0; i < MAX_TIROS; i++) {

    tiroAtivo[i] = false;
  }
}


// =========================
// LOOP
// =========================

void loop() {

  // =================================
  // GAME OVER
  // =================================

  if (gameOver) {

    oled.firstPage();

    do {

      oled.setFont(u8g2_font_6x10_tf);

      oled.drawStr(32, 25, "GAME OVER");

      oled.setCursor(38, 40);
      oled.print("SCORE: ");
      oled.print(score);

      oled.drawStr(13, 58, "APERTE O JOYSTICK");

    } while (oled.nextPage());


    if (digitalRead(SW) == LOW) {

      gameOver = false;

      jogadorX = 64;
      jogadorY = 32;

      score = 0;

      quantidadeMeteoros = 2;

      velocidadeInimigo = 1;

      explosaoAtiva = false;

      botaoAnterior = HIGH;


      // Reinicia meteoros

      for (int i = 0; i < MAX_METEOROS; i++) {

        meteoroX[i] = 128 + (i * 55);

        meteoroY[i] = random(12, 58);
      }


      // Remove tiros

      for (int i = 0; i < MAX_TIROS; i++) {

        tiroAtivo[i] = false;
      }

      delay(300);
    }

    return;
  }


  // =========================
  // 1. LER JOYSTICK
  // =========================

  int joystickX = analogRead(VRX);
  int joystickY = analogRead(VRY);

  jogadorX = map(
    joystickX,
    0,
    1023,
    0,
    120
  );

  jogadorY = map(
    joystickY,
    0,
    1023,
    57,
    0
  );


  // =========================
// 2. DEFINIR DIFICULDADE
// =========================

if (score < 2) {

  quantidadeMeteoros = 2;
  velocidadeInimigo = 2;

} else if (score < 5) {

  quantidadeMeteoros = 2;
  velocidadeInimigo = 3;

} else if (score < 8) {

  quantidadeMeteoros = 3;
  velocidadeInimigo = 3;

} else if (score < 12) {

  quantidadeMeteoros = 3;
  velocidadeInimigo = 4;

} else {

  quantidadeMeteoros = 4;
  velocidadeInimigo = 5;
}
  // =========================
  // 3. DISPARAR
  // =========================

  bool botaoAtual = digitalRead(SW);


  // Detecta o momento do clique

  if (botaoAnterior == HIGH && botaoAtual == LOW) {

    // Procura um espaço livre para o tiro

    for (int i = 0; i < MAX_TIROS; i++) {

      if (!tiroAtivo[i]) {

        tiroAtivo[i] = true;

        tiroX[i] = jogadorX + larguraNave;

        tiroY[i] = jogadorY + 3;

        break;
      }
    }
  }

  botaoAnterior = botaoAtual;


  // =========================
  // 4. MOVER TIROS
  // =========================

  for (int i = 0; i < MAX_TIROS; i++) {

    if (tiroAtivo[i]) {

      tiroX[i] += velocidadeTiro;


      if (tiroX[i] > 128) {

        tiroAtivo[i] = false;
      }
    }
  }


  // =========================
  // 5. MOVER METEOROS
  // =========================

  for (int i = 0; i < quantidadeMeteoros; i++) {

    meteoroX[i] -= velocidadeInimigo;


    if (meteoroX[i] < -tamanhoMeteoro) {

      meteoroX[i] = 128 + random(0, 50);

      meteoroY[i] = random(12, 58);
    }
  }


  // =========================
  // 6. TIRO × METEORO
  // =========================

  for (int t = 0; t < MAX_TIROS; t++) {

    if (!tiroAtivo[t]) {
      continue;
    }


    for (int m = 0; m < quantidadeMeteoros; m++) {

      if (
        tiroX[t] < meteoroX[m] + tamanhoMeteoro &&
        tiroX[t] + tamanhoTiro > meteoroX[m] &&
        tiroY[t] < meteoroY[m] + tamanhoMeteoro &&
        tiroY[t] + tamanhoTiro > meteoroY[m]
      ) {

        // Guarda posição da explosão

        explosaoX = meteoroX[m] + 3;
        explosaoY = meteoroY[m] + 3;

        explosaoAtiva = true;

        tamanhoExplosao = 1;


        // Desativa tiro

        tiroAtivo[t] = false;


        // Ganha ponto

        score++;


        // Novo meteoro

        meteoroX[m] = 128 + random(20, 70);

        meteoroY[m] = random(12, 58);


        break;
      }
    }
  }


  // =========================
  // 7. ANIMAÇÃO DA EXPLOSÃO
  // =========================

  if (explosaoAtiva) {

    tamanhoExplosao++;


    if (tamanhoExplosao > 7) {

      explosaoAtiva = false;
    }
  }


  // =========================
  // 8. NAVE × METEORO
  // =========================

  for (int i = 0; i < quantidadeMeteoros; i++) {

    if (
      jogadorX < meteoroX[i] + tamanhoMeteoro &&
      jogadorX + larguraNave > meteoroX[i] &&
      jogadorY < meteoroY[i] + tamanhoMeteoro &&
      jogadorY + alturaNave > meteoroY[i]
    ) {

      gameOver = true;
    }
  }


  // =========================
  // 9. DESENHAR
  // =========================

  oled.firstPage();

  do {

    oled.setFont(u8g2_font_6x10_tf);


    // Nome

    oled.drawStr(0, 9, "SPACE RUN");


    // Score

    oled.setCursor(85, 9);
    oled.print("S:");
    oled.print(score);


    // =========================
    // NAVE
    // =========================

    oled.drawXBMP(
      jogadorX,
      jogadorY,
      larguraNave,
      alturaNave,
      nave
    );


    // =========================
    // TIROS
    // =========================

    for (int i = 0; i < MAX_TIROS; i++) {

      if (tiroAtivo[i]) {

        oled.drawBox(
          tiroX[i],
          tiroY[i],
          tamanhoTiro,
          tamanhoTiro
        );
      }
    }


    // =========================
    // METEOROS
    // =========================

    for (int i = 0; i < quantidadeMeteoros; i++) {

      oled.drawDisc(
        meteoroX[i] + 3,
        meteoroY[i] + 3,
        3
      );
    }


    // =========================
    // EXPLOSÃO
    // =========================

    if (explosaoAtiva) {

      oled.drawCircle(
        explosaoX,
        explosaoY,
        tamanhoExplosao
      );

      oled.drawLine(
        explosaoX - tamanhoExplosao,
        explosaoY,
        explosaoX + tamanhoExplosao,
        explosaoY
      );

      oled.drawLine(
        explosaoX,
        explosaoY - tamanhoExplosao,
        explosaoX,
        explosaoY + tamanhoExplosao
      );
    }

  } while (oled.nextPage());


  delay(30);
}