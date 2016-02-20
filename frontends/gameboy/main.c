#include <gb/gb.h>
#include <gb/drawing.h>
#include <asm/types.h>
#include <board.h>
#include "start.h"
#include "game.h"


void main()
{
	DISPLAY_ON;
	while (1)
	{
		show_start();
		play_game();
		delay(250);
	}
}
