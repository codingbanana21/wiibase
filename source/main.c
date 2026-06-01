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
bool is_cube_visible(float cube_x, float cube_y, float cube_z);

bool project_running = true;

int ui_number = 0;
int frame = 0;

// camera setup
float camera_x = 0.0f;
float camera_y = 0.0f;
float camera_z = 0.0f;

float camera_rot_x = 0.0f;
float camera_rot_y = 1.0f;

float camera_look_x;
float camera_look_y;
float camera_look_z;


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

    GRRLIB_FillScreen(GRRLIB_BLACK);
    GRRLIB_Printf(250, 200, tex_BMfont5, GRRLIB_WHITE, 1, "loading...");
    GRRLIB_Render();

    for (int i=0;i<MAP_SIZE;i++) {
        nodes[i] = 0;
    }

    int node_number = 0;
        
    for (int i=0;i<32;i++) {
        for (int j=0;j<32;j++) {
            int hight = rand() % 5 + 2;
            // int hight = 7;
            node_number += 8 - hight;

            nodes[node_number] = 1;
            node_number++;
            hight--;

            while (hight > 0) {
                // nodes[node_number] = rand() % 2 + 1;
                nodes[node_number] = 2;
                node_number++;
                hight--;
            }
        }
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
            camera_rot_x -= 0.02f;
        }
        else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) {
            camera_rot_x += 0.02f;
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
            camera_rot_y += 0.02f;
        }
        else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
            camera_rot_y -= 0.02f;
        }

        if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
            camera_x += camera_look_x;
            camera_z += camera_look_z;
        }

        camera_look_x = sin(camera_rot_x) * cos(camera_rot_y);
        camera_look_y = sin(camera_rot_y);
        camera_look_z = cos(camera_rot_x) * cos(camera_rot_y);

        GRRLIB_Camera3dSettings(camera_x, camera_y, camera_z, // pos
                        0, 1, 0,
                        camera_x + camera_look_x, 
                        camera_y + camera_look_y, 
                        camera_z + camera_look_z // look at
                        );

        // background
		GRRLIB_2dMode();
        GRRLIB_FillScreen(GRRLIB_BLUE);

        GRRLIB_3dMode(0.1, 1000, 70, 0,1);
        GRRLIB_SetLightAmbient(0x111111FF);
        GRRLIB_SetLightDiff(0, (guVector){0.0f, 64.0f, 0.0f}, 20.0f, 1.0f, 0xCCCCCCFF);

        GX_SetCullMode(GX_CULL_FRONT);

        GRRLIB_ObjectViewBegin();
        GRRLIB_ObjectViewRotate(0, 0, 0);
        GRRLIB_ObjectViewScale(512, 16, 512);
        GRRLIB_ObjectViewTrans(0, -64, 0);
        GRRLIB_ObjectViewEnd();

        GRRLIB_DrawCube(1, 1, GRRLIB_BLACK);

        node_number = 0;
        int nodes_drawn = 0;

        for (int x=0;x<32;x++) {
            for (int z=0;z<32;z++) {
                for (int y=0;y<8;y++) {
                    node_number++;

                    if (nodes[node_number] == 0 || nodes_drawn >= 255) {
                        continue;
                    }
                    
                    float cube_x = x * 16 - 64;
                    float cube_y = y * -16;
                    float cube_z = z * 16 - 64;
                    
                    // Only draw if visible
                    if (!is_cube_visible(cube_x, cube_y, cube_z)) {
                        continue;
                    }
                    
                    GRRLIB_ObjectViewBegin();
                    GRRLIB_ObjectViewRotate(0, 0, 0);
                    GRRLIB_ObjectViewScale(16, 16, 16);
                    GRRLIB_ObjectViewTrans(cube_x, cube_y, cube_z);
                    GRRLIB_ObjectViewEnd();

                    nodes_drawn++;

                    if (nodes[node_number] == 1) {
                        GRRLIB_DrawCube(1, 1, GRRLIB_GREEN);
                    }
                    else if (nodes[node_number] == 2) {
                        GRRLIB_DrawCube(1, 1, 0x886400FF);
                    }
                }
            }
        }

        // UI
        GX_SetCullMode(GX_CULL_NONE);
        GRRLIB_2dMode();
        draw_wiimotes();

        GRRLIB_Printf(0, 50, tex_BMfont5, GRRLIB_WHITE, 1, "x %f y %f z %f", camera_x, camera_y, camera_z);
        GRRLIB_Printf(250, 225, tex_BMfont5, GRRLIB_WHITE, 1, "nodes_drawn %d", nodes_drawn);
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


bool is_cube_visible(float cube_x, float cube_y, float cube_z) {
    // Direction from camera to cube
    float dx = cube_x - camera_x;
    float dy = cube_y - camera_y;
    float dz = cube_z - camera_z;
    
    // Dot product with look direction
    float dot = dx * camera_look_x + dy * camera_look_y + dz * camera_look_z;
    
    return (dot > 1 && dot < 512);
}