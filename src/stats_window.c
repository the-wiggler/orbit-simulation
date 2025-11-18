#include "stats_window.h"
#include "config.h"
#include "sdl_elements.h"
#include <stdio.h>

extern SDL_Color TEXT_COLOR;

void renderGraph(SDL_Renderer* renderer, body_properties_t* bodies, int num_bodies, window_params_t wp) {

}

// the stats box that shows stats yay
void renderStatsBox(SDL_Renderer* renderer, body_properties_t* bodies, int num_bodies, window_params_t wp) {
    int margin_x = wp.window_size_x * 0.02;
    int start_y = wp.window_size_y * 0.07;
    int line_height = wp.window_size_y * 0.02;

    // all calculations for things to go inside the box:
    if (bodies != NULL) {
        for (int i = 0; i < num_bodies; i++) {
            char vel_text[32];
            snprintf(vel_text, sizeof(vel_text), "Vel of %s: %.1f", bodies[i].name, bodies[i].vel);
            // render text
            SDL_WriteText(renderer, g_font, vel_text, margin_x, start_y + i * line_height, TEXT_COLOR);
        }
        renderGraph(renderer, bodies, num_bodies, wp);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// SDL WINDOW STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize the stats window
bool statsWindowInit(stats_window_t* stats) {
    if (!stats) return false;

    stats->window = SDL_CreateWindow("Stats Window", 400, 300, SDL_WINDOW_RESIZABLE);
    if (!stats->window) {
        SDL_Log("Failed to create stats window: %s", SDL_GetError());
        return false;
    }

    stats->renderer = SDL_CreateRenderer(stats->window, NULL);
    if (!stats->renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(stats->window);
        return false;
    }

    stats->window_ID = SDL_GetWindowID(stats->window);
    stats->is_shown = true;
    return true;
}

// handle events for the stats window
void StatsWindow_handleEvent(stats_window_t* stats, SDL_Event* e) {
    // check if event belongs to this window
    if (e->type >= SDL_EVENT_WINDOW_FIRST && 
        e->type <= SDL_EVENT_WINDOW_LAST &&
        e->window.windowID == stats->window_ID) {
        
        if (e->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            SDL_HideWindow(stats->window);
            stats->is_shown = false;
        }
    }
}

// show the window
void StatsWindow_show(stats_window_t* stats) {
    SDL_ShowWindow(stats->window);
    stats->is_shown = true;
}

// cleanup
void StatsWindow_destroy(stats_window_t* stats) {
    if (stats->renderer) {
        SDL_DestroyRenderer(stats->renderer);
    }
    if (stats->window) {
        SDL_DestroyWindow(stats->window);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN RENDER LOGIC
////////////////////////////////////////////////////////////////////////////////////////////////////
// render the stats window with data from main window
void StatsWindow_render(stats_window_t* stats, int fps, float posX, float posY, body_properties_t* gb, int num_bodies, window_params_t wp) {
    if (!stats->is_shown) return;
    
    SDL_SetRenderDrawColor(stats->renderer, 30, 30, 30, 255);
    SDL_RenderClear(stats->renderer);

    // draw the stats
    renderStatsBox(stats->renderer, gb, num_bodies, wp);
    
    SDL_RenderPresent(stats->renderer);
}