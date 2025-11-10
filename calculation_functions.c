////////////////////////////////////////////////////////////////////////////////////////////////////
// SIMULATION CALCULATION FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////
#define SDL_MAIN_HANDLED
#include "calculation_functions.h"
// at the end of each sim loop, this function should be run to calculate the changes in
// the force values based on other parameters. for example, using F to find a based on m.
void calculateForces(body_properties_t *b, body_properties_t b2) {
    // calculate the distance between the two bodies
    double delta_pos_x = b2.pos_x - b->pos_x;
    double delta_pos_y = b2.pos_y - b->pos_y;
    double r = sqrt(delta_pos_x * delta_pos_x + delta_pos_y * delta_pos_y);

    // calculate the force that b2 applies on b due to gravitation (F = (GMm) / r)
    double total_force = (G * b->mass * b2.mass) / (r * r);
    b->force_x = total_force * (delta_pos_x / r);
    b->force_y = total_force * (delta_pos_y / r);

    // calculate the acceleration from the force on the object
    b->acc_x = b->force_x / b->mass;
    b->acc_y = b->force_y / b->mass;
}

// this calculates the changes of velocity and position based on the force values from before
void updateMotion(body_properties_t *b, double dt) {
    // update the velocity
    b->vel_x = b->vel_x + b->acc_x * dt;
    b->vel_y = b->vel_y + b->acc_y * dt;

    // update the position using the new velocity
    b->pos_x = b->pos_x + b->vel_x * dt;
    b->pos_y = b->pos_y + b->vel_y * dt;
}

// transforms spacial coordinates (for example, in meters) to pixel coordinates
void transformCoordinates(body_properties_t *b) {
    b->pixel_coordinates_x = ORIGIN_X + (int)(b->pos_x / METERS_PER_PIXEL);
    b->pixel_coordinates_y = ORIGIN_Y + (int)(b->pos_y / METERS_PER_PIXEL);
}

// draw a circle in SDL
void SDL_RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int x = radius;
    int y = 0;
    int radiusError = 1 - x;

    while (x >= y) {
        SDL_RenderLine(renderer, centerX - x, centerY + y, centerX + x, centerY + y);
        SDL_RenderLine(renderer, centerX - x, centerY - y, centerX + x, centerY - y);
        SDL_RenderLine(renderer, centerX - y, centerY + x, centerX + y, centerY + x);
        SDL_RenderLine(renderer, centerX - y, centerY - x, centerX + y, centerY - x);

        y++;
        if (radiusError < 0) {
            radiusError += 2 * y + 1;
        } else {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

// draw a distance scale bar on the sreen
void drawScaleBar(SDL_Renderer* renderer, double meters_per_pixel, int window_width, int window_height) {
    const int BAR_HEIGHT = 3;
    const int MARGIN = 20;
    const int BAR_WIDTH_PIXELS = 150;  // Fixed pixel width
    
    // calculate what distance this represents
    double distance_meters = BAR_WIDTH_PIXELS * meters_per_pixel;
    
    // round to nice value and format text
    double scale_value;
    char scale_text[50];
    
    if (distance_meters >= 1000000) {
        scale_value = distance_meters / 1000.0;
        if (scale_value >= 1000) {
            scale_value = round(scale_value / 1000.0) * 1000.0;
        } else if (scale_value >= 100) {
            scale_value = round(scale_value / 100.0) * 100.0;
        } else if (scale_value >= 10) {
            scale_value = round(scale_value / 10.0) * 10.0;
        } else {
            scale_value = round(scale_value);
        }
        sprintf(scale_text, "%.0f km", scale_value);
    } else {
        scale_value = distance_meters;
        if (scale_value >= 100) {
            scale_value = round(scale_value / 100.0) * 100.0;
        } else if (scale_value >= 10) {
            scale_value = round(scale_value / 10.0) * 10.0;
        } else {
            scale_value = round(scale_value);
        }
        sprintf(scale_text, "%.0f m", scale_value);
    }
    
    // draw the scale bar
    int bar_x = MARGIN;
    int bar_y = window_height - MARGIN - BAR_HEIGHT;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect bar_rect = {bar_x, bar_y, BAR_WIDTH_PIXELS, BAR_HEIGHT};
    SDL_RenderFillRect(renderer, &bar_rect);
    SDL_FRect left_cap = {bar_x, bar_y - 3, 2, BAR_HEIGHT + 6};
    SDL_FRect right_cap = {bar_x + BAR_WIDTH_PIXELS - 2, bar_y - 3, 2, BAR_HEIGHT + 6};
    SDL_RenderFillRect(renderer, &left_cap);
    SDL_RenderFillRect(renderer, &right_cap);
    
    // render the text
    if (g_font) {
        SDL_Color white = {255, 255, 255, 255};
        
        // SDL3 requires length parameter (0 for null-terminated strings)
        SDL_Surface* text_surface = TTF_RenderText_Blended(g_font, scale_text, 0, white);
        
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            
            if (text_texture) {
                float text_w = text_surface->w;
                float text_h = text_surface->h;
                SDL_FRect text_rect = {bar_x, bar_y - text_h - 5, text_w, text_h};
                SDL_RenderTexture(renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
            }
            
            SDL_DestroySurface(text_surface);
        }
    }
}

// calculates the size (in pixels) that the planet should appear on the screen based on its mass
int calculateVisualRadius(double mass) {
    const float scaling_coeff = 0.0351; // coefficient that came from real math I promise
    float radius_meters =  scaling_coeff * pow(mass, 1.0f/3.0f);
    return (int)(radius_meters / METERS_PER_PIXEL);
}



//////////////////////////////////////////////////////
// thing that calculates changing sim speed         //
//////////////////////////////////////////////////////
bool isMouseInRect(int mouse_x, int mouse_y, int rect_x, int rect_y, int rect_w, int rect_h) {
    return (mouse_x >= rect_x && mouse_x <= rect_x + rect_w &&
            mouse_y >= rect_y && mouse_y <= rect_y + rect_h);
}

void drawSpeedControl(SDL_Renderer* renderer, speed_control_t* control, double multiplier) {
    // background color that highlights if hovered
    if (control->is_hovered) {
        SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
    }
    
    SDL_FRect bg_rect = {
        (float)control->x, 
        (float)control->y, 
        (float)control->width, 
        (float)control->height
    };
    SDL_RenderFillRect(renderer, &bg_rect);
    
    // boarder
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &bg_rect);
    
    // text showing current speed
    if (g_font != NULL) {
        char speed_text[64];
        snprintf(speed_text, sizeof(speed_text), "Speed: %.2fx", multiplier);
        
        SDL_Color text_color = {255, 255, 255, 255};
        SDL_Surface* text_surface = TTF_RenderText_Solid(g_font, speed_text, 0, text_color);
        
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            
            SDL_FRect text_rect = {
                (float)(control->x + 10),
                (float)(control->y + 10),
                (float)text_surface->w,
                (float)text_surface->h
            };
            
            SDL_RenderTexture(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
            SDL_DestroySurface(text_surface);
        }
    }
}