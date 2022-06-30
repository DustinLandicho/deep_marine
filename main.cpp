#include <stdio.h>

#include "Game.h"
#include "classes/Sprite.h"
#include "classes/Player.h"
#include "classes/Ammo.h"

// INGAME ENTITIES
// character model
class Submarine {
public:
	FIBITMAP* Fi_submarine;
	uint8_t SubmarineBits;
	int SubmarinePitch;

	// Constructor
	Submarine(FIBITMAP* fi_submarine, uint8_t submarineBits, int submarinePitch) {

	}

};

// obstacle entities
class Brick {
public:
	FIBITMAP* Fi_brick;
	uint8_t BrickBits;
	int BrickPitch;

	// Constructor
	Brick(FIBITMAP* fi_brick, uint8_t brickBits, int brickPitch) {

	}

};

class Seaweed {
public:
	FIBITMAP* Fi_seaweed;
	uint8_t SubmarineBits;
	int SubmarinePitch;

	// Constructor
	Seaweed(FIBITMAP* fi_seaweed, uint8_t seaweedBits, int seaweedPitch) {

	}

};


// hostile entities
class Shark {
public:
	FIBITMAP* Fi_shark;
	uint8_t SharkBits;
	int SharkPitch;

	// Constructor
	Shark(FIBITMAP* fi_shark, uint8_t sharkBits, int sharkPitch) {

	}

};

class Squid {
public:
	FIBITMAP* Fi_squid;
	uint8_t SquidBits;
	int SquidPitch;

	// Constructor
	Squid(FIBITMAP* fi_squid, uint8_t squidBits, int squidPitch) {

	}

};


// additional life entity
class Coral {
public:
	FIBITMAP* Fi_coral;
	uint8_t CoralBits;
	int CoralPitch;

	// Constructor
	Coral(FIBITMAP* fi_coral, uint8_t coralBits, int coralPitch) {

	}

};


// weapon entity
class Starfish {
public:
	FIBITMAP* Fi_starfish;
	uint8_t StarfishBits;
	int StarfishPitch;

	// Constructor
	Starfish(FIBITMAP* fi_starfish, uint8_t starfishBits, int starfishPitch) {

	}

};

int main()
{
    struct mfb_window* window = mfb_open_ex("Project", WINDOW_WIDTH, WINDOW_HEIGHT, WF_RESIZABLE); // width x height
    if (!window)
        return 0;

    uint32_t* frame_buffer = (uint32_t*)malloc(BG_WIDTH * BG_HEIGHT * 4); // width x height

    uint8_t* bg = load_asset(FIF_PNG, "assets/bg.png");

    Player player = Player(PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, 0, 0, 3, FIF_PNG, "assets/submarine.png");
    Ammo ammo = Ammo(50, 50, 0, 0, 0, 0, FIF_PNG, "assets/ammo.png");

    Sprite obs1 = Sprite(100, 100, 1000, 0, 0, 0, FIF_PNG, "assets/b1.png");
    Sprite obs2 = Sprite(100, 100, 2310, 360, 0, 0, FIF_PNG, "assets/b1.png");
    Sprite obs3 = Sprite(100, 100, 3000, 620, 0, 0, FIF_PNG, "assets/b1.png");
    Sprite obs4 = Sprite(100, 100, 1000, 360, 0, 0, FIF_PNG, "assets/b1.png");
    Sprite obstacles[4] = { obs1, obs2, obs3, obs4 };

    game_data data;
    data.player = &player;
    data.ammo = &ammo;
    data.obstacles = obstacles;
    data.buffer = frame_buffer;


    // Load background to the frame buffer
    for (int i = 0; i < BG_WIDTH * BG_HEIGHT * 3; i += 3) {
        frame_buffer[i / 3] = (bg[i + 2] << 16) | (bg[i + 1] << 8) | bg[i];
    }

    mfb_set_keyboard_callback(window, key_press_handler);
    mfb_set_user_data(window, (void*)&data);

    do {
        static int bgx = 0;
        static int bgy = 0;
        static bool lose = false;
        
        data.bgx = &bgx;
        data.bgy = &bgy;

        // Scrolls background until x-axis limit
        if (bgx >= BG_WIDTH - WINDOW_WIDTH) {
            bgx = BG_WIDTH - WINDOW_WIDTH;
            if (ammo.isFired) {
                //bg = load_asset(FIF_PNG, "assets/bg2.png"); // load next stage
                printf("Next Level Start");
                bgx = 0;
                data.player->x = 0;
            }
        }
        else {
            data.player->x += 4;
            bgx += 5;
        }

        // Update background based on player's movement 
        update_bg(frame_buffer, bg, bgx, bgy);
        if (data.player->x <= bgx - data.player->width) {   // Detects when player is out of bounds
            printf("Game Over");
            lose = true;
            //break;    // Game will crash if uncommented whenever screen catches up to sprite
        }

        // Display player's sprite
        display_asset(frame_buffer, &player, 0, 0);

        // Display obstacles
        for (int i = 0; i < 4; i++)
            display_asset(frame_buffer, &obstacles[i], 0, 0);

        if (ammo.isFired) {
            display_asset(frame_buffer, &ammo, 0, 0);
            ammo.x += 20;
            if(ammo.x >= BG_WIDTH)ammo.isFired=!ammo.isFired;
        }

        // Update window
        uint32_t* stride = frame_buffer + (BG_WIDTH * bgy + bgx);
        int state = mfb_update_crop(window, stride, WINDOW_WIDTH, WINDOW_HEIGHT, BG_WIDTH);

        if (state < 0) {
            window = NULL;
            break;
        }
    } while (mfb_wait_sync(window));


    return 0;
}

void key_press_handler(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed)
{
    game_data* data = (game_data*)mfb_get_user_data(window);
    if (isPressed) {
        if (key == KB_KEY_LEFT) {
            data->player->x_old = data->player->x;
            data->player->x -= 20;
            data->player->xdir = 1;
        }
        else if (key == KB_KEY_RIGHT) {
            data->player->x_old = data->player->x;
            data->player->x += 20;
            data->player->xdir = 1;
        }
        else if (key == KB_KEY_UP) {
            data->player->y_old = data->player->y;
            data->player->y -= 20;

            // Update bg y position, if upper bound of the window is touched
            if (data->player->y > 0 && data->player->y == *(data->bgy)) {
                *(data->bgy) -= 20;

                if (*(data->bgy) <= 0) { // Make sure player sprite does not go beyond the upper bound of the frame buffer
                    *(data->bgy) = 0;
                }
            }
        }
        else if (key == KB_KEY_DOWN) {
            data->player->y_old = data->player->y;
            data->player->y += 20;

            // Update bg y position, if lower bound touched
            if (data->player->y >= WINDOW_HEIGHT - PLAYER_HEIGHT - 1 + *(data->bgy)) {
                *(data->bgy) += 20;

                if (*(data->bgy) >= BG_HEIGHT - WINDOW_HEIGHT - 1) { // Make sure player sprite does not go beyond the lower bound of the frame buffer
                    *(data->bgy) = BG_HEIGHT - WINDOW_HEIGHT - 1;
                }
            }
        }
        else if (key == KB_KEY_SPACE) {
            data->ammo->x = data->player->x + data->player->width;
            data->ammo->y = data->player->y + data->player->height / 2 - data->ammo->height / 2;
            printf("%d %d \n", data->ammo->x, data->ammo->y);

            data->ammo->isFired = true;
        }

        // Make sure player sprite does not go beyond boundaries
        if (data->player->x >= *(data->bgx) + WINDOW_WIDTH - PLAYER_WIDTH) { // Right bound touched
            data->player->x = *(data->bgx) + WINDOW_WIDTH - PLAYER_WIDTH;
        }
        else if (data->player->x <= *(data->bgx)) {// Left bound touched
            data->player->x = *(data->bgx);
        }

        if (data->player->y >= BG_HEIGHT - PLAYER_HEIGHT - 1) { // Lower bound touched
            data->player->y = BG_HEIGHT - PLAYER_HEIGHT - 1;
        }
        else if (data->player->y <= 0) {// Upper bound touched
            data->player->y = 0;
        }

        // Check for collisions
        bool collision = check_player_collision(data->player, data->obstacles, 4);
    }
}
