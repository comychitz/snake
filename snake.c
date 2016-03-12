#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>
#include <errno.h>
/********************************** SNAKE *************************************
 * NOTES:
 * -----
 * int direction: 0 - up               
 *                1 - right
 *                2 - down
 *                3 - left
 *                4 - timeout occurred
 *
 ******************************************************************************/
#define MAX_LENGTH  2048
#define TIMEOUT     100

#define TAIL_CHAR               'o'
#define HEAD_CHAR               '@' 
#define GOODIE_CHAR             'X'
#define EMPTY_CHAR              ' '
#define VERTICAL_BORDER_CHAR    '#'
#define HORIZONTAL_BORDER_CHAR  '#'

#define HIGHSCORES_FILENAME     "/Users/true/snake/highscores"

/* globals */
short score = 0;
short MAX_ROW;
short MAX_COL;
int PREV_KEY = 0;
int PREV_DIRECTION = 0;
bool gameOver = false;
bool gotGoodie = false;
int highscores[3];

/* main data structures */
struct unit 
{
  char symbol;
  int  x;
  int  y;
};
struct snake
{
  struct unit body[MAX_LENGTH];
  int length;

};
struct unit thegoodie;
struct snake theSnake;

/* function declarations */
void presetup( void );
void setup( void );
int readInput( void );
void advanceSnake( int direction );
void moveHead ( int direction );
void placeGoodie( void );
void gameOverSeq( void );
void drawBorder( char, char, char, char );
void teardown( void );
void getHighscores( void );
void showHighScores( void );
void saveHighScores( void );

/* main */
int main( int argc, const char *argv[] )
{
  presetup();
  setup();

  while( true )
  {
    int directionPressed = readInput(); 
    if( directionPressed == -2 ) // quit
    {
      break;
    } 
    advanceSnake( directionPressed );
    refresh();
    if( gameOver )
    {
      saveHighScores();
      gameOverSeq();
    }
  }
  teardown();
  return 0;
}

/*******************************************************************************
 * Description: the setup function that doesn't actually setup the game, but
 * does the ncurses window setup, other system-related calls
 * 
 * Inputs: 
 * 
 * Returns: 
 ******************************************************************************/
void presetup( void )
{
  getHighscores();

  /* prepare the screen */
  initscr();
  cbreak();
  noecho(); // turns off echoing characters read by getch()
  curs_set(0); // hide the cursor
  MAX_ROW = LINES;
  MAX_COL = COLS;
  srand(time(NULL)); // seed for random number generator

  /* check for size of terminal window bigger than 30 by 30 */
  if( MAX_ROW <= 30 || MAX_COL <= 30 )
  {
    teardown();
    printf( "your terminal window is too small! try again.\n" );
    exit(-1);
  }

  /* clear screen */
  clear();

  /* draw border */
  drawBorder( '#', '#', '#', '#' ); 

  /* write a message that says press something to start */
  char welcomeMesg1[] = "Welcome to Snake!";
  char welcomeMesg2[] = "Press any key to begin playing";
  mvprintw( MAX_ROW/2 - 5, (MAX_COL-strlen(welcomeMesg1))/2, "%s", welcomeMesg1 );
  mvprintw( MAX_ROW/2 - 2, (MAX_COL-strlen(welcomeMesg2))/2, "%s", welcomeMesg2 );

  showHighScores();

  /* wait for something to be pressed, then return */
  getch();
}
 

/*******************************************************************************
 * Description: setup function that prepares the windows, gets data ready
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void setup( void )
{ 
  /* clear screen */
  clear();

  /* draw the border for the game */
  drawBorder( 'v', '<', '^', '>' );

  /* put the score and the instructions at the bottom */
  mvprintw( MAX_ROW-1, 0, "Score: %d", score );
  char temp[] = "[P]ause [Q]uit [R]estart";
  mvprintw( MAX_ROW-1, MAX_COL-strlen(temp)-1, "%s", temp );

  timeout( TIMEOUT ); // sets the timeout for waiting for keyboard input

  /* put the snake it its original position */
  PREV_KEY = KEY_RIGHT; // have it going to the right when starting up
  PREV_DIRECTION = 1;
  mvaddch( 10, 5, HEAD_CHAR );
  mvaddch( 10, 4, TAIL_CHAR );
  mvaddch( 10, 3, TAIL_CHAR );
  mvaddch( 10, 2, TAIL_CHAR );
  theSnake.body[0].symbol = HEAD_CHAR;
  theSnake.body[0].x = 5;
  theSnake.body[0].y = 10;
  theSnake.body[1].symbol = TAIL_CHAR;
  theSnake.body[1].x = 4;
  theSnake.body[1].y = 10;
  theSnake.body[1].symbol = TAIL_CHAR;
  theSnake.body[2].x = 3;
  theSnake.body[2].y = 10;
  theSnake.body[3].symbol = TAIL_CHAR;
  theSnake.body[3].x = 2;
  theSnake.body[3].y = 10;

  theSnake.length = 4;
  
  /* place the goodie somewhere */
  thegoodie.symbol = GOODIE_CHAR;
  thegoodie.x = -1;
  thegoodie.y = -1; 
  placeGoodie();

  score = 0;

  refresh();
}

/*******************************************************************************
 * Description: waits for key stroke input, or until timeout occurs
 * 
 * Inputs: void
 * 
 * Returns: integer value following the direction definition
 ******************************************************************************/
int readInput( void )
{
  int direction;
  bool flag = true, flag2 = false;
  while( flag || flag2 )
  {
    int pressedKey = getch(); 

    if( PREV_KEY == pressedKey &&
        (pressedKey == 65      || 
        pressedKey == 66       || 
        pressedKey == 67       ||
        pressedKey == 68)       )
    {
      return PREV_DIRECTION;
    }
    else
    {
      PREV_KEY = pressedKey;
    }

    switch( pressedKey )
    {
      case 65: // up
        direction = 0;
        flag = false;
        break;
      case 67: // right
        direction = 1;
        flag = false;
        break;
      case 66: // down
        direction = 2;
        flag = false;
        break;
      case 68: // left
        direction = 3;
        flag = false;
        break;
      case 112: // [p]ause
        if( !flag2 ) {
          timeout( 900000 ); // after 15 minutes of pause, just go
          mvprintw( MAX_ROW-1, (MAX_COL-7)/2, "Paused!" );
          flag2 = true;
        } else {
          timeout( TIMEOUT );
          mvprintw( MAX_ROW-1, (MAX_COL-7)/2, "       " );
          flag2 = false;
          return PREV_DIRECTION;
        }
        break;
      case 113: // [q]uit
        direction = -2;
        return -2;
        break;
      case 114: // [r]estart
        setup();
        return 4;
        break;
      case ERR: // -1
        direction = 4;
        flag = false;
        break;
      default:
        break;
    }
    /*
    char temp[128];
    sprintf( temp, "echo %d >> keysPressed", pressedKey );
    system( temp );
    */
  }

  return direction;
}

/*******************************************************************************
 * Description: moves the snake one step, based on the input direction
 * 
 * Inputs: int direction
 * 
 * Returns: void
 ******************************************************************************/
void advanceSnake( int direction )
{
  if( direction == 4 || direction == -1 )
  { // continue the same direction as we are going
    direction = PREV_DIRECTION;
  }
  else 
  {
    PREV_DIRECTION = direction;
  }

  /* clear the snake */
  int i;
  for( i = 0; i < theSnake.length; i++) 
  {
    mvaddch( theSnake.body[i].y,
             theSnake.body[i].x,
             EMPTY_CHAR );
  }

  /* move the head to the corresponding location */
  struct unit lastTail = theSnake.body[theSnake.length-1];
  
  struct unit tempSnake[MAX_LENGTH];

  /* shift the rest of the snake */
  for( i = 0; i < theSnake.length; i++) 
  {
    tempSnake[i] = theSnake.body[i];
  }
  for( i = 1; i < theSnake.length; i++) 
  {
    theSnake.body[i] = tempSnake[i-1];
  }

  moveHead( direction );

  /* draw the new snake. draw it backwards so the head is always on top */
  for( i = 1; i < theSnake.length; i++ )
  {
    mvaddch( theSnake.body[i].y,
             theSnake.body[i].x,
             TAIL_CHAR );
  }
  mvaddch( theSnake.body[0].y, theSnake.body[0].x, theSnake.body[0].symbol );

  if( gotGoodie )
  { /* append another tail to the snake */
    theSnake.length++;
    theSnake.body[theSnake.length-1] = lastTail;
    mvaddch( lastTail.y, lastTail.x, lastTail.symbol);
    placeGoodie(); // place a new goodie
    score++;
    mvprintw( MAX_ROW-1, 0, "Score: %d", score );
    gotGoodie = false;
  }
}

/*******************************************************************************
 * Description: function to move the head toward the corresponding direction
 * 
 * Inputs: int direction - the direction to move the head
 * 
 * Returns: void
 ******************************************************************************/
 void moveHead( int direction )
{
  switch( direction )
  {
    case 0:
      /* move up */
      theSnake.body[0].y -= 1;
      break;
    case 1:
      /* move right */
      theSnake.body[0].x += 1;
      break;
    case 2:
      /* move down */
      theSnake.body[0].y += 1;
      break;
    case 3:
      /* move left */
      theSnake.body[0].x -= 1;
      break;
    default:
      break;
  }

  /* check if we hit a goodie */
  if( theSnake.body[0].x == thegoodie.x &&
      theSnake.body[0].y == thegoodie.y )
  {
    gotGoodie = true;
  }

  /*  check we didn't hit a wall or ourself */
  if( theSnake.body[0].x == 0          ||
      theSnake.body[0].x == MAX_COL-1  ||
      theSnake.body[0].y == 0          ||
      theSnake.body[0].y == MAX_ROW-2   ) 
  {
    gameOver = true;
    return;
  }
  int i;
  for( i = 1; i < theSnake.length; i++) 
  {
    if( theSnake.body[i].x == theSnake.body[0].x &&
        theSnake.body[i].y == theSnake.body[0].y )
    {
      gameOver =true;
      break;
    }
  }
}

/*******************************************************************************
 * Description: teardown function for the window
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void teardown( void )
{
  saveHighScores();
  endwin();
}
 
/*******************************************************************************
 * Description: function that randomly places the goodie somewhere
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void placeGoodie( void )
{
  // erase the old goodie
  if( thegoodie.x != -1 || thegoodie.y != -1 )
  {
    mvaddch( thegoodie.y, thegoodie.x, EMPTY_CHAR );
  }

  bool flag = true;
  int newX, newY;
  while( flag )
  {
    newX = rand() % (MAX_COL-2) + 1;
    newY = rand() % (MAX_ROW-3) + 1;
    
    int i;
    flag = false;
    for( i = 0; i < theSnake.length; i++) 
    {
      if( theSnake.body[i].x == newX  ||
          theSnake.body[i].y == newY )
      {
         flag = true;
         break;
      }
    }
  }
  thegoodie.x = newX;
  thegoodie.y = newY;
  mvaddch( newY, newX, GOODIE_CHAR );
}
 
/*******************************************************************************
 * Description: function that runs the sequence when gameOver is true
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
 void gameOverSeq( void )
{
  char gameOverMsg[] = "Game Over!";
  char gameOverMsg2[] = "Press any key to restart";
  mvprintw( MAX_ROW/2 - 5, (MAX_COL-strlen(gameOverMsg))/2, "%s", gameOverMsg );
  mvprintw( MAX_ROW/2 - 3, (MAX_COL-strlen(gameOverMsg2))/2, "%s", gameOverMsg2 );

  timeout(10000); // 10 seconds or it will restart itself
  getch();
  setup();
  gameOver = false;
}

/*******************************************************************************
 * Description: draws border at edge of screen
 * 
 * Inputs: 
 * 
 * Returns: void
 ******************************************************************************/
void drawBorder( char uchar, char rchar, char dchar, char lchar)
{
  /* draw a cool border */
  int i;
  for( i=0; i<MAX_COL; i++)
  {
    if( i % 2 == 0 )
      mvaddch( 0, i, uchar );
    else
      mvaddch( 0, i, ' ' );
    if( i % 2 == 0 )
      mvaddch( MAX_ROW-2, i, dchar );
    else
      mvaddch( MAX_ROW-2, i, ' ' );
  }
  for( i=1; i<MAX_ROW-2; i++ )
  {
    mvaddch( i, 0, lchar );
    mvaddch( i, MAX_COL-1, rchar );
  }
}
 
/*******************************************************************************
 * Description: get the high score from the high scores file, if file isn't 
 * there this function will create it
 * 
 * Inputs: void
 * 
 * Returns: void 
 ******************************************************************************/
void getHighscores( void )
{
  char *line = NULL;
  int count = 0;
  size_t len = 0;
  ssize_t read;

  FILE *f = fopen( HIGHSCORES_FILENAME, "r+" ); 

  if( !f )
  {
    printf( "Failed to open file %s: %s (%d)", 
            HIGHSCORES_FILENAME, 
            strerror(errno), 
            errno );
    return;
  }

  while( (read = getline( &line, &len, f)) != -1 )
  {
    highscores[count] = atoi( line );
    count++;
  }

  int i;
  for( i = count; i < 3; i++) 
  {
    highscores[i] = 0;
  }

  fclose( f );
}

/*******************************************************************************
 * Description: display high scores on the welcome screen
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void showHighScores( void )
{
  if( highscores[0] > 0 || highscores[1] > 0 || highscores[2] > 0 )
  {
    char title[] = "High Scores";
    mvprintw( MAX_ROW/2 + 5, (MAX_COL-(strlen(title)))/2, "%s", title );
    int i;
    for( i = 0; i < 3; i++ )
    {
      if( highscores[i] > 0 )
      {
        char score[32];
        sprintf( score, "%d. %d", i+1, highscores[i] );
        mvprintw( MAX_ROW/2 + 5 + (i+1)*2, (MAX_COL-(sizeof(score)))/2 + 12, "%s", score );
      }
    }
  }
}

/*******************************************************************************
 * Description: saves high scores to file
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void saveHighScores( void )
{
  int i;
  for( i=0; i<3; i++ )
  {
    if( highscores[i] < score )
    {
      int j;
      for( j=2; j>i; j-- )
      {
        highscores[j] = highscores[j-1];
      }
      highscores[i] = score;
      break;
    }
  } 

  FILE *f = fopen( HIGHSCORES_FILENAME, "w+" );
  if( !f )
  {
    printf( "Failed to open file %s: %s (%d)", 
            HIGHSCORES_FILENAME,
            strerror(errno),
            errno );
    return;
  }
  
  for( i = 0; i < 3; i++) 
  {
    fprintf( f, "%d\n", highscores[i] );
  }

  fclose( f );
}
