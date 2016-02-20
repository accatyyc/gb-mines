#include <gb/gb.h>
#include <gb/drawing.h>
#include <board.h>
#include <stdlib.h>
#include "game.h"

#define GRID_WIDTH 20
#define GRID_HEIGHT 18

unsigned char tiles[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00, // 0
	0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x1C,0x1C,0x04,0x04,0x04,0x04, // 1
	0x04,0x04,0x04,0x04,0x04,0x04,0x1F,0x1F,
	0x00,0x00,0x3C,0x3C,0x04,0x04,0x04,0x04, // 2
	0x3C,0x3C,0x20,0x20,0x20,0x20,0x3C,0x3C,
	0x00,0x00,0x3C,0x3C,0x04,0x04,0x04,0x04, // 3
	0x3C,0x3C,0x04,0x04,0x04,0x04,0x3C,0x3C,
	0x00,0x00,0x24,0x24,0x24,0x24,0x24,0x24, // 4
	0x3C,0x3C,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x3C,0x3C,0x20,0x20,0x20,0x20, // 5
	0x3C,0x3C,0x04,0x04,0x04,0x04,0x3C,0x3C,
	0x00,0x00,0x3C,0x3C,0x20,0x20,0x20,0x20, // 6
	0x3C,0x3C,0x24,0x24,0x24,0x24,0x3C,0x3C,
	0x00,0x00,0x3C,0x3C,0x04,0x04,0x04,0x04, // 7
	0x0E,0x0E,0x04,0x04,0x04,0x04,0x04,0x04,
	0x00,0x00,0x3C,0x3C,0x24,0x24,0x24,0x24, // 8
	0x3C,0x3C,0x24,0x24,0x24,0x24,0x3C,0x3C,
	0x00,0x00,0x00,0x7E,0x00,0x7E,0x00,0x7E, // Unopened
	0x00,0x7E,0x00,0x7E,0x00,0x7E,0x00,0x00,
	0x00,0x00,0x08,0x08,0x18,0x18,0x38,0x38, // Flagged
	0x18,0x18,0x08,0x08,0x08,0x08,0x00,0x00,
	0x18,0x18,0x24,0x3C,0x7E,0x5A,0xBD,0xFF, // Mine
	0xBD,0xFF,0x7E,0x5A,0x24,0x3C,0x18,0x18
};

unsigned char sprites[] =
{
	0xFF,0xFF,0x81,0x81,0x81,0x81,0x81,0x81, // Marker
	0x81,0x81,0xC1,0x81,0xA1,0xC1,0xFF,0xFF
};

enum tile {
	Zero = 0,
	One = 1,
	Two = 2,
	Three = 3,
	Four = 4,
	Five = 5,
	Six = 6,
	Seven = 7,
	Eight = 8,
	Unopened = 9,
	Flagged = 10,
	Mine = 11
};

char digit_to_tile(int digit)
{
	return (char)digit; // Actually the same for now.
}

void set_tile(int x, int y, unsigned char tile)
{
	set_bkg_tiles(x, y, 1, 1, &tile);
}

void move_marker(int x, int y)
{
	move_sprite(0, (SCREENWIDTH / GRID_WIDTH) * (x+1), (SCREENHEIGHT / GRID_HEIGHT) * (y+2));
}

int wrap(int value, int max)
{
	if (value < 0) return max;
	if (value > max) return 0;
	return value;
}

void render_board(struct board* board)
{
	int x, y;
	uint8_t* tile;
	char sprite;
	for (x = 0;x<board->width;x += 1)
	{
		for (y = 0;y < board->height;y += 1)
		{
			tile = get_tile_at(board, x, y);

			if (*tile&TILE_OPENED && *tile&TILE_MINE)
				sprite = Mine;
			else if (*tile&TILE_OPENED)
				sprite = digit_to_tile(adjacent_mine_count(tile));
			else if (*tile&TILE_FLAG)
				sprite = Flagged;
			else
				sprite = Unopened;

			set_tile(x, y, sprite);
		}
	}
}

void play_game()
{
	char input;
	struct board board;

	srand(DIV_REG);

	board_init(&board, GRID_WIDTH, GRID_HEIGHT, 0.1);
	board.cursor_x = board.width / 2;
	board.cursor_y = board.height / 2;

	// Setup background
	set_bkg_data(0, 12, tiles);

	SHOW_BKG;

	// Setup sprites
	set_sprite_data(0, 1, sprites);
	set_sprite_tile(0, 0);
	SHOW_SPRITES;
	SPRITES_8x8;

	render_board(&board);

	while (!board.game_over)
	{
		input = joypad();
		if (input&J_UP)
			board.cursor_y = wrap(board.cursor_y - 1, GRID_HEIGHT - 1);
		if (input&J_DOWN)
			board.cursor_y = wrap(board.cursor_y + 1, GRID_HEIGHT - 1);
		if (input&J_LEFT)
			board.cursor_x = wrap(board.cursor_x - 1, GRID_WIDTH - 1);
		if (input&J_RIGHT)
			board.cursor_x = wrap(board.cursor_x + 1, GRID_WIDTH - 1);
		if (input&J_A)
		{
			open_tile_at_cursor(&board);
			render_board(&board);
		}
		if (input&J_B)
		{
			toggle_flag_at_cursor(&board);
			render_board(&board);
		}
		move_marker(board.cursor_x, board.cursor_y);
		delay(75);
	}

	while (!(joypad()&J_START));
}