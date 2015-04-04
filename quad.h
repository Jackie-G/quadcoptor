#ifndef _QUADCONTROL_H_
#define _QUADCONTROL_H_

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <signal.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <math.h>
#include <ctime>
#include <sys/select.h>
#include <stropts.h>
#include <sys/ioctl.h>

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyACM0"
#define _POSIX_SOURCE 1
#define FALSE 0
#define TRUE 1
#define uint8_t signed char

extern int selection;
extern char ASK_FOR_DISPLAY;
extern int fd,c,res;
extern struct termios oldtio,newtio;
extern struct sigaction saio;
extern FILE *file;


int config();
int shutdown();
void signal_handler_IO (int status);   /* definition of signal handler interrupt*/
int _kbhit();

void delay(int i);
double byte2Double(uint8_t* input);
void Double2byte(double DATA, char* output);
void CVT_double2send(const int num_of_w,const double LAT[],const double LON[],char send[]);
void CVT_send2double(int& num_of_w,double LAT[],double LON[],const char send[]);

double radians(double val);
double degrees(double val);
double distanceBetween(double lat1, double long1, double lat2, double long2);
double courseTo(double lat1, double long1, double lat2, double long2);


void mainmenu();
int menu0();
int menu1();
int menu2();
void menu3();
void menu4();
void menu5(int &ter_program);

#endif
