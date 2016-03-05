#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
#define MAX_LENGTH  (64)
#define TIMEOUT     (1)

/* main data structures */
struct unit 
{
  char symbol;
  int  x;
  int  y;
}
typedef struct unit thegoodie;
struct unit snake[MAX_LENGTH];

/* function declarations */
void setup( void );
int readInput( void );
void advanceSnake( int direction );
void refresh( void );

/* main */
int main( int argc, const char *argv[] )
{
  setup();
  while( true )
  {
    int directionPressed = readInput(); 
    advanceSnake( directionPressed );
    refresh();
  }
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
  /* TODO */
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
  int direction = 4;

  /* TODO */

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
  /* TODO */
}

/*******************************************************************************
 * Description: refreshes the data on the screen
 * 
 * Inputs: void
 * 
 * Returns: void
 ******************************************************************************/
void refresh( void )
{
  /* TODO */
}
