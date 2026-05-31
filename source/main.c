#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <stdlib.h>
#include <math.h>

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
int frame = 0;

float camera_x = 0.0f;
float camera_y = 600.0f;
float camera_z = 0.0f;
float camera_rot_x = 0.5f;
float camera_rot_y = 5.0f;

uint8_t nodes[MAP_SIZE];


int main(int argc, char **argv) {
    // setup
    GRRLIB_Init();
    
    // import font
    tex_BMfont5 = GRRLIB_LoadTexture(BMfont5_png);
    GRRLIB_InitTileSet(tex_BMfont5, 8, 16, 0); 

    // import images
    tex_shot = GRRLIB_LoadTexture(shot_png);
    GRRLIB_SetMidHandle(tex_shot, true);

    GRRLIB_Camera3dSettings(0.0f,3.0f,5.0f, 0,1,0, 0,-0.4,0);

    // wiimote setup
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);

    
    for (int i=0;i<60;i++) {
        GRRLIB_FillScreen(GRRLIB_BLACK);
        GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "loading...");
        GRRLIB_Render();
    }

    for (int i=0;i<MAP_SIZE;i++) {
        nodes[i] = rand() % 3;
    }
	
    while (project_running) {
        frame++;

        WPAD_ScanPads();

        for (int wiimote = 0; wiimote <= 3; wiimote++) {
            WPADData* data = WPAD_Data(wiimote);

            if(!data->data_present) {
                continue;
            }
            if (WPAD_ButtonsDown(wiimote) & WPAD_BUTTON_HOME) {
                project_running = false;
            }
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) {
            camera_rot_x -= 0.01f;
        }
        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) {
            camera_rot_x += 0.01f;
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
            camera_rot_y += 0.03f;
        }
        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
            camera_rot_y -= 0.03f;
        }

        // background
		GRRLIB_2dMode();
		// GRRLIB_Rectangle(0, 0, 640, 480, GRRLIB_BLUE, 1);
        if (background_color == 0) {
            GRRLIB_FillScreen(GRRLIB_BLUE);
        }
        else if (background_color == 1) {
            GRRLIB_FillScreen(GRRLIB_GREEN);
        }
        else if (background_color == 2) {
            GRRLIB_FillScreen(GRRLIB_RED);
        }

        GRRLIB_3dMode(0.1, 1000, 45, 0,1);
        GRRLIB_SetLightAmbient(0x111111FF);
        GRRLIB_SetLightDiff(0, (guVector){0.0f, 500.0f, 0.0f}, 20.0f, 1.0f, 0xCCCCCCFF);

        int node_number = 0;
        for (int i=0;i<8;i++) {
            for (int j=0;j<8;j++) {
                for (int k=0;k<8;k++) {
                    node_number++;
                    if (nodes[node_number] == 0) {
                        continue;
                    }
                    
                    GRRLIB_ObjectViewBegin();
                    GRRLIB_ObjectViewRotate(0, frame, 0);
                    GRRLIB_ObjectViewScale(50, 50, 50);
                    GRRLIB_ObjectViewTrans(200 + i * 50, 0 + k * 50, 0 + j * 50);
                    GRRLIB_ObjectViewEnd();
                    // GRRLIB_DrawSphere(0.5, 9, 9, 1, 0xFF0000FF);

                    if (nodes[node_number] == 1) {
                        GRRLIB_DrawCube(1, 1, GRRLIB_GREEN);
                    }
                    else if (nodes[node_number] == 2) {
                        GRRLIB_DrawCube(1, 1, 0x886400FF);
                    }
                }
            }
        }

        GRRLIB_Camera3dSettings(camera_x, camera_y, camera_z, // position
                        0, 1, 0,
                        sin(camera_rot_x) * 100, camera_rot_y * 100, cos(camera_rot_x) * 100 // look at
                        );

        // UI
        GRRLIB_2dMode();
        
        draw_wiimotes();
        
        GRRLIB_Render();
        // if (ui_number == 0) {
        //     GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "> Start");
        // }
        // else {
        //     GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "Start");
        // }

        // if (ui_number == 1) {
        //     GRRLIB_Printf(250, 225, tex_BMfont5, GRRLIB_WHITE, 1, "> background_color %d", background_color);
        // }
        // else {
        //     GRRLIB_Printf(250, 225, tex_BMfont5, GRRLIB_WHITE, 1, "background_color %d", background_color);
        // }
        // if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {
        //     ui_number++;
        // }
        // if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) {
        //     ui_number--;
        // }
    
        // if (ui_number > 1) {
        //     ui_number = 0;
        // }
        // if (ui_number < 0) {
        //     ui_number = 1;
        // }
        
        // if (ui_number == 1) {
        //     if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT) {
        //         background_color++;
        //     }
        //     if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT) {
        //         background_color--;
        //     }
    
        //     if (background_color > 2) {
        //         background_color = 0;
        //     }
    
        //     else if (background_color < 0) {
        //         background_color = 2;
        //     }
        // }
    
        // if (WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_BUTTON_RIGHT)) {
        //     if (ui_number == 0) {
        //         project_running = false;
        //     }
        // }
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