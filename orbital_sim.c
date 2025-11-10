#include <stdio.h>
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
const int WINDOW_SIZE_X         = 1000;
const int WINDOW_SIZE_Y         = 1000;
const int ORIGIN_X              = WINDOW_SIZE_X / 2;
const int ORIGIN_Y              = WINDOW_SIZE_Y / 2;
const double METERS_PER_PIXEL   = 100; // 1m in space will equal x number of pixels on screen

TTF_Font* g_font = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////
// UNIVERSAL VARIABLES
////////////////////////////////////////////////////////////////////////////////////////////////////
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
    g_font = TTF_OpenFont("CascadiaCode.ttf", 14);

    // set to false to stop the sim program
    bool sim_running = true;

    // initial properties of the bodies
    body1.mass = 50000000000000.0f;
    body2.mass = 500000000000.0f;
    body1.pos_y = 0.0f;
    body2.pos_y = -25000.0f;
    body1.vel_x = 0.0f;
    body2.vel_x = -0.3f;

    body1.radius = calculateVisualRadius(body1.mass);
    body2.radius = calculateVisualRadius(body2.mass);

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

        // calculate the forces that each body applies on the other
        calculateForces(&body1, body2); // finds the force body 2 applies on body 1
        calculateForces(&body2, body1); // finds the force body 1 applies on body 2

        // update the velocity and position of both bodies based on the calculated force
        updateMotion(&body1, TIME_STEP);
        updateMotion(&body2, TIME_STEP);

        // map position data to pixel data on the screen
        transformCoordinates(&body1);
        transformCoordinates(&body2);

        // set the renderer color and draw the bodies as circles to the renderer
        SDL_SetRenderDrawColor(renderer, 91, 161, 230, 255);
        SDL_RenderFillCircle(renderer, body1.pixel_coordinates_x, body1.pixel_coordinates_y, body1.radius);
        SDL_SetRenderDrawColor(renderer, 230, 166, 91, 255);
        SDL_RenderFillCircle(renderer, body2.pixel_coordinates_x, body2.pixel_coordinates_y, body2.radius);

        // draw scale reference bar
        drawScaleBar(renderer, METERS_PER_PIXEL, WINDOW_SIZE_X, WINDOW_SIZE_Y);

        // draw speed control box
        drawSpeedControl(renderer, &speed_control, TIME_STEP);

        // draw the stats info box
        drawStatsBox(renderer, body1, body2);

        // present the renderer to the screen
        SDL_RenderPresent(renderer);
    }

    // clean up
    if (g_font) TTF_CloseFont(g_font);
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}