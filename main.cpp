#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "quad.h"
#include <fstream>

using namespace std;

int main()
{
	int ter_program = 0;
	while(1)
	{
		mainmenu();
		switch (selection)
		{
			case 0:
			{
				menu0();
				break;
			}
			case 1:
			{
				menu1();
				break;
			}
			case 2:
			{
				menu2();
				break;
			}
			case 3:
			{
				menu3();
				break;
			}
			case 4:
			{
				menu4();
				break;
			}
			case 5:
			{
			    menu5(ter_program);
			    break;
			}
			default:break;
		}//switch
		selection = 0;
		if(ter_program == 1) break;
	}
	cout<<"terminatd control"<<endl;
	return 0;
}
