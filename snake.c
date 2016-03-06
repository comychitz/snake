#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
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
#define MAX_LENGTH  64
#define TIMEOUT     500

#define TAIL_CHAR               'o'
#define HEAD_CHAR               '>' 
#define GOODIE_CHAR             '!'
#define EMPTY_CHAR              ' '
#define VERTICAL_BORDER_CHAR    '#'
#define HORIZONTAL_BORDER_CHAR  '#'

/* globals */
short MAX_ROW;
short MAX_COL;
int PREV_KEY = 0;
int PREV_DIRECTION = 0;
bool gameOver = false;
bool gotGoodie = false;

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
void setup( void );
int readInput( void );
void advanceSnake( int direction );
void moveHead ( int direction );
void placeGoodie( void );
void gameOverSeq( void );
void teardown( void );

/* main */
int main( int argc, const char *argv[] )
{
  setup();
  while( true )
  {
    int directionPressed = readInput(); 
    advanceSnake( directionPressed );
    refresh();
    if( gameOver )
    {
      gameOverSeq();
    }
  }
  teardown();
  return 0;
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
  /* prepare the screen */
  initscr();
  cbreak();
  noecho(); // turns off echoing characters read by getch()
  MAX_ROW = LINES;
  MAX_COL = COLS;
  
  /* draw a cool border */
  int i;
  for( i=0; i<MAX_COL; i++)
  {
    if( i % 2 == 0 )
      mvaddch( 0, i, HORIZONTAL_BORDER_CHAR );
    else
      mvaddch( 0, i, ' ' );
    if( i % 2 == 0 )
      mvaddch( MAX_ROW-1, i, HORIZONTAL_BORDER_CHAR );
    else
      mvaddch( MAX_ROW-1, i, ' ' );
  }
  for( i=1; i<MAX_ROW-1; i++ )
  {
    mvaddch( i, 0, VERTICAL_BORDER_CHAR );
    mvaddch( i, MAX_COL-1, VERTICAL_BORDER_CHAR );
  }
  
  /* write a message that says press something to start */
  char welcomeMesg1[] = "Welcome to Snake!";
  char welcomeMesg2[] = "Press any key to begin playing";
  mvprintw( MAX_ROW/2 - 5, (MAX_COL-strlen(welcomeMesg1))/2, "%s", welcomeMesg1 );
  mvprintw( MAX_ROW/2 - 2, (MAX_COL-strlen(welcomeMesg2))/2, "%s", welcomeMesg2 );

  /* wait for something to be pressed, then return */
  getch();
  clear();

  /* draw a cool border */
  for( i=0; i<MAX_COL; i++)
  {
    if( i % 2 == 0 )
      mvaddch( 0, i, HORIZONTAL_BORDER_CHAR );
    else
      mvaddch( 0, i, ' ' );
    if( i % 2 == 0 )
      mvaddch( MAX_ROW-1, i, HORIZONTAL_BORDER_CHAR );
    else
      mvaddch( MAX_ROW-1, i, ' ' );
  }
  for( i=1; i<MAX_ROW-1; i++ )
  {
    mvaddch( i, 0, VERTICAL_BORDER_CHAR );
    mvaddch( i, MAX_COL-1, VERTICAL_BORDER_CHAR );
  }

  timeout( TIMEOUT ); // sets the timeout for waiting for keyboard input

  /* put the snake it its original position */
  PREV_KEY = KEY_RIGHT; // have it going to the right when starting up
  PREV_DIRECTION = 1;
  mvaddch( 10, 5, HEAD_CHAR );
  mvaddch( 10, 4, TAIL_CHAR );
  theSnake.body[0].symbol = HEAD_CHAR;
  theSnake.body[0].x = 5;
  theSnake.body[0].y = 10;
  theSnake.body[1].symbol = TAIL_CHAR;
  theSnake.body[1].x = 4;
  theSnake.body[1].y = 10;
  theSnake.length = 2;
  
  /* place the goodie somewhere */
  thegoodie.symbol = GOODIE_CHAR;
  thegoodie.x = -1;
  thegoodie.y = -1; 
  placeGoodie();

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

  int pressedKey = getch(); 
  /* TODO - * make sure the timeout has been waiting before continueing */
  if( PREV_KEY == pressedKey )
  {
    return PREV_DIRECTION;
  }
  else 
  {
    PREV_KEY = pressedKey;
  }

  char temp[128];
  sprintf( temp, "echo %d > test", pressedKey );
  system( temp );
  exit(-1);
  
  switch( pressedKey )
  {
    case KEY_UP:
      direction = 0;
      break;
    case KEY_RIGHT:
      direction = 1;
      break;
    case KEY_DOWN:
      direction = 2;
      break;
    case KEY_LEFT:
      direction = 3;
      break;
    case ERR:
      direction = 4;
      break;
    default:
      direction = 4;
      break;
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
  if( direction == 4 )
  { // continue the same direction as we are going
    direction = PREV_DIRECTION;
  }
  else 
  {
    PREV_DIRECTION = direction;
  }

  /* move the head to the corresponding location */
  struct unit lastHead = theSnake.body[0];
  moveHead( direction );

  /* fill where the head was with a tail char */
  mvaddch( lastHead.y, lastHead.x, TAIL_CHAR );

  /* update the contents of theSnake data structure */
  struct unit lastTail = theSnake.body[theSnake.length-1];
  int i;
  for( i = 1; i < theSnake.length; i++) 
  {
    if( i == 1 )
    {
      theSnake.body[i].x = lastHead.x;
      theSnake.body[i].y = lastHead.y;
    }
    else if( i == theSnake.length-2 )
    {
      theSnake.body[i].x = theSnake.body[i+1].x;
      theSnake.body[i].y = theSnake.body[i+1].y;
      break;
    }
  }

  if( !gotGoodie )
  { /* erase the last tail */
    mvaddch( lastTail.y, 
             lastTail.x, 
             EMPTY_CHAR );
  }
  else 
  { /* append another tail to the snake */
    theSnake.length++;
    theSnake.body[theSnake.length-1] = lastTail;
    placeGoodie(); // place a new goodie
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
  mvaddch( theSnake.body[0].y, theSnake.body[0].x, theSnake.body[0].symbol );

  /* TODO - check we didn't hit a wall or ourself */
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
  /* TODO */
  // don't forget to erase where the goodie is before drawing new one
  // and make sure its not somewhere the snake is currently
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
  /* TODO */
}
