/*
 * snake.h
 *
 *  Created on: Dec 12, 2023
 *      Author: QUI
 */

#ifndef INC_SNAKE_H_
#define INC_SNAKE_H_

#include "lcd.h"
#include "button.h"

void snake_init();
void move();
void wall(uint8_t id, uint16_t difficult);
void start_game();
void uartSendPlay();
void createWall();

#endif /* INC_SNAKE_H_ */
