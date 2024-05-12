/*
 * vector.h
 *
 *  Created on: Dec 27, 2023
 *      Author: Thanh Duy
 */

#ifndef INC_VECTOR_H_
#define INC_VECTOR_H_

#include <stdint.h>
#include <stdlib.h>
enum state { goUp, goDown, goLeft, goRight };
typedef struct {
	enum state currentState;
	uint8_t isTail;
    int32_t length;
    int32_t x1;
    int32_t x2;
    int32_t y1;
    int32_t y2;

} path;

typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} obstacle;

#endif /* INC_VECTOR_H_ */
