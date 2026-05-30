#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>

#include "../source/globals.h"

// Font
#include "BMfont5_png.h"
GRRLIB_texImg *tex_BMfont5;

//Images
#include "shot_png.h"
GRRLIB_texImg *tex_shot;

// colors
#define GRRLIB_BLACK  0x000000FF
#define GRRLIB_WHITE  0xFFFFFFFF
#define GRRLIB_RED    0xFF0000FF
#define GRRLIB_GREEN  0x00FF00FF
#define GRRLIB_BLUE   0x0000FFFF
#define GRRLIB_YELLOW 0xFFFF00FF

void draw_wiimotes();

bool project_running = true;

int ui_number = 0;
int background_color = 0;

int main(int argc, char **argv) {
    // setup
    GRRLIB_Init();
    
    // import font
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 

    // import images
    tex_shot = GRRLIB_LoadTexture(shot_png);
    GRRLIB_SetMidHandle(tex_shot, true);

    // wiimote setup
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	
    while (project_running) {
        WPAD_ScanPads();

        for (int wiimote = 0; wiimote <= 3; wiimote++) {
            if (WPAD_ButtonsDown(wiimote) & WPAD_BUTTON_HOME) {
                project_running = false;
            }
        }

        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {
            ui_number++;
        }
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) {
            ui_number--;
        }

        if (ui_number > 1) {
            ui_number = 0;
        }
        if (ui_number < 0) {
            ui_number = 1;
        }
        
        if (ui_number == 1) {
            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT) {
                background_color++;
            }
            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT) {
                background_color--;
            }

            if (background_color > 2) {
                background_color = 0;
            }

            else if (background_color < 0) {
                background_color = 2;
            }
        }

        if (WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT)) {
            if (ui_number == 0) {
                project_running = false;
            }
        }

        if (background_color == 0) {
            GRRLIB_FillScreen(GRRLIB_BLUE);
        }
        else if (background_color == 1) {
            GRRLIB_FillScreen(GRRLIB_GREEN);
        }
        else if (background_color == 2) {
            GRRLIB_FillScreen(GRRLIB_RED);
        }

        draw_wiimotes();
        
        if (ui_number == 0) {
            GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "> Start");
        }
        else {
            GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "Start");
        }

        if (ui_number == 1) {
            GRRLIB_Printf(250, 225, tex_BMfont5, GRRLIB_WHITE, 1, "> background_color %d", background_color);
        }
        else {
            GRRLIB_Printf(250, 225, tex_BMfont5, GRRLIB_WHITE, 1, "background_color %d", background_color);
        }

        GRRLIB_Render();
    }

    GRRLIB_FreeTexture(tex_shot);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_Exit();
    return 0;
}


void draw_wiimotes() {
    WPAD_ScanPads();

    for (int wiimote = 0; wiimote <= 3; wiimote++) {
        WPADData* data = WPAD_Data(wiimote);

        if(!data->data_present) {
            continue;
        }

        if (wiimote == 0) {
            GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_shot, 1, 0.15, 0.15, GRRLIB_BLUE);
        }
        else if (wiimote == 1) {
            GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_shot, 1, 0.15, 0.15, GRRLIB_RED);
        }
        else if (wiimote == 2) {
            GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_shot, 1, 0.15, 0.15, GRRLIB_GREEN);
        }
        else {
            GRRLIB_DrawImg(data->ir.x, data->ir.y, tex_shot, 1, 0.15, 0.15, GRRLIB_YELLOW);
        }
    }
}