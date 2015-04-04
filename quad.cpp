#include "quad.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

const double pi = 3.1415926536;

int selection = 0;
int num_of_waypoint = 0;
int cur_of_waypoint = -1;
double LAT[5] = {0,0,0,0,0};
double LON[5] = {0,0,0,0,0};

char ARDUINO_SEND_WAYPOINT_TO_COMPUTER = 'm';
char INFORM_THAT_COMPUTER_WILL_SEND_TO_ARDUINO = 'r';
char ASK_FOR_DISPLAY = 'g';

volatile int STOP=FALSE;
volatile int wait_flag = TRUE;
int fd,c,res;
struct termios oldtio,newtio;
struct sigaction saio; //it is better to be use interrupt(Asynchronous Serial)
FILE *file; //this approach does working!!!!!
int num_of_read = 43;
uint8_t shifter = 0x30;
time_t now;
int tempicon;
int flag_waypoint;


void mainmenu()
{
	cout<<"-------------------------------------------------------------------------------"<<endl;
	cout<<"                         console of waypoint configuration                     \n"<<endl;
	cout<<"   0.    import waypoint info from file"<<endl;
	cout<<"   1.    select number of waypoints"<<endl;
	cout<<"   2.    set values(LAT and LON)"<<endl;
	cout<<"   3.    check current setting"<<endl;
	cout<<"   4.    display GPS data"<<endl;
	cout<<"   5.    terminate control"<<endl;
	cout<<"-------------------------------------------------------------------------------"<<endl;
	while(1)
	{
		cin>>selection;
		if((selection >= 0) && (selection <= 5) && (sizeof(selection)==sizeof(int)))
		{
			cout<<"choose: "<<selection<<endl;
			break;
		}
		else
		{
			cout<<"invalied input, please type again, press Enter to confirm"<<endl;
		}
	}

}
int menu0()
{
	string name_of_file;
	char temp[100];
	cout<<"please name the file, include extension, type 0 to cancel"<<endl;
	cin>>name_of_file;
	strcpy(temp, name_of_file.c_str());
	if(temp[0] == '0') return 0;
	ifstream open_file(temp);
	if(open_file.is_open())
	{
		open_file >> num_of_waypoint;
		for(int i = 0; i < num_of_waypoint; ++i)
		{
			open_file >> LAT[i];
			open_file >> LON[i];
		}
		cur_of_waypoint = 0;
		open_file.close();
		cout<<"import succeed"<<endl;
		return 0;
	}
	else
	{
		cout<<"open failed, please retry"<<endl;
		return 0;
	}
}
int menu1()
{
	double temp = 0;
	int nestkiller = 0;
	char confirm;
	cout<<"1.select number of waypoints (max:5),press 0 to terminate this operation"<<endl;
	while(1)
	{
		cin>>temp;
		temp = int(temp);
		if(temp == 0)
		{
			return 0;
		}
		else if((temp > 0) && (temp < 6))
		{
			cout<<"you will have: "<<temp<<"waypoint(s)"<<endl;
			cout<<"if this is true, press 'y',press any key to withdraw"<<endl;
			cin>>confirm;
			if(confirm == 'y' || confirm == 'Y')
			{
				num_of_waypoint = temp;
				cur_of_waypoint = 0;
				nestkiller = 1;
			}
			else 
			{
				cout<<"would you like to re-enter or terminate? 'y' to re-enter"<<endl;
				cin>>confirm;
				if(confirm == 'y' || confirm == 'Y')
				{
					cout<<"please type in number of waypoint, press 0 to terminate this operation"<<endl;
				}
				else
					nestkiller = 1;
			}
		}
		else
		{
			cout<<"invalied input, please type again, press Enter to confirm"<<endl;
		}
		if(nestkiller == 1) break;
	}
	return 0;
}
int menu2()
{
	char confirm;
	int cancel = 0;
	if(num_of_waypoint == 0)
	{
		cout<<"you have not specify the number of waypoints, "
			<<"please go back to step 1 set your waypoint first"<<endl;
		cout<<"press any key to continue:"<<endl;
		cin>>confirm;
		return 0;
	}
	double tempLAT[5] = {0,0,0,0,0};
	double tempLON[5] = {0,0,0,0,0};
	for(int i = 0; i < num_of_waypoint; ++i)
	{
		int nestkiller = 0;
		cout<<"set your waypoint, this is "<<i+1<<"th waypoint"<<endl;
		cout<<"type in your "<<i+1<<" waypoint, LAT, press 0 to terminate this operation"<<endl;
		while(cin>>tempLAT[i])
		{
			if(tempLAT[i] == 0)
			{
				cout<<"you have terminate the set-up"<<endl;
				return 0;
			}
			else if((tempLAT[i] > 30.0) && (tempLAT[i] < 35.0) && (sizeof(tempLAT[i])==sizeof(double)))
			{
				cout<<"latitude"<<i+1<<" is: "<<setw(10)<<setprecision(6)<<fixed<<right<<tempLAT[i]<<". is this correct? \ny/n"<<endl;
				cin>>confirm;
				if((confirm == 'y') || (confirm == 'Y')) nestkiller = 1;//this is the correct input
				else cout<<"cancelled, please re-enter latitude"<<i+1<<". 0 to terminate"<<endl;
			}
			else
			{
				cout<<"invalied input, please type again, press anykey to confirm"<<endl;
				cin>>confirm;
				cout<<"type in your "<<i+1<<" waypoint, LAT, press 0 to terminate this operation"<<endl;
			}
			if(nestkiller == 1) 
			{
			cancel = 1;break;
			}
		}
	}
	for(int i = 0; i < num_of_waypoint; ++i)
	{
		int nestkiller = 0;
		cout<<"set your waypoint, this is "<<i+1<<"th waypoint"<<endl;
		cout<<"type in your "<<i+1<<" waypoint, LON, press 0 to terminate this operation"<<endl;
		while(cin>>tempLON[i])
		{
			if(tempLON[i] == 0)
			{
				cout<<"you have terminate the set-up"<<endl;
				return 0;
			}
			else if((tempLON[i] > -120.0) && (tempLON[i] < -110.0) && (sizeof(tempLON[i])==sizeof(double)))
			{
				cout<<"longitute"<<i+1<<" is: "<<setw(10)<<setprecision(6)<<fixed<<right<<tempLON[i]<<". is this correct? \ny/n"<<endl;
				cin>>confirm;
				if((confirm == 'y') || (confirm == 'Y')) nestkiller = 1;//this is the correct input
				else cout<<"cancelled, please re-enter longitute"<<i+1<<". 0 to terminate"<<endl;
			}
			else
			{
				cout<<"invalied input, please type again, press anykey to confirm"<<endl;
				cin>>confirm;
				cout<<"type in your "<<i+1<<" waypoint, LON, press 0 to terminate this operation"<<endl;
			}
			if(nestkiller == 1) 
			{cancel = 1;break;}
		}
	}
	if(cancel == 1)
	{
		cout<<"Ok, this is your waypoint: "<<endl;
		for(int i = 0; i < num_of_waypoint; ++i)
		{
			cout<<"LATITUDE: "<<setw(10)<<setprecision(6)<<fixed<<right<<tempLAT[i]
			 <<"  LONGITUDE: "<<setw(10)<<setprecision(6)<<fixed<<right<<tempLON[i]<<endl;
		}
		cout<<"press 'y' to confirm these value, any key to cancel these value:"<<endl;
		cin>>confirm;
		if((confirm == 'y') || (confirm == 'Y'))
		{
			for(int i = 0; i < num_of_waypoint; ++i)
			{
				LAT[i] = tempLAT[i];
				LON[i] = tempLON[i];
			}
		cout<<"your command has been successfully recorded"<<endl;
		}
		else
		{
			cout<<"your operation has been cancelled"<<endl;
			return 0;
		}
	}
	else
	{
		cout<<"you have withdral your config"<<endl;
		return 0;
	}
	return 0;
}
void menu3()
{
	cout<<"this is your current configuration "<<endl;
	cout<<"number of waypoint(s): "<<num_of_waypoint<<endl;
	for(int i = 0; i < num_of_waypoint; ++i)
	{
		cout<<"latitude"<<i+1<<" : "<<setw(10)<<setprecision(6)<<fixed<<right<<LAT[i]
		 <<"  longitude"<<i+1<<" : "<<setw(10)<<setprecision(6)<<fixed<<right<<LON[i]<<endl;
	}
}

void menu4()
{
	ofstream JP("output.txt");
	char n[1] = {'g'};
	char end[1] = {'e'};

	uint8_t buffer[num_of_read];
	uint8_t buffer2[num_of_read - 1];
	uint8_t temp[8];
	double data[5] = {0,0,0,0,0};
	int res = 0;
	int ID = 0;
	int pos = TRUE;

		cout<<"  Latitude   Longitude   Altitude    P          T        distanceToNextWayPoint courseToNextWayPoint  current waypoint"<<endl;
		cout<<"   (deg)       (deg)       (m)    (pascal)   (celsius) 	        (m)	           (deg)"<<endl;
		JP<<"Latitude"<<"	"<<"Longitude"<<"	"<<"Name"<< "	"<<"Description"<<"	"<<"Icon"<<"	"
		  <<"IconAltitude"<< "	" << "IconAltitudeMode"<< "	"<< "LineStringColor" <<"	"
		  <<"LineStringAltitude"<<"	"<<"LineStringAltitudeMode"<< endl;

		config();
		fputs(n,file);
		sleep(1);
		fputs(n,file);
		sleep(2);
		tcflush(fd, TCIOFLUSH);
		rewind(file);
		usleep(100000);
		tcflush(fd, TCIOFLUSH);
		STOP = FALSE;
		while(STOP==FALSE)
		{
			if(wait_flag == FALSE )
			{
				pos = FALSE;
				res = fread(buffer,1,num_of_read,file);//0-7: LAT 8-15: LON 16-23: ALT 24-31: Press 32-39: Temp
				if(res <= 35)
				{
					cout<<"data transmission broken for some reason"<<endl;
				}
				else
				{
					for(int i = 0; i < num_of_read; ++i)
					{
						if(buffer[i] == 0x21)
						{
							ID = i;
						}
					}
					for(int i = 0; i < (num_of_read-1); ++i)
					{
						buffer2[i] = buffer[abs((ID+i+1)%num_of_read)];
					}

					for(int i = 0; i < 5; ++i)
					{
						for(int j = 0; j < 8; ++j)
						{
							temp[j] = buffer2[8*i+j] - shifter;
						}
						data[i] = byte2Double(temp)/1000000.0;
					}
					data[1] = -data[1];

					//-------------------------------------------display block-----------------------------------------------
				/* for (1): display lat long alt
				 * for (2): display P T
				 * for (3): display dis course time
				 */
					for(int i = 0; i < 2; ++i)
						cout<<setw(12)<<setprecision(6)<<fixed<<right<<data[i];
					for(int i = 2; i < 5; ++i)
						cout<<setw(10)<<setprecision(1)<<fixed<<right<<data[i];
					if(num_of_waypoint > 0)
					{
						cout<<setw(20)<<setprecision(1)<<fixed<<right<<distanceBetween(data[0],data[1],LAT[0],LON[0])
							<<setw(20)<<setprecision(1)<<fixed<<right<<courseTo(data[0],data[1],LAT[0],LON[0]);
					}
					else
					{
						cout<<"			***************************         ";
					}
					cout<<cur_of_waypoint<<"  ";
					now = time(0);
					cout << "  " << ctime(&now);
					//--------------------------------------------------------------------------------------------------------
					//------------------------------------txt file block------------------------------------------------------
					if((data[0] > 30.0) && (data[0] < 35.0) && (data[1] < -110.0) && (data[1]) > -120.0)
					{
						if((distanceBetween(data[0],data[1],LAT[cur_of_waypoint],LON[cur_of_waypoint]) < 5.0) && ((flag_waypoint == FALSE) ))
						{
							tempicon = 157;//waypoint
							flag_waypoint = TRUE;
						}
						else if((distanceBetween(data[0],data[1],LAT[cur_of_waypoint],LON[cur_of_waypoint]) >= 5.0) && ((flag_waypoint == TRUE) ))
						{
							tempicon = 224;
							++cur_of_waypoint;
							flag_waypoint = FALSE;
						}
						else
						{
							tempicon = 224;
						}
						JP<<setw(9)<<setprecision(6)<<fixed<<right<<data[0]<<"	"
						<<setw(11)<<setprecision(6)<<fixed<<right<<data[1]<<"	"
						<<"name"<<"		"<<tempicon<<"	"<<setw(4)<<setprecision(1)<<fixed<<right
						<<data[2]<<"	"<<"MSL"<<"	"<<"aqua"<<"	"
						<<setw(4)<<setprecision(1)<<fixed<<right<<data[2]<<"	"<<"MSL"<<endl;
					}
					//--------------------------------------------------------------------------------------------------------------------
					tcflush(fd, TCIFLUSH);
					wait_flag = TRUE;
				}
			}
			if((_kbhit() != FALSE) && (pos == FALSE))
			{
				STOP = TRUE;
			}
		}

	fputs(end,file);
	sleep(1);
	JP.close();
	shutdown();
	STOP = FALSE;
}

void menu5(int &ter_program)
{
	char confirm = '0';
	cout<<"would you like to terminate the control? y-yes; any key-no"<<endl;
	cin>>confirm;
	if((confirm == 'y') || (confirm == 'Y'))
	{
		ter_program = 1;
	}
}

void signal_handler_IO (int status)
{
  wait_flag = FALSE;
}

int config()
{
	fd = open(MODEMDEVICE, O_RDWR | O_NONBLOCK);//O_NOCTTY here we need discussion Note: NONBLOCK is the key
	if (fd <0) {perror(MODEMDEVICE); exit(-1); }
	tcgetattr(fd,&oldtio);
	tcsetattr(fd,TCSANOW,&oldtio);
	saio.sa_handler = signal_handler_IO;
	sigemptyset(&(saio.sa_mask));
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);
	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, FASYNC); //using Asynchronous method

	memset(&newtio,0,sizeof(newtio));
	newtio.c_cflag &= ~PARENB; //make 8n1
	newtio.c_cflag &= ~CSTOPB;
	newtio.c_cflag &= ~CSIZE;
	newtio.c_cflag &= ~CRTSCTS; //no flow control

	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD; //according to a very nice example in Arduino forum
	newtio.c_iflag = 0;
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
	newtio.c_oflag = 0;// no remapping, no delays
	newtio.c_oflag &= ~OPOST;
	newtio.c_lflag = 0;// no signaling chars, no echo, no canonical processing
	newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	newtio.c_cc[VMIN] = num_of_read;//1.minimum number of reading
	newtio.c_cc[VTIME] = 0;//2.maximum time (in second) to be waiting for
	tcflush(fd, TCIOFLUSH);//flush both input and output buffer
    cfsetospeed(&newtio,B19200);
    cfsetispeed(&newtio,B19200);//I am not sure whether these two are redundant but putting here just for safety
	tcsetattr(fd,TCSANOW,&newtio);//set new attribute
	usleep(100);
	file = fdopen(fd,"w+");//file discriptor opening method, my de facto Saviour T_T, (w+ means open with write but can switch)
	return 0;
}

int shutdown()
{
        /* restore old port settings */
		tcflush(fd, TCIOFLUSH);
        tcsetattr(fd,TCSANOW,&oldtio);//set back the old config
        return (fclose(file));//close STREAM*FILE
}


void Double2byte(double DATA, char* output)  //converting double to byte array
{
  long l = long(DATA);        //1. convert the float to long int
  for(int i = 0; i < 8; i++)
  output[i] = (char)((l >> ((7 - i)*8)) & 0xff); //2.convert the long int to byte array
}

double byte2Double(uint8_t* input)  //converting byte array to double
{
  long accum = 0;
  int i = 7;
  for ( int shiftBy = 0; shiftBy < 64; shiftBy += 8 ) //1. convert each byte to long int
  {
     accum |= ( (long)( input[i] & 0xff ) ) << shiftBy; //2. the byte has smallest weight goes first
     i--;
  }
  return double(accum);  //3. return double type
}

void CVT_double2send(const int num_of_w,const double LAT[],const double LON[],char send[])
{
	char temp[8] = {'0'};
	send[0] = char(num_of_w);
	for(int i = 0; i < num_of_w; ++i)
	{
		Double2byte(LAT[i],temp);
		for(int j = 0; j < 8;++j) send[i*8 + j + 1] = temp[j];
	}
	for(int i = num_of_w; i < 2*num_of_w; ++i)
	{
		Double2byte(LON[i],temp);
		for(int j = 0; j < 8;++j) send[i*8 + j + 1] = temp[j];
	}
}
void CVT_send2double(int& num_of_w,double LAT[],double LON[],const char send[])
{
	uint8_t temp[8] = {'0'};
	num_of_w = int(send[0]);
	for(int i = 0;i < num_of_w; ++i)
	{
		for(int j = 0; j < 8; ++j) temp[j] = send[i*8 + j +1];
		LAT[i] = byte2Double(temp);
	}
	for(int i = num_of_w;i < 2*num_of_w; ++i)
	{
		for(int j = 0; j < 8; ++j) temp[j] = send[i*8 + j +1];
		LAT[i-num_of_w] = byte2Double(temp);
	}
}
double radians(double val)
{
	return (val*pi/180.0);
}
double degrees(double val)
{
	return (val*180.0/pi);
}
double distanceBetween(double lat1, double long1, double lat2, double long2)
{
	/*sin: param: double
	 * 	   return: double
	 *pow: param1: double
	 *     param2: double
	 *     return:	double
	 */
	  double delta = radians(long1-long2);
	  double sdlong = sin(delta);
	  double cdlong = cos(delta);
	  lat1 = radians(lat1);
	  lat2 = radians(lat2);
	  double slat1 = sin(lat1);
	  double clat1 = cos(lat1);
	  double slat2 = sin(lat2);
	  double clat2 = cos(lat2);
	  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
	  delta = pow(delta,2);
	  delta += pow(clat2 * sdlong,2);
	  delta = sqrt(delta);
	  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
	  delta = atan2(delta, denom);
	  return delta * 6372795.0;
}
double courseTo(double lat1, double long1, double lat2, double long2)
{
	  double dlon = radians(long2-long1);
	  lat1 = radians(lat1);
	  lat2 = radians(lat2);
	  double a1 = sin(dlon) * cos(lat2);
	  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
	  a2 = cos(lat1) * sin(lat2) - a2;
	  a2 = atan2(a1, a2);
	  if (a2 < 0.0)
	  {
	    a2 += (2*pi);
	  }
	  return degrees(a2);
}
int _kbhit()
{
    static const int STDIN = 0;
    static bool initialized = false;

    if (! initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

