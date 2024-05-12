/*
 * snake.c
 *
 *  Created on: Dec 12, 2023
 *      Author: QUI
 */

#include "snake.h"
#include <stdlib.h>
#include "led_7seg.h"
#include "vector.h"
#include "uart.h"

#include <string.h>

uint8_t id_game = 0;
uint16_t level = 0;
uint8_t currentIndex = 0;
path allPath[30];
const int16_t x_max = 239, y_max = 319, y_min = 100;
uint8_t flag = 0;
uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint16_t x1 = 160, y1 = 160, x2 = 170, y2 = 170;
enum state firstState = goDown;
enum state prevState = goRight;

uint16_t random_eat(uint16_t minN, uint16_t maxN) {
	return minN + rand()%(maxN + 1 - minN);
}

uint16_t x_food;
uint16_t y_food;
uint16_t init_len = 10;
uint16_t snakeTailLen = 10;
uint16_t speed = 2;

uint16_t count = 0;
uint16_t max_count = 0;
//uint16_t pre_count = 0;
uint16_t CREATE_FOOD = 1;

obstacle obstacles[9] = {
        {55, 140, 65, 200},    // Wall 1
        {55, 140, 90, 150},    // Wall 2
        {40, 250, 80, 260},
		{40, 250, 50, 280},// Wall 3
        {170, 140, 200, 150},    // Wall 4   // Wall 5
        {190, 220, 200, 285},    // Wall 6
        {130, 275, 200, 285},
		{90, 150, 140, 160},
		{115, 150, 125, 200}
    };
void createWall_color(int32_t X1, int32_t Y1, int32_t X2, int32_t Y2) {
    // Replace this with the appropriate function in your graphics library
    lcd_Fill(X1, Y1, X2, Y2, MAGENTA);
}
void createWall() {
	for (int8_t i = 0; i < 9; ++i) {
		createWall_color(obstacles[i].x1, obstacles[i].y1, obstacles[i].x2, obstacles[i].y2);
	}
}
void snake_init() {
	lcd_Fill(x1, y1, x2, y2, BLUE);
}

void reset_game() {
	count = 0;

	lcd_Clear(WHITE);
	x1 = 160, y1 = 190, x2 = 170, y2 = 200;
	CREATE_FOOD = 1;

	lcd_Fill(0, 0, 240, 100, BLACK);
	currentIndex = 0;
	for (int8_t i = 0; i < 30; i++) {
	        allPath[i].currentState = goDown;
	        allPath[i].isTail = 0;
	        allPath[i].length = 0;
	        allPath[i].x1 = 0;
	        allPath[i].x2 = 0;
	        allPath[i].y1 = 0;
	        allPath[i].y2 = 0;
	    }
	init_len = 10;
	snakeTailLen = 10;
	flag = 0;
	flag1 = 0;
	flag2 = 0;
	speed = 2;
	firstState = goDown;
	prevState = goRight;
	snake_init();
	if(level >= 1) {
		createWall();

	}
}
uint8_t flag_clear_screen = 1;
void game_over(){
	if(count > max_count) {
		max_count = count;
	}
	if(flag_clear_screen == 1) {
		uartSendPlay();
		lcd_Fill(0, 101,240 ,319, WHITE);
		flag_clear_screen = 0;
	}

	lcd_ShowStr(50,175,"Diem cua ban la: ",RED,WHITE,16,0);
	if(count < 10)
		lcd_ShowIntNum(185,175,count,1,RED,WHITE,16);
	else
		lcd_ShowIntNum(185,175,count,2,RED,WHITE,16);

//	lcd_Fill(x1, y1, x2, y2, WHITE);
//	lcd_Fill(x_food, y_food, x_food+5, y_food+5, WHITE);

	lcd_ShowStr(10,230,"Nhan phim 9 de choi lai!",BLACK,WHITE,16,0);
	lcd_ShowStr(10,250,"Nhan phim E de thoat game!",BLACK,WHITE,16,0);
	lcd_ShowStr(50,130,"Game Over!!!",BLUE,YELLOW,24,0);

//	reset_game();
	if(button_count[10] == 1) {
		uartSendPlay();
		reset_game();
		flag_clear_screen = 1;

	} else if (button_count[12] == 1) {
		reset_game();
		flag_clear_screen = 1;
		max_count = 0;
	}

}

uint8_t checkBite (int16_t X1 , int16_t Y1, int16_t X2, int16_t Y2,int8_t count) {
	for (int8_t i = count; i < currentIndex; i++) {
		if (X1 >= allPath[i].x1 && X1 <= allPath[i].x2) {
			if (Y1 >= allPath[i].y1 && Y1 <= allPath[i].y2) {

				return 1;
			} else if (Y2 >= allPath[i].y1 && Y2 <= allPath[i].y2) {

				return 1;
			}
		} else if (X2 >= allPath[i].x1 && X2 <= allPath[i].x2) {
			if (Y1 >= allPath[i].y1 && Y1 <= allPath[i].y2) {

				return 1;
			} else if (Y2 >= allPath[i].y1 && Y2 <= allPath[i].y2) {

				return 1;
			}
		}
	}
	return 0;
}
uint8_t snakeSelfBite (int16_t X1 , int16_t Y1, int16_t X2, int16_t Y2,int8_t flag3){
	if (flag3 == 1) {
		if(checkBite(X1, Y1, X2, Y2, 0) == 1)
			return 1;
		return 0;
	}
	else {
		if (currentIndex >= 4) {
			if(checkBite(X1, Y1, X2, Y2, 3) == 1) {
				return 1;
			}
		}
		return 0;
	}

}
void moveWall() {
	if (flag1 == 0) {
		lcd_Fill(obstacles[7].x2-2, obstacles[7].y1, obstacles[7].x2, obstacles[7].y2, WHITE);
		obstacles[7].x1-=2;
		obstacles[7].x2-=2;
		lcd_Fill(obstacles[7].x1, obstacles[7].y1, obstacles[7].x1+2, obstacles[7].y2, MAGENTA);

		snakeSelfBite(obstacles[7].x1,obstacles[7].y1,obstacles[7].x1+10,obstacles[7].y2, 1);
		if (obstacles[7].x1 <= 10) flag1 = 1;
	} else {
		lcd_Fill(obstacles[7].x1, obstacles[7].y1, obstacles[7].x1+2, obstacles[7].y2, WHITE);
		obstacles[7].x1+=2;
		obstacles[7].x2+=2;
		lcd_Fill(obstacles[7].x2-2, obstacles[7].y1, obstacles[7].x2, obstacles[7].y2, MAGENTA);
		snakeSelfBite(obstacles[7].x2-10,obstacles[7].y1,obstacles[7].x2,obstacles[7].y2, 1);
		if (obstacles[7].x2 >= 230) flag1 = 0;
	}

	if (flag2 == 0) {
		lcd_Fill(obstacles[8].x1, obstacles[8].y2-2, obstacles[8].x2, obstacles[8].y2, WHITE);
		obstacles[8].y1-=2;
		obstacles[8].y2-=2;
		lcd_Fill(obstacles[8].x1, obstacles[8].y1, obstacles[8].x2, obstacles[8].y1+2, MAGENTA);
		snakeSelfBite(obstacles[7].x1,obstacles[7].y1,obstacles[7].x2,obstacles[7].y1+10, 1);
		if (obstacles[8].y1 <= y_min) flag2 = 1;
	} else {
		lcd_Fill(obstacles[8].x1, obstacles[8].y1, obstacles[8].x2, obstacles[8].y1+2, WHITE);
		obstacles[8].y1+=2;
		obstacles[8].y2+=2;
		lcd_Fill(obstacles[8].x1, obstacles[8].y2-2, obstacles[8].x2, obstacles[8].y2, MAGENTA);
		snakeSelfBite(obstacles[7].x1,obstacles[7].y2-10,obstacles[7].x2,obstacles[7].y2, 1);
		if (obstacles[8].y2 >= 310) flag2 = 0;
	}

}

uint8_t checkCollision() {
	if(level == 0)
		return 0;
	for (int8_t i = 0; i < 9; ++i) {
		if (x1 >= obstacles[i].x1 && x1 <= obstacles[i].x2) {
			if (y1 >= obstacles[i].y1 && y1 <= obstacles[i].y2) {

				return 1;
			} else if (y2 >= obstacles[i].y1 && y2 <= obstacles[i].y2) {

				return 1;
			}
		} else if (x2 >= obstacles[i].x1 && x2 <= obstacles[i].x2) {
			if (y1 >= obstacles[i].y1 && y1 <= obstacles[i].y2) {

				return 1;
			} else if (y2 >= obstacles[i].y1 && y2 <= obstacles[i].y2) {

				return 1;
			}
		}
	}
	return 0;
}
void delete_path() {
    // Assuming lcd_Fill is a function to clear the specified area on the LCD with white color
    	if (allPath[0].isTail == 0) {
    		//if (allPath[0].currentState == goRight) lcd_Fill(x1,0, x2, y2, RED);
    		if (allPath[0].length >= snakeTailLen) {
    			if (allPath[0].currentState == goUp) {
					lcd_Fill(allPath[0].x1, allPath[0].y2-speed, allPath[0].x2, allPath[0].y2, WHITE);
					allPath[0].y2 -= speed;
				} else if (allPath[0].currentState == goDown) {
					lcd_Fill(allPath[0].x1, allPath[0].y1, allPath[0].x2, allPath[0].y1 + speed, WHITE);
					allPath[0].y1 += speed;
				} else if (allPath[0].currentState == goLeft) {
					lcd_Fill(allPath[0].x2-speed, allPath[0].y1, allPath[0].x2, allPath[0].y2, WHITE);
					allPath[0].x2 -= speed;
				} else if (allPath[0].currentState == goRight) {
					lcd_Fill(allPath[0].x1, allPath[0].y1, allPath[0].x1+speed, allPath[0].y2, WHITE);
					allPath[0].x1 += speed;
				}

    		}
    	} else if (allPath[0].isTail == 1) {
    		allPath[0].length -= speed;
    		if (allPath[0].currentState == goUp) {
				lcd_Fill(allPath[0].x1, allPath[0].y2-speed, allPath[0].x2, allPath[0].y2, WHITE);
				allPath[0].y2 -= speed;
			} else if (allPath[0].currentState == goDown) {
				lcd_Fill(allPath[0].x1, allPath[0].y1, allPath[0].x2, allPath[0].y1 + speed, WHITE);
				allPath[0].y1 += speed;
			} else if (allPath[0].currentState == goLeft) {
				lcd_Fill(allPath[0].x2-speed, allPath[0].y1, allPath[0].x2, allPath[0].y2, WHITE);
				allPath[0].x2 -= speed;
			} else if (allPath[0].currentState == goRight) {
				lcd_Fill(allPath[0].x1, allPath[0].y1, allPath[0].x1+speed, allPath[0].y2, WHITE);
				allPath[0].x1 += speed;
			}
    		if (flag == 0) {
				if (allPath[0].length == 0 || allPath[0].length < 0) {
						flag = 1;
						delete_path();
						flag = 0;
					for (uint8_t i = 0; i < currentIndex - 1; i++) {
						allPath[i] = allPath[i + 1];
					}
					currentIndex--;
				}
    		}



    	}

}
void right() {

	if (prevState == goRight) {
				if (allPath[currentIndex-1].length <= snakeTailLen) {
					allPath[currentIndex-1].length +=speed;
				}
				allPath[currentIndex-1].x2 = x2;
			} else {
				if (currentIndex == 0) allPath[0].isTail = 0;
				else allPath[currentIndex-1].isTail = 1;
				allPath[currentIndex].isTail = 0;
				if (snakeTailLen == init_len) allPath[currentIndex].length = snakeTailLen;
				else allPath[currentIndex].length = 0;
				allPath[currentIndex].currentState = goRight;
				allPath[currentIndex].x1 = x1;
				allPath[currentIndex].x2 = x2;
				allPath[currentIndex].y1 = y1;
				allPath[currentIndex].y2 = y2;

				currentIndex++;
			}
			prevState = goRight;
			//if (snakeTailLen - allPath[currentIndex-1].length == init_len) allPath[currentIndex-1].length+=speed;
			x1 += speed;
			x2 += speed;


			lcd_Fill(x1, y1, x2, y2, BLUE);
			delete_path();
}
void left() {
	if (prevState == goLeft) {
		if (allPath[currentIndex-1].length <= snakeTailLen) {

							allPath[currentIndex-1].length +=speed;
						}

			allPath[currentIndex-1].x1 = x1;

		} else {
			if (currentIndex == 0) allPath[0].isTail = 0;
			else allPath[currentIndex-1].isTail = 1;
			allPath[currentIndex].isTail = 0;
			if (snakeTailLen == init_len) allPath[currentIndex].length = snakeTailLen;
			else allPath[currentIndex].length = 0;
			allPath[currentIndex].currentState = goLeft;
			allPath[currentIndex].x1 = x1;
			allPath[currentIndex].x2 = x2;
			allPath[currentIndex].y1 = y1;
			allPath[currentIndex].y2 = y2;
			currentIndex++;
		}
		prevState = goLeft;
		//if (snakeTailLen - allPath[currentIndex-1].length == init_len) allPath[currentIndex-1].length+=speed;
		x1 -= speed;
		x2 -= speed;


		lcd_Fill(x1, y1, x2, y2, BLUE);
		delete_path();

}


void up() {

	if (prevState == goUp) {
		if (allPath[currentIndex-1].length <= snakeTailLen) {

					allPath[currentIndex-1].length += speed;
				}

		allPath[currentIndex-1].y1 -= y1 ;


	} else {
		if (currentIndex == 0) allPath[0].isTail = 0;
		else allPath[currentIndex-1].isTail = 1;
		allPath[currentIndex].isTail = 0;
		if (snakeTailLen == init_len) allPath[currentIndex].length = snakeTailLen;
		else allPath[currentIndex].length = 0;
		allPath[currentIndex].currentState = goUp;
		allPath[currentIndex].x1 = x1;
		allPath[currentIndex].x2 = x2;
		allPath[currentIndex].y1 = y1;
		allPath[currentIndex].y2 = y2;
		currentIndex++;
	}
	prevState = goUp;
	//if (snakeTailLen - allPath[currentIndex-1].length == init_len) allPath[currentIndex-1].length+=speed;

	y1 -= speed;
	y2 -= speed;


	lcd_Fill(x1, y1, x2, y2, BLUE);
	delete_path();
}


void down() {

	if (prevState == goDown) {
		if (allPath[currentIndex-1].length <= snakeTailLen) {

			allPath[currentIndex-1].length += speed;
		}

		allPath[currentIndex-1].y2 = y2;
	} else {
		if (currentIndex == 0) allPath[0].isTail = 0;
		else allPath[currentIndex-1].isTail = 1;
		allPath[currentIndex].isTail = 0;
		if (snakeTailLen == init_len) allPath[currentIndex].length = snakeTailLen;
		else allPath[currentIndex].length = 0;
		allPath[currentIndex].currentState = goDown;
		allPath[currentIndex].x1 = x1;
		allPath[currentIndex].x2 = x2;
		allPath[currentIndex].y1 = y1;
		allPath[currentIndex].y2 = y2;
		currentIndex++;
	}
	prevState = goDown;
	//if (snakeTailLen - allPath[currentIndex-1].length == init_len) allPath[currentIndex-1].length+=speed;
		y1 += speed;
		y2 += speed;

		lcd_Fill(x1, y1, x2, y2, BLUE);
		delete_path();
}

void eat_food_success() {

	CREATE_FOOD = 1; //QUAY LAI HÀM TẠO FRUIT
	count++; //tăng số điểm
	uartSendPlay();
	snakeTailLen += 10;
	lcd_Fill(x_food, y_food, x_food+5, y_food+5, WHITE);
}

void food() {
	if(CREATE_FOOD == 1) { // Tạo mồi nhậu
		while(1) {
			x_food = random_eat(0,x_max-5);
			y_food = random_eat(100,y_max-5);
			if(checkBite(X1, Y1, X2, Y2, count))
			// to do
			// check dính tường chỗ này
		}


		lcd_Fill(x_food, y_food, x_food+5, y_food+5, RED);
		CREATE_FOOD = 0; // tránh tạo mồi liên tục

	}
	else {
		if((x2 == x_food) && (
				(y1 >= y_food && y1 <= y_food+5)
			||	(y2 >= y_food && y2 <= y_food+5)
			||  (y1 <= y_food && y2 >= y_food+5)
			)
			) {
			eat_food_success();
		}
		if((x1 == x_food+5) && (
				(y1 >= y_food && y1 <= y_food+5)
			||	(y2 >= y_food && y2 <= y_food+5)
			||  (y1 <= y_food && y2 >= y_food+5)
			)
			) {
			eat_food_success();
		}
		if((y2 == y_food) && (
				(x1 >= x_food && x1 <= x_food+5)
			||	(x2 >= x_food && x2 <= x_food+5)
			||  (x1 <= x_food && x2 >= x_food+5)
			)
			) {
			eat_food_success();
		}
		if((y1 == y_food+5) && (
				(x1 >= x_food && x1 <= x_food+5)
			||	(x2 >= x_food && x2 <= x_food+5)
			||  (x1 <= x_food && x2 >= x_food+5)
			)
			) {
			eat_food_success();
		}
	}
}


void move() {
	if(x1 <= 2 || x1 >= x_max || x2 <= 2 || x2 >= x_max
	|| y1 <= y_min || y1 >= y_max || y2 <= y_min || y2 >= y_max - 2) { //đụng tường
			game_over();
			return;
	}
	led7_SetDigit(0, 0, 0);
	led7_SetDigit(0, 1, 0);
	led7_SetDigit(count/10, 2, 0);
	led7_SetDigit(count%10, 3, 0); //hiển thị điểm số
//	count++;

	food();
	if (snakeSelfBite(x1,y1,x2,y2,0)) {
		game_over();
		return;
	}
	if (level >= 1) {
		if (checkCollision()) {
			game_over();
			return;
		}
	}
	if(level == 2) {
		moveWall();

	}
		 // mode 3
	 //mode 2 va 3
	//snake move with button
	if (button_count[6] == 1) {
		if (firstState != goDown) {
			prevState = firstState;
			firstState = goUp;
		}
	}
	else if (button_count[14] == 1) {
		if (firstState != goUp) {
			prevState = firstState;
			firstState = goDown;
		}
	}
	else if (button_count[11] == 1) {
		if (firstState != goLeft) {
			prevState = firstState;
			firstState = goRight;
		}
	}
	else if (button_count[9] == 1) {
		if (firstState != goRight) {
			prevState = firstState;
			firstState = goLeft;
		}
	}
	switch (firstState) {
		case goUp:
			up();
			break;
		case goDown:
			down();
			break;
		case goRight:
			right();
			break;
		default:
			left();
			break;
	}
}


void uartSendPlay()
{

	char str1[100] = "GAME_PLAY#ID: ";
	char *str2[50];
	snprintf(str2, sizeof(str2), "%d", id_game);
	strcat(str1, str2);

	char str3[20] = "\nScore: ";
	char *str4[50];
	snprintf(str4, sizeof(str4), "%d", count);
	strcat(str3, str4);

	char str5[20] = "\nMax Score: ";
	char *str6[50];
	snprintf(str6, sizeof(str6), "%d", max_count);
	strcat(str5, str6);

	char str7[20] = "\nDo kho : ";
	char *str8;
	switch (level) {
		case 0:
			str8 = "Easy";
			break;
		case 1:
			str8 = "Normal";
			break;
		case 2:
			str8 = "Hard";
			break;
	}
	strcat(str7, str8);

	strcat(str1, str3);
	strcat(str1, str5);
	strcat(str1, str7);
	strcat(str1, "#");
	uart_EspSendBytes(str1, strlen(str1));
}


void wall(uint8_t id, uint16_t difficult) {
	level = difficult;
	id_game = id;

	lcd_ShowStr(10,10,"ID nguoi choi: ",WHITE,BLACK,16,0);
	lcd_ShowIntNum(125,10,id,1,WHITE,BLACK,16);

//	hiện thị score hiện tại
	lcd_ShowStr(10,30,"Diem so cua ban la: ",WHITE,BLACK,16,0);
	if(count < 10)
		lcd_ShowIntNum(164,30,count,1,WHITE,BLACK,16);
	else
		lcd_ShowIntNum(164,30,count,2,WHITE,BLACK,16);


	// hiển thị max score
	lcd_ShowStr(10,50,"Diem so cao nhat la: ",WHITE,BLACK,16,0);
	if(max_count < 10)
		lcd_ShowIntNum(173,50,max_count,1,WHITE,BLACK,16);
	else
		lcd_ShowIntNum(173,50,max_count,2,WHITE,BLACK,16);


	lcd_ShowStr(10,70,"Do kho: ",WHITE,BLACK,16,0);
	lcd_ShowIntNum(70,70,difficult,1,WHITE,BLACK,16);
	switch (difficult) {
	case 0:
		lcd_ShowStr(70,70,"Easy",WHITE,BLACK,16,0);
		break;
	case 1:
		lcd_ShowStr(70,70,"Normal",WHITE,BLACK,16,0);
		break;
	case 2:
		lcd_ShowStr(70,70,"Hard",WHITE,BLACK,16,0);
		break;
	}

	lcd_DrawRectangle(0, 100, x_max, y_max, RED); //TẠO TƯỜNG
}
