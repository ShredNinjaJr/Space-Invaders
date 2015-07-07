#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H


#include "emulator.h"
#include <SDL.h>

uint8_t space_invaders_port_in(uint8_t port, State8080* state);
void space_invaders_port_out(uint8_t port, State8080* state);
void handle_input();
void draw_screen(State8080 * state);


#endif
