#include "interface.h"

#ifndef __AVR__
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
void select_display_line(uint8_t line){
	if(line==0){ for(int i=0; i<100; i++) printf("\n"); }
	else{ printf("\n"); }
}
void put_character(uint8_t c){
	printf("%c", c); fflush(stdout);
}
uint8_t poll_user_input(){
	while(1){
		struct pollfd fds;
		fds.fd=STDIN_FILENO;
		fds.events=POLLIN;
		while(poll(&fds, 1, 0)){
			char c[1];
			read(STDIN_FILENO, c, 1);
			switch(c[0]){
			case 'a': return A_LEFT;
			case 'd': return A_RIGHT;
			case 's': return A_PRESS;
			case 'q': return B_LEFT;
			case 'e': return B_RIGHT;
			case 'w': return B_PRESS;
			};
		}
		return 0;
	}
}
void start_timer(timer* t){
	gettimeofday(t, NULL);
}
uint16_t elapsed_time(timer* t){
	timer t2;
	gettimeofday(&t2, NULL);
	return (t2.tv_sec-t->tv_sec)*1000+(t2.tv_usec-t->tv_usec)/1000;
}
int usleep (__useconds_t __useconds);
void small_delay(){
	usleep(100*1000);
}
#else
#include <avr/io.h>
#include <util/delay.h>
// Stubs for now.
void select_display_line(uint8_t line){
	_delay_ms(10);
	DDRB=line;
	_delay_ms(10);
}
void put_character(uint8_t c){
	_delay_ms(10);
	DDRB=c;
	_delay_ms(10);
}
uint8_t poll_user_input(){
	_delay_ms(10);
	return DDRB;
}
void start_timer(timer* t){
	_delay_ms(10);
	t->x=DDRB;
	_delay_ms(10);
}
uint16_t elapsed_time(timer* t){
	_delay_ms(10);
	return t->x+DDRB;
	_delay_ms(10);
}
void small_delay(){
	_delay_ms(10);
}
#endif
