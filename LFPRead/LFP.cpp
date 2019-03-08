//Read Sample LFP one data point at a time
#include <iostream>
#include <fstream>
#include "LFP.h"
using namespace std;

extern "C" Plugin::Object *createRTXIPlugin(void)
{
	return new LFP();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "LFP", "LFP Voltage", DefaultGUIModel::OUTPUT},
	{ "File Path", "", DefaultGUIModel::COMMENT,},
	{ "File Name", "", DefaultGUIModel::COMMENT,},
	{ "Time (s)", "Time (s)", DefaultGUIModel::STATE, },  
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

LFP::LFP(void) : DefaultGUIModel("LFP", ::vars, ::num_vars)
{
	setWhatsThis(
		"<p><b>LFP:</b><br>This module reads a sample LFP to simulate a real-time analysis.</p>");
	initParameters();
	DefaultGUIModel::createGUI(vars, num_vars);
	update(INIT);
	refresh();
	resizeMe();
}

LFP::~LFP(void)
{
}

void LFP::execute(void)
{
	v2 = 0;
	if (fs.is_open())
	{
		getline(fs,line);
        	v2 = stof(line.substr(line.find("\t")+1, line.length()),0); //v2 is amplitude data
	}
	output(0) = v2;
}

void LFP::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
	case INIT:
	qDebug("INIT is called");
		setComment("File Path", QString::fromStdString(path));
		setComment("File Name", QString::fromStdString(file));
		break;
		
	case MODIFY:
	qDebug("MODIFY is called");

	qDebug()<< QString::fromStdString(path);
	qDebug()<< getComment("File Path");

		path = getComment("File Path").toStdString();
		file = getComment("File Name").toStdString();
		
	qDebug()<< QString::fromStdString(path);
		
	case PERIOD:
		break;
	case PAUSE:
		output(0) = 0.0;
		break;
	case UNPAUSE:
		break;
	default:
		break;
	}
}

void LFP::initParameters()
{	
	v2 = 0;
	line = "\0";
	
	path = "/home/ndl/RTXI/data/";
	file = "LFP_channel1.txt";

	fs.open(path+file);

	if (fs.fail())
		std::cout << "Could not open LFP file." << endl;

	qDebug()<< QString::fromStdString(path);

}

