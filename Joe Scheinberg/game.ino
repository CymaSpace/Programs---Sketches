#include <FastLED.h>
#include "EEPROM.h"

/* Output pin definitions */
#define NUM_LEDS_ARRAY 76 // Number of LED's in the strip
#define DATA_PIN 6 // Data out
#define ANALOG_PIN_L 1 // Left audio channel
#define ANALOG_PIN_R 0 // Right audio channel
#define REFRESH_POT_PIN 2 // Left pot
#define SENSITIVITY_POT_PIN 3 // Right pot
#define STOMP_PIN 5 // The pin connected to the stomp button
#define STROBE_PIN 12 // Strobe pin 
#define RESET_PIN 13 // Reset Pin

// Set color value to full saturation and value. Set the hue to 0
CRGB gLeds[NUM_LEDS_ARRAY]; // Represents LED strip

#define MAX_LEDS 256
#define MAX_BULLETS 6
#define MAX_ENEMIES 4
#define BULLET_SPEED 15.0f

#define ARRAYSIZE(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))

void SetState( int state );

enum GameState
{
  GameState_AttractMode,
  GameState_Playing,
  GameState_GameOver
};

CRGB PlayerColors[] =
{
  CRGB(255,255,0),
  CRGB(0,0,255)
};


enum BulletState
{
  BulletState_InFlight,
  BulletState_Explode
};

struct Bullet
{
  float Pos;
  float Velocity;
  BulletState State;
  float TimeInState;
  int FramesInState;
};

enum EnemyState
{
  EnemyState_Spawning,
  EnemyState_Active
};

struct Enemy
{
  float Pos;
  EnemyState State;
  float TimeInState;
  int FrameInState;
};

struct GameGlob
{
  GameState State;
  float TimeInState;
  int FramesInState;
  
  int PlayerPos;
  float PlayerPosFloat;
  int PlayerColorIndex;
  bool StompPressed;
  float SecondsInGame;
  int WorldBoundsStartLED;
  int WorldBoundsEndLED;
  float WallTimer;

  Bullet Bullets[MAX_BULLETS];
  int BulletCount;

  Enemy Enemies[MAX_ENEMIES];
  int EnemyCount;
  
};

GameGlob gGameGlob;
int gLEDCount;

void ResetGame() 
{
  gGameGlob.State = GameState_AttractMode;
  gGameGlob.TimeInState = 0.0f;
  gGameGlob.FramesInState = 0;
  gGameGlob.PlayerPos = gLEDCount / 2;
  gGameGlob.PlayerPosFloat = gLEDCount / 2;
  gGameGlob.StompPressed = false;
  gGameGlob.PlayerColorIndex = 0;
  gGameGlob.SecondsInGame = 0.0f;
  gGameGlob.WorldBoundsStartLED = 1;
  gGameGlob.WorldBoundsEndLED = gLEDCount - 1;
  gGameGlob.WallTimer = 0.0f;
  gGameGlob.BulletCount = 0;
  gGameGlob.EnemyCount = 0;
}

void setup() {

  Serial.begin(9600);
  Serial.print("Hello World\n");
  gLEDCount = 18 * 3;
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(gLeds, gLEDCount + 1);

  // Clear any old values on EEPROM
  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM

  // Set pin modes
  pinMode(ANALOG_PIN_L, INPUT);
  pinMode(ANALOG_PIN_R, INPUT);
  pinMode(STOMP_PIN, INPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);

  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH); 

  ResetGame();
}

void SetGameboardLED(int index, CRGB rgb)
{
  gLeds[index+1] = rgb;
}

void UpdateWalls(float dt)
{
  float wallsPerSecond = 0.65f;
  
  gGameGlob.WallTimer += dt;
  if( gGameGlob.WallTimer > ( 1 / wallsPerSecond))
  {
    gGameGlob.WorldBoundsStartLED++;
    gGameGlob.WorldBoundsEndLED--;
    
    gGameGlob.WallTimer = 0.0f;
  } 
}

// *******************************************************************************************
// Bullet Code

void RemoveBullet( int index )
{
  if( gGameGlob.BulletCount > 1 )
  {
    gGameGlob.Bullets[index] = gGameGlob.Bullets[gGameGlob.BulletCount-1];
  }
  gGameGlob.BulletCount--;
}

void FireBullet(int position, float velocity)
{
  if( gGameGlob.BulletCount >= MAX_BULLETS )
    return;

  Bullet *b = &gGameGlob.Bullets[gGameGlob.BulletCount];
  b->TimeInState = 0.0f;
  b->FramesInState = 0;
  b->Pos = position;
  b->Velocity = velocity;
  b->State = BulletState_InFlight;
  gGameGlob.BulletCount++;
}

void UpdateBullets(float dt)
{
  for( int i = 0 ; i < gGameGlob.BulletCount; i++ )
  {
    Bullet *b = &gGameGlob.Bullets[i];
    if( b->State == BulletState_InFlight )
    {
      b->Pos += b->Velocity * dt;
      int intPos = (int)b->Pos;
      if( intPos <= gGameGlob.WorldBoundsStartLED || intPos >= gGameGlob.WorldBoundsEndLED )
      {
        b->State = BulletState_Explode;
        b->TimeInState = 0.0f;
        b->FramesInState = 0;
        continue;
      }
      for( int j = 0 ; j < gGameGlob.EnemyCount; j++ )
      {
        if( abs( b->Pos - gGameGlob.Enemies[j].Pos ) < 1.0f )
        {
          RemoveEnemy( j );
          b->State = BulletState_Explode;
          b->TimeInState = 0.0f;
          b->FramesInState = 0;
          break;
        }
      }
    }
    if ( b->State == BulletState_Explode )
    {
      if( b->TimeInState > 1 )
      {
        RemoveBullet(i);
        i--;
      }
    }
    b->TimeInState += dt;
    b->FramesInState++;
  }
}

void RenderBullets()
{
  for( int i = 0 ; i < gGameGlob.BulletCount; i++ )
  {
    Bullet *b = &gGameGlob.Bullets[i];
    
    if( b->State == BulletState_InFlight )
    {
      SetGameboardLED(b->Pos, CRGB(128, 128, 128));
    }
    if( b->State == BulletState_Explode )
    {
      
    }
  }
}

// *******************************************************************************************
// Enemy Code

void RemoveEnemy( int index )
{
  if( gGameGlob.EnemyCount > 1 )
  {
    gGameGlob.Enemies[index] = gGameGlob.Enemies[gGameGlob.EnemyCount-1];
  }
  gGameGlob.EnemyCount--;
}

void SpawnEnemy(int position)
{
  if( gGameGlob.EnemyCount >= MAX_ENEMIES )
    return;

  Enemy *e = &gGameGlob.Enemies[gGameGlob.EnemyCount];
  e->TimeInState = 0.0f;
  e->FrameInState = 0;
  e->Pos = position;
  e->State = EnemyState_Spawning;
  gGameGlob.EnemyCount++;
}

void SpawnRandomEnemy()
{
  int availableSpaces = gGameGlob.WorldBoundsEndLED - gGameGlob.WorldBoundsStartLED;
  int num = random(availableSpaces);
  SpawnEnemy( gGameGlob.WorldBoundsStartLED + num);
}

void UpdateEnemies(float dt)
{
  for( int i = 0 ; i < gGameGlob.EnemyCount; i++ )
  {
    Enemy *e = &gGameGlob.Enemies[i];
    if( e->State == EnemyState_Spawning )
    {
      if( e->TimeInState > 1.0f )
      {
        e->State = EnemyState_Active;
        e->TimeInState = 0.0f;
        e->FrameInState = 0;
      }
    }
    else if ( e->State == EnemyState_Active )
    {
      float enemySpeed = 3.0f;
      // move the enemy towards the player
      float dir = e->Pos > gGameGlob.PlayerPosFloat ? -1.0f : 1.0f;
      e->Pos += dir * dt * enemySpeed;
      
      if( abs( e->Pos - gGameGlob.PlayerPosFloat ) < 0.5f )
      {
        SetState( GameState_GameOver );
      }
    }
    e->TimeInState += dt;
    e->FrameInState++;
  }
}

void RenderEnemies()
{
  for( int i = 0 ; i < gGameGlob.EnemyCount; i++ )
  {
    Enemy *e = &gGameGlob.Enemies[i];
    
    if( e->State == EnemyState_Spawning )
    {
      if( ((e->FrameInState / 4) % 1) == 1)
        SetGameboardLED(e->Pos, CRGB(128, 0, 128));
    }
    if( e->State == EnemyState_Active )
    {
      SetGameboardLED(e->Pos, CRGB(0, 170, 0));
    }
  }
}

void RenderGame()
{
   
    for( int i = 0 ; i < gLEDCount; i++ )
    {
      if( i < gGameGlob.WorldBoundsStartLED || i >= gGameGlob.WorldBoundsEndLED )
        gLeds[i+1].setRGB(128,0,0);
      else
        gLeds[i+1].setRGB(0,0,0);
    }
    SetGameboardLED(gGameGlob.PlayerPos, PlayerColors[gGameGlob.PlayerColorIndex]);

    RenderEnemies();
    RenderBullets();
    

    FastLED.show();
}

void SetState( int state )
{
  gGameGlob.State = (GameState)state;
  gGameGlob.TimeInState = 0.0f;
  gGameGlob.FramesInState = 0;
}

void UpdateGameplay(float dt)
{
  float maxSpeed = 20.0f;

  // move the player location from the left knob
  float lPinValue = (float)analogRead(REFRESH_POT_PIN);
  float direction = (lPinValue - 512.0f)/512.0f;
  gGameGlob.PlayerPosFloat += direction * maxSpeed * dt;
  gGameGlob.PlayerPosFloat = fmod((gGameGlob.PlayerPosFloat + (float)gLEDCount), (float)gLEDCount);
  gGameGlob.PlayerPos = (int)gGameGlob.PlayerPosFloat;

  if( gGameGlob.StompPressed )
  {
    FireBullet(gGameGlob.PlayerPos, BULLET_SPEED * ((direction > 0.0f) ? 1.0f : -1.0f));
    gGameGlob.StompPressed = false;
  }

  SpawnRandomEnemy();

  UpdateWalls( dt );
  UpdateBullets( dt );
  UpdateEnemies( dt );

  if( gGameGlob.PlayerPos <= gGameGlob.WorldBoundsStartLED || gGameGlob.PlayerPos >= gGameGlob.WorldBoundsEndLED )
  {
    SetState( GameState_GameOver );
    gGameGlob.BulletCount = 0;
  }
}

void UpdateGameLogic(float dt)
{

    if( gGameGlob.State == GameState_AttractMode )
    {
      CRGB colors[] = { CRGB(128,128,0), CRGB(255,0,255), CRGB(0,255,0), CRGB(255,255,255) };
      for( int i = 0 ; i < gLEDCount; i++ )
      {
        gLeds[i+1] = colors[(i + gGameGlob.FramesInState/3) % ARRAYSIZE(colors)];
      }
      FastLED.show();
      if( gGameGlob.StompPressed )
      {
        SetState( GameState_Playing );
        gGameGlob.StompPressed = false;
      }
    }
    if( gGameGlob.State == GameState_Playing )
    { 
      UpdateGameplay(dt);
      RenderGame();
    }
    if ( gGameGlob.State == GameState_GameOver )
    {
      // Make the character color flash
      if( gGameGlob.TimeInState < 1.0f )
      {
        gGameGlob.PlayerColorIndex = (gGameGlob.PlayerColorIndex+1) % ARRAYSIZE(PlayerColors); 
        RenderGame();
      }
      else if( gGameGlob.TimeInState < 2.0f )
      {
        if( (gGameGlob.FramesInState % 2) == 1 )
        {
          RenderGame();
        }
        else
        {
          for( int i = 0 ; i < gLEDCount; i++ )
          {
            gLeds[i+1].setRGB(255,0,0);
          }
          FastLED.show();
        }
      }
      else if( gGameGlob.TimeInState < 3.0f )
      {
          for( int i = 0 ; i < gLEDCount; i++ )
          {
            gLeds[i+1].setRGB(255,0,0);
          }
          FastLED.show();
      }
      else
      {
        ResetGame();
        return;
      }
    }

    gGameGlob.TimeInState += dt;
    gGameGlob.FramesInState++;
    gGameGlob.SecondsInGame += dt;
}

void loop() 
{
  static int lastStompValue = LOW;
  int curStompValue = digitalRead(STOMP_PIN);
  if( curStompValue == HIGH && lastStompValue == LOW )
  {
    gGameGlob.StompPressed = true;
  }
  lastStompValue = curStompValue;

  static unsigned long gLastUpdate = micros();
  unsigned long currentTime = micros();
  if( (currentTime - gLastUpdate) > 33000 )
  {
    UpdateGameLogic(1.0f/30.0f);
    gLastUpdate = currentTime;
  }
    
}


