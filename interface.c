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
// Stubs for now.
void select_display_line(uint8_t line){
	DDRB=line;
}
void put_character(uint8_t c){
	DDRB=c;
}
uint8_t poll_user_input(){
	return DDRB;
}
void start_timer(timer* t){
	t->x=DDRB;
}
uint16_t elapsed_time(timer* t){
	return t->x+DDRB;
}
void small_delay(){
}
#endif
