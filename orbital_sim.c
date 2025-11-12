#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "calculation_functions.h"

// NOTE: ALL CALCULATIONS SHOULD BE DONE IN BASE SI UNITS

////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND GLOBAL VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////
///////////// simulation constants:
// universal gravitation constant
const double G = 6.67430E-11;

double TIME_STEP                = 1; // amount of time in seconds each step in the simulation should be
                                        // i.e. each new updated position shown is after x seconds
                                        // this value can be changed to adjust the speed/accuracy of the simulation
speed_control_t speed_control = {10, 10, 150, 40, false};
double speed_multiplier = 1.0; // multiplier for TIME_STEP

// SDL window sizing numbers
const int WINDOW_SIZE_X         = 1500;
const int WINDOW_SIZE_Y         = 1500;
const int ORIGIN_X              = WINDOW_SIZE_X / 2;
const int ORIGIN_Y              = WINDOW_SIZE_Y / 2;
const double METERS_PER_PIXEL   = 100; // 1m in space will equal x number of pixels on screen
const int FONT_SIZE             = WINDOW_SIZE_Y / (WINDOW_SIZE_X * 0.05);

TTF_Font* g_font = NULL;

// holds the information on the bodies
int num_bodies = 2;
body_properties_t *global_bodies = NULL;

body_properties_t body1 = {0};
body_properties_t body2 = {0};

////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    // initialize SDL3
    SDL_Init(SDL_INIT_VIDEO);
    // create an SDL window
    SDL_Window* window = SDL_CreateWindow("Orbit Simulation", WINDOW_SIZE_X, WINDOW_SIZE_Y, SDL_WINDOW_RESIZABLE);
    // create an SDL renderer and clear the window to create a blank canvas
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // SDL ttf font stuff
    TTF_Init();
    g_font = TTF_OpenFont("CascadiaCode.ttf", FONT_SIZE);

    // set to false to stop the sim program
    bool sim_running = true;

    global_bodies = (body_properties_t *)malloc(num_bodies * sizeof(body_properties_t));

    // initial properties of the bodies
    global_bodies[0].mass = 50000000000000.0f;
    global_bodies[0].pos_y = 0.0f;
    global_bodies[0].vel_x = 0.0f;
    global_bodies[0].radius = calculateVisualRadius(global_bodies[0].mass);

    global_bodies[1].mass = 500000000000.0f;
    global_bodies[1].pos_y = -25000.0f;
    global_bodies[1].vel_x = -0.3f;
    global_bodies[1].radius = calculateVisualRadius(global_bodies[1].mass);

    ////////////////////////////////////////////////////////
    // simulation loop                                    //
    ////////////////////////////////////////////////////////

    while (sim_running) {
        // checks inputs into the window
        SDL_Event event;
        runEventCheck(&event, &sim_running, &speed_control, &TIME_STEP);

        // clears previous frame from the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // color for drawing bodies
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        ////////////////////////////////////////////////////
        // START OF SIMULATION LOGIC                      //
        ////////////////////////////////////////////////////

        // calculate forces between all body pairs
        for (int i = 0; i < num_bodies; i++) {
            for (int j = 0; j < num_bodies; j++) {
                if (i != j) {
                    calculateForces(&global_bodies[i], global_bodies[j]);
                }
            }
        }
        
        // update the motion for each body and draw
        for (int i = 0; i < num_bodies; i++) {
            updateMotion(&global_bodies[i], TIME_STEP);
            transformCoordinates(&global_bodies[i]);
            SDL_RenderFillCircle(renderer, global_bodies[i].pixel_coordinates_x,
                            global_bodies[i].pixel_coordinates_y, 
                            global_bodies[i].radius);
    }

        // draw scale reference bar
        drawScaleBar(renderer, METERS_PER_PIXEL, WINDOW_SIZE_X, WINDOW_SIZE_Y);

        // draw speed control box
        drawSpeedControl(renderer, &speed_control, TIME_STEP);

        // draw the stats info box
        drawStatsBox(renderer, global_bodies[0], global_bodies[1]);

        // present the renderer to the screen
        SDL_RenderPresent(renderer);
    }

    // clean up
    free(global_bodies);
    global_bodies = NULL;
    num_bodies = 0;
    if (g_font) TTF_CloseFont(g_font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}