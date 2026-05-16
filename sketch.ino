#include <GameBoy.h>
GameBoy gb;

int score=0;

// ===== SNAKE  begin =====

int snakeHeadX,snakeHeadY;      // позиція голови
int snakeFoodX,snakeFoodY;      // позиція їжі
int snakeDirX,snakeDirY;        // напрям руху
int snakeLength;                // довжина та хвіст
int snakeTailX[128],snakeTailY[128];

void setupSnake(){
  snakeLength=0;                 // обнуляємо хвіст
  snakeFoodX=3; snakeFoodY=3;    // стартова їжа
  snakeHeadX=5; snakeHeadY=5;    // старт голови
  snakeDirX=0; snakeDirY=-1;     // рух вгору
  score=0;
}

bool loopSnake(){
  // зсув хвоста
  for(int i=snakeLength;i>0;i--)
    snakeTailX[i]=snakeTailX[i-1],
    snakeTailY[i]=snakeTailY[i-1];

  if(snakeLength>0)
    snakeTailX[0]=snakeHeadX,
    snakeTailY[0]=snakeHeadY;

  // рух голови
  snakeHeadX+=snakeDirX;
  snakeHeadY+=snakeDirY;

  // телепорт через край
  if(snakeHeadX<0)snakeHeadX=7;
  if(snakeHeadX>7)snakeHeadX=0;
  if(snakeHeadY<0)snakeHeadY=15;
  if(snakeHeadY>15)snakeHeadY=0;

  // перевірка програшу
  for(int i=1;i<snakeLength;i++)
    if(snakeHeadX==snakeTailX[i] && snakeHeadY==snakeTailY[i]){
      score=snakeLength;         // записуємо рахунок
      gb.sound(1);
      delay(1000);
      return false;              // вихід з гри
    }

  // перевірка їжі
  if(snakeHeadX==snakeFoodX && snakeHeadY==snakeFoodY){
    snakeLength++;
    snakeFoodX=random(8);
    snakeFoodY=random(16);
    gb.sound(1);
  }

  // малювання
  gb.clearDisplay();

  for(int i=0;i<snakeLength;i++)
    gb.drawPoint(snakeTailX[i],snakeTailY[i]);

  gb.drawPoint(snakeFoodX,snakeFoodY);
  gb.drawPoint(snakeHeadX,snakeHeadY);

  // керування
  int key=gb.getKey();
  if(key==RIGHT && snakeDirX!=-1) snakeDirX=1,  snakeDirY=0;   // вправо
  if(key==LEFT  && snakeDirX!= 1) snakeDirX=-1, snakeDirY=0;   // вліво
  if(key==DOWN  && snakeDirY!=-1) snakeDirY=1,  snakeDirX=0;   // вниз
  if(key==UP    && snakeDirY!= 1) snakeDirY=-1, snakeDirX=0;   // вгору

  delay(120);                     // швидкість гри
  return true;
}


// ===== SNAKE  end =====

void setup(){ gb.begin(0); setupSnake(); }
void loop(){ if(!loopSnake()) setupSnake(); }