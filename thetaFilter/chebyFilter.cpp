/*


*/

#include "chebyFilter.h"
#include <default_gui_model.h>
#include <main_window.h>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

extern "C" Plugin::Object *createRTXIPlugin(void)
{
	return new chebyFilter();
}

static DefaultGUIModel::variable_t vars[] = {
	{"Vin","Unfiltered input voltage", DefaultGUIModel::INPUT, },
	{"Vout", "Filtered output voltage", DefaultGUIModel::OUTPUT, },
};
static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t); 

chebyFilter::chebyFilter(void) : DefaultGUIModel("Theta Filter", ::vars, ::num_vars)
{
	setWhatsThis(
		"<p><b>Chebyshev Filter Type II:</b><br>This module filters theta waves from 4 to 10 Hz.</p>");
	initParameters();
	DefaultGUIModel::createGUI(vars, num_vars);
	update(INIT);
	refresh();
	resizeMe();
}

chebyFilter::~chebyFilter(void)
{
}

void chebyFilter::execute(void) 
{
	systime = count * dt;
	count++;
	
	if (systime > WAIT_TIME)
	{
		wait = false;
	}
	
	x = input(0);
	writeToBuffer(x);
	
	if (wait)
		output(0) = 0.0;
	else
	{
		y = conv(x);
		output(0) = y;	
	}
}

double chebyFilter::readFromBuffer(int index)
{
	return b[index];
}

void chebyFilter::writeToBuffer(double val)
{
	start = ((start-1) % N + N) % N;
	b[start] = val;
}

double chebyFilter::conv(double val)
{
	y = 0;
	for (int i = 0; i < N; i++)
	{
		y += h[i]*b[((i+start) % N + N) % N];
	}
	return y;
}

void chebyFilter::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
	case INIT:
		wait = true;
		systime = 0;
		count = 0;
		break;
	case MODIFY:
		wait = true;
		systime = 0;
		count = 0;
		break;
	case PERIOD:
		dt = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
		output(0) = 0.0;
		break;
	case PAUSE:
		output(0) = 0.0;
		break;
	case UNPAUSE:
		wait = true;
		systime = 0;
		count = 0;
		break;
	default:
		break;
	}
}

void chebyFilter::initParameters()
{
	output(0) = 0;
	y = 0;
	start = 0;
	dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
	systime = 0;
	count = 0;
	wait = true;
	
	path = "/home/ndl/Modules/thetaFilter/";
	file = "impulse_response.csv";
	
	ifstream fin;
	fin.open(path+file,ios::in);
	string line;
	getline(fin,line);
	
	if(!fin.good())
		std::cout << "Error with csv input of filter impulse response. File path is incorrect, file is empty, or data is not in row format." << endl;

	stringstream iss(line);
	
	for (int i = 0; i < N; i++)
	{
		string val;
		if (i == (N-1))
		{
			getline(iss,val);
		}
		else 
		{
			getline(iss,val,',');
			if(!iss.good())
			{
				std::cout << "Error with csv input of filter impulse response." << endl;
				break;
			}
		}
		stringstream converter(val);
		converter >> h[i];
	}
	
	fin.close();
}
