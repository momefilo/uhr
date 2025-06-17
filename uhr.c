// uhr Bibliothek
#include "uhr.h"

uint16_t Center_x, Center_y, MinZeiger_len, StdZeiger_len;
int16_t **StdVektor, **MinVektor;
uint8_t I2c_bus, Sda, Scl;
char Tage[7][3] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
char Monate[12][4] = {"Jan", "Feb", "Mrz", "Apr", "Mai", "Jun", "jul", "Aug", "Sep", "Okt", "Nov", "Dez"};
struct repeating_timer Timer;
bool Timer_set = false;

int16_t getStdVektor(uint8_t std, uint8_t min){
	// Stundenzeiger
	if(std > 12) std -= 12;
	int8_t fak = 1;
	if(std > 5){ std -= 6; fak = -1;}
	double_t grad = ((std * STD_GRAD) + ((min * MIN_GRAD) / 12.f));
	double_t sinus = sin(grad  * (2 * 3.14) / 360);
	double_t cosin = cos(grad  * (2 * 3.14) / 360);
	int cnt = 0;
	for(int i=0; i<StdZeiger_len; i++){
		if(grad == 90){
			StdVektor[i][0] = fak * i;
			StdVektor[i][1] = 0;
			cnt++;
		}
		else if(grad == 0){
			StdVektor[i][0] = 0;
			StdVektor[i][1] = fak * -i;
			cnt++;
		}
		else{
			double_t cos_ = -i * cosin;
			double_t sin_ = i * sinus;
			int16_t icos = cos_*fak, isin = sin_*fak;
			if(i<1){
				StdVektor[cnt][0] = isin;
				StdVektor[cnt][1] = icos;
				cnt++;
			}
			else if(i>1 && ((StdVektor[cnt-1][0] != isin) || (StdVektor[cnt-1][1] != icos))){
				StdVektor[cnt][0] = isin;
				StdVektor[cnt][1] = icos;
				cnt++;
			}
		}
	}
	return cnt;
}
int16_t getMinVektor(uint8_t min){
	// Minutenzeiger
	int8_t fak = 1; if(min > 29){ min -= 30; fak = -1;}
	double_t sinus = sin((min * MIN_GRAD) * (2 * 3.14) / 360);
	double_t cosin = cos((min * MIN_GRAD) * (2 * 3.14) / 360);
	int cnt = 0;
	for(int i=0; i<MinZeiger_len; i++){
		if(min == 15){
			MinVektor[i][0] = fak * i;
			MinVektor[i][1] = 0;
			cnt++;
		}
		else if(min == 0){
			MinVektor[i][0] = 0;
			MinVektor[i][1] = fak * -i;
			cnt++;
		}
		else{
			double_t cos_ = -i * cosin;
			double_t sin_ = i * sinus;
			int16_t icos = cos_*fak, isin = sin_*fak;
			if(i<1){
				MinVektor[cnt][0] = isin;
				MinVektor[cnt][1] = icos;
				cnt++;
			}
			else if(i>1 && ((MinVektor[cnt-1][0] != isin) || (MinVektor[cnt-1][1] != icos))){
				MinVektor[cnt][0] = isin;
				MinVektor[cnt][1] = icos;
				cnt++;
			}
		}
	}
	return cnt;
}

void paintBlatt(){
	uint16_t sx = Center_x - MinZeiger_len - Abstand;
	uint16_t ex = Center_x + MinZeiger_len + Abstand;
	uint16_t sy = Center_y - MinZeiger_len - Abstand;
	uint16_t ey = Center_y + MinZeiger_len + Abstand;
	uint16_t area[] = {sx, sy, ex, ey};
	paintRect(area, ClrColor);
	for(int i=0; i<60; i += 5){
		uint16_t len = getMinVektor(i);
		for(int p=Abstand; p<Abstand + ZifferLen; p++){
			uint16_t startx = MinVektor[len-1][0] + MinVektor[p][0] + Center_x;
			uint16_t starty = MinVektor[len-1][1] + MinVektor[p][1] + Center_y;
			uint16_t endy = starty;
			uint16_t endx = startx;
			if((i > 7 && i < 23) || (i > 37 && i < 53)){ endy+=ZifferWidth; starty-=ZifferWidth; }
			else if((i > 22 && i < 38) || (i > 52 || i < 8)){ endx+=ZifferWidth; startx-=ZifferWidth; }
			uint16_t area[] = {startx, starty, endx, endy};
			paintRect(area, 0xFFEF);
		}
	}
}
void paintZeiger(uint8_t std, uint8_t min){
	paintBlatt();
	// Minuten
	uint16_t len = getMinVektor(min);
	uint8_t mid  = (MinZeiger_width - 1) / 2;
	if((min > 7 && min < 23) || (min > 37 && min < 53)){// Pixel von Unten nach Oben um das Pixel auffüllen
		for(int i=0; i<len; i++){
			uint16_t startx = MinVektor[i][0] + Center_x;
			uint16_t starty = MinVektor[i][1] + Center_y - MinZeiger_width + mid;
			uint16_t endy = MinVektor[i][1] + Center_y + MinZeiger_width - mid;
			uint16_t area[] = { startx, starty, startx, endy};
			paintRect(area, MinColor);
		}
	}
	else if((min > 22 && min < 38) || (min > 52 || min < 8)){// Pixel von Links nach Rechts um das Pixel auffüllen
		for(int i=0; i<len; i++){
			uint16_t starty = MinVektor[i][1] + Center_y;
			uint16_t startx = MinVektor[i][0] + Center_x - MinZeiger_width + mid;
			uint16_t endx = MinVektor[i][0] + Center_x + MinZeiger_width - mid;
			uint16_t area[] = { startx, starty, endx, starty};
			paintRect(area, MinColor);
		}
	}
	// Stunden
	len = getStdVektor(std, min);
	mid  = (StdZeiger_width - 1) / 2;
	if(std > 11) std -= 12;
	if((std > 1 && std < 5) || (std > 7 && std < 11)){// Pixel von Unten nach Oben um das Pixel auffüllen
		for(int i=0; i<len; i++){
			uint16_t startx = StdVektor[i][0] + Center_x;
			uint16_t starty = StdVektor[i][1] + Center_y - StdZeiger_width + mid;
			uint16_t endy = StdVektor[i][1] + Center_y + StdZeiger_width - mid;
			uint16_t area[] = { startx, starty, startx, endy};
			paintRect(area, StdColor);
		}
	}
	else if((std > 4 && std < 8) || (std > 10 || std < 2)){// Pixel von Links nach Rechts um das Pixel auffüllen
		for(int i=0; i<len; i++){
			uint16_t starty = StdVektor[i][1] + Center_y;
			uint16_t startx = StdVektor[i][0] + Center_x - StdZeiger_width + mid;
			uint16_t endx = StdVektor[i][0] + Center_x + StdZeiger_width - mid;
			uint16_t area[] = { startx, starty, endx, starty};
			paintRect(area, StdColor);
		}
	}
}

bool timer_callback(__unused struct repeating_timer *t) {
    datetime_t now;
    rtc_get_datetime(&now);
    paintZeiger(now.hour, now.min);
    return true;
}
uint8_t bcdToDec(uint8_t n){ return n - 6 * (n >> 4); }

void ds3231_init(){
	i2c_inst_t  *bus = i2c_get_instance(I2c_bus);
	i2c_init(bus, 400 * 1000);
	gpio_set_function(Sda, GPIO_FUNC_I2C);
    gpio_set_function(Scl, GPIO_FUNC_I2C);
    gpio_pull_up(Sda);
    gpio_pull_up(Scl);
    uint8_t reg[] = {0, 0, 0, 0, 0, 0, 0, 0};
    rtc_init();
    i2c_write_blocking(bus, Adr, reg, 1, true);
    i2c_read_blocking(bus, Adr, reg+1, 7, true);
    datetime_t t = {
            .year  = bcdToDec(reg[7]) + 2000,
            .month = bcdToDec(reg[6]),
            .day   = bcdToDec(reg[5]),
            .dotw  = bcdToDec(reg[4]), // 0 is Sunday, so 5 is Friday
            .hour  = bcdToDec(reg[3]),
            .min   = bcdToDec(reg[2]),
            .sec   = bcdToDec(reg[1])
    };
    rtc_set_datetime(&t);
    paintZeiger(t.hour, t.min);
}
void allocMemory(){
	MinVektor = (int16_t **)malloc((MinZeiger_len + Abstand) * sizeof(int16_t*));
	for(int i=0; i<(MinZeiger_len + Abstand); i++) MinVektor[i] = (int16_t *)malloc(2 * sizeof(int16_t));
	StdVektor = (int16_t **)malloc((StdZeiger_len + Abstand) * sizeof(int16_t*));
	for(int i=0; i<(StdZeiger_len + Abstand); i++) StdVektor[i] = (int16_t *)malloc(2 * sizeof(int16_t));
}
void uhr_init(uint16_t x, uint16_t y, uint16_t width, uint8_t bus, uint8_t sda, uint8_t scl){
	I2c_bus = bus;
	Sda = sda;
	Scl = scl;
	Center_x = x + width / 2;
	Center_y = y + width / 2;
	MinZeiger_len = width / 2 - Abstand - ZifferLen;
	StdZeiger_len = MinZeiger_len - ZeigerDiff;
	allocMemory();
	ds3231_init();
}

int main() {
	stdio_init_all();
	busy_wait_ms(50);
	buttons_init();
	ili9341_init();
	setOrientation(VERTICAL);
	uint16_t pos [] = { 0, 0};
	writeText7x11(pos, "Test", false, false);
	uhr_init(0, 40, 240, 1, 2, 3);
	uint8_t std = 13, min = 14;
	datetime_t now;
	while (true) {
		if(!Timer_set){
			rtc_get_datetime(&now);
			if(now.sec < 1){
				add_repeating_timer_ms(-60000, timer_callback, NULL, &Timer);
				paintZeiger(now.hour, now.min);
				Timer_set = true;
			}
		}
		uint8_t button = get_Button();
		if(button < 100){
			if(button == BUTTON_U){ min++; if(min > 59){ min = 0; std++;} }
			else if(button == BUTTON_D){ if(min > 0)min--; else  min = 59; }
			else if(button == BUTTON_L){ std++; if(std > 11) std = 0; }
			else if(button == BUTTON_R){ if(std > 0)std--; else  std = 11; }
			paintZeiger(std, min);
		}
		busy_wait_ms(10);
	}
}

