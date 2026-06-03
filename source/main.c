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
#define GRRLIB_BLACK  0x000000ff
#define GRRLIB_WHITE  0xFFFFFFFF
#define GRRLIB_RED    0xff0000ff
#define GRRLIB_GREEN  0x00ff00ff
#define GRRLIB_BLUE   0x0000ffff
#define GRRLIB_YELLOW 0xffff00ff

void player_input();
void draw_wiimotes();
bool is_node_visible(float node_x, float node_y, float node_z);
float clamp(float value, float min, float max);

bool project_running = true;

unsigned int frame = 0;

// camera setup
float player_x = 256.0f;
float player_y = 0.0f;
float player_z = 256.0f;

const int view_range = 4;
const float player_speed = 0.5f;
float velocity_y = 0.0f;

float camera_rot_x = 0.0f;
float camera_rot_y = 1.0f;

float camera_look_x;
float camera_look_y;
float camera_look_z;

// uint8_t nodes[MAP_SIZE];
uint8_t *nodes;


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

    nodes = malloc(MAP_SIZE * sizeof(uint8_t));

    if (nodes == NULL) {
        goto cleanup;
    }

    for (int i=0;i<MAP_SIZE;i++) {
        nodes[i] = 0;
    }

    unsigned int node_number = 0;
        
    for (int i=0;i<32;i++) {
        for (int j=0;j<32;j++) {
            int hight = rand() % 5 + 2;
            node_number += 8 - hight;

            // make grass
            nodes[node_number] = 1;
            node_number++;
            hight--;

            while (hight > 0) {
                nodes[node_number] = 2;
                node_number++;
                hight--;
            }
        }
    }

    if (node_number > MAP_SIZE) {
        return -1;
    }
	
    while (project_running) {
        frame++;

        player_input();
        velocity_y -= 0.03f;
        player_y += velocity_y;

        const int player_node_x = round(player_x / 16.0);
        const int player_node_y = round(player_y / 16.0);
        const int player_node_z = round(player_z / 16.0);

        node_number = (clamp(player_node_x,0,31) * 256) + (clamp(player_node_z,0,31) * 8) + clamp(-player_node_y,0,7);
        
        if (nodes[node_number] != 0) {
            velocity_y = 1.5f;
        }

        nodes[node_number] = 0;

        GRRLIB_Camera3dSettings(player_x, player_y, player_z, 
                        0, 1, 0,
                        player_x + camera_look_x, 
                        player_y + camera_look_y, 
                        player_z + camera_look_z
                        );

        // background
		GRRLIB_2dMode();
        GRRLIB_FillScreen(0x4578d1ff);

        GRRLIB_3dMode(0.1, 1000, 70, 0,1);
        GRRLIB_SetLightAmbient(0x111111FF);
        GRRLIB_SetLightDiff(0, (guVector){player_x, 64.0f, player_z}, 50.0f, 1.0f, 0xffffffff);

        GX_SetCullMode(GX_CULL_FRONT);

        int nodes_drawn = 0;

        // draw map
        for (int x=player_node_x - view_range;x<player_node_x + view_range;x++) {
            for (int z=player_node_z - view_range;z<player_node_z + view_range;z++) {
                for (int y=0;y<8;y++) {
                    
                    node_number = (clamp(x,0,31) * 256) + (clamp(z,0,31) * 8) + clamp(y,0,7);

                    if (node_number > MAP_SIZE) {
                        return -1;
                    }

                    if (nodes[node_number] == 0 || nodes_drawn >= 255) {
                        continue;
                    }

                    const float node_x = x * 16;
                    const float node_y = y * -16;
                    const float node_z = z * 16;   

                    // Only draw if visible
                    if (!is_node_visible(node_x, node_y, node_z)) {
                        continue;
                    }
                    
                    GRRLIB_ObjectViewBegin();
                    GRRLIB_ObjectViewRotate(0, 0, 0);
                    GRRLIB_ObjectViewScale(16, 16, 16);
                    GRRLIB_ObjectViewTrans(node_x, node_y, node_z);
                    GRRLIB_ObjectViewEnd();

                    nodes_drawn++;

                    if (nodes[node_number] == 1) {
                        GRRLIB_DrawCube(1, 1, 0x228e13ff);
                    }
                    else if (nodes[node_number] == 2) {
                        GRRLIB_DrawCube(1, 1, 0x352608ff);
                    }    
                }
            }
        }

        // node_number = (clamp(camera_look_x / 16.0,0,31) * 256) + (clamp(camera_look_x / 16.0,0,31) * 8) + (4);
        // nodes[node_number] = 0;


        // UI
        GX_SetCullMode(GX_CULL_NONE);
        GRRLIB_2dMode();
        draw_wiimotes();

        node_number = (clamp(player_node_x,0,31) * 256) + (clamp(player_node_z,0,31) * 8) + clamp(-player_node_y,0,7);

        GRRLIB_Printf(0, 0, tex_BMfont5, GRRLIB_WHITE, 1, "x %f y %f z %f", player_x, player_y, player_z);
        GRRLIB_Printf(0, 25, tex_BMfont5, GRRLIB_WHITE, 1, "Node x %d y %d z %d", player_node_x, player_node_y, player_node_z);
        GRRLIB_Printf(0, 50, tex_BMfont5, GRRLIB_WHITE, 1, "nodes_drawn %d", nodes_drawn);
        GRRLIB_Printf(0, 75, tex_BMfont5, GRRLIB_WHITE, 1, "debug %d", nodes[node_number]);
        GRRLIB_Render();
    }

    free(nodes);
    nodes = NULL;
    cleanup:
    GRRLIB_FreeTexture(tex_shot);
    GRRLIB_FreeTexture(tex_BMfont5);
    GRRLIB_Exit();
    return 0;
}


void player_input() {
    WPAD_ScanPads();

    if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
        project_running = false;
    }

    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_RIGHT) {
        camera_rot_x -= 0.03f;
    }
    
    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_LEFT) {
        camera_rot_x += 0.03f;
    }

    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) {
        camera_rot_y += 0.03f;
    }

    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) {
        camera_rot_y -= 0.03f;
    }

    camera_look_x = sin(camera_rot_x) * cos(camera_rot_y) * 3;
    camera_look_y = sin(camera_rot_y) * 3;
    camera_look_z = cos(camera_rot_x) * cos(camera_rot_y) * 3;

    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_A) {
        player_x += camera_look_x * player_speed;
        player_z += camera_look_z * player_speed;
    }
    
    if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_B) {
        player_x -= camera_look_x * player_speed;
        player_z -= camera_look_z * player_speed;
    }
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


bool is_node_visible(float node_x, float node_y, float node_z) {
    // Direction from camera to node
    float dx = node_x - player_x;
    float dy = node_y - player_y;
    float dz = node_z - player_z;
    
    // Dot product with look direction
    float dot = dx * camera_look_x + dy * camera_look_y + dz * camera_look_z;
    
    return (dot > 1 && dot < 512);
}


float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}