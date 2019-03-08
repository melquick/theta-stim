#include <cstdlib> //abs()
#include <default_gui_model.h>
#include <main_window.h>
#include <string>
#include <QTextStream>
#include "ptDetector.h"
#include <iostream>

extern "C" Plugin::Object *createRTXIPlugin(void)
{
	return new ptDetector();
}

static DefaultGUIModel::variable_t vars[] =
{
	{ "Vin", "Local field potential (V)", DefaultGUIModel::INPUT, },
	{ "Stimulation", "Output simulation (0 = no stimulation, 1 = stimulation", DefaultGUIModel::OUTPUT, },
	{ "Acquire?", " 0 or 1", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Cell Number", "Cell identifier",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "Record Length (s)", "duration of recording session",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },
	{ "File Path", "", DefaultGUIModel::COMMENT,},
	{ "File Prefix", "", DefaultGUIModel::COMMENT,},
	{ "File Info", "", DefaultGUIModel::COMMENT,},
	{ "Time (s)", "Time (s)", DefaultGUIModel::STATE, }, 
	{ "Percent Done", "", DefaultGUIModel::STATE | DefaultGUIModel::DOUBLE, },	  
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);
 
ptDetector::ptDetector(void) : DefaultGUIModel("p_t_detector", ::vars, ::num_vars)
{
	setWhatsThis(
		"<p><b>Peak and Trough Detector:</b><br>This module detects and stimulates peaks or troughs of theta waves.</p>");
	initParameters();
	DefaultGUIModel::createGUI(vars, num_vars);
	customizeGUI();
	update(INIT);
	refresh();
	resizeMe();
}

ptDetector::~ptDetector(void)
{
}

void ptDetector::execute(void) 
{
	systime = count * dt; // time in seconds
	Tn = systime;
	An = input(0); // current amplitude
	Dn = (An-Ap)/(Tn-Tp); //current derivative
	out = 0;
		
	if (((Tn - laststim) > stimdur) && refrac)
	{	
		refrac = false;
	}
		
	if (((sign(Dn) != sign(Dp)) && (count >= 2))) // determines if extrema
        {
		type = peakOrTrough(Dn);
		
		if (((Tn - lastext) >= halfcycle) && (abs(An) >= threshold))
		{
			//Peaks
			if ((type == 1) && ((Tn - lastpeak) >= fullcycle))
			{
				lastpeak = Tn;
				lastext = Tn;
			}
			// Troughs
			else if ((type == -1) && ((Tn - lasttrough) >= fullcycle))
			{
				lasttrough = Tn;
				lastext = Tn;
			}
		}
	}

	if ((type == extremaType) && (Tn == lastext) && (!refrac) && (count >= 2))
	{
		out = 1;
		laststim = Tn;
		refrac = true;
	}
			
	//Set all previous time, amplitude, and derivative values to current values
	Tp = Tn;
	Ap = An;
	Dp = Dn;
		
	count++;
		
	if (refrac)
	{
		out = 1;	
	}
	
	output(0) = out;
		
			
	// Record data if acquire is 1
	if (acquire && tcnt < maxt) {
		data->insertdata(tcnt);
		data->insertdata(An);
		data->insertdata(out);
		data->insertdata(extremaType);
		
		tcnt += dt;
		pdone = (tcnt/maxt)*100;
	
	} else if (acquire && tcnt > maxt) {
		tcnt = 0;		// reset timer
		acquire = 0;	// reset acquire signal
		
		data->writebuffer(prefix,info);
		data->resetbuffer();
	}
}


int ptDetector::sign(double x)
{
    if (x > 0)
        return 1; //positive
    else if (x < 0)
        return -1; //negative
    else
        return 0; //zero
}

int ptDetector::peakOrTrough(double d)
{
    //Assumes that current derivative and previous derivative are different signs already
	//Peak = 1, Trough = -1
    if (sign(d) == 1)
        return -1; //trough
    else if (sign(d) == -1)
        return 1; //peak
    else
        return 0;
}

int ptDetector::getExtremaType(int index)
{
	switch(index) {
		case 0:
			return PEAK;
		case 1:
			return TROUGH;
	}
}

void ptDetector::updateType(int index)
{
	switch(index)
	{
		case 0:
			extremaType = PEAK;
			break;
		case 1:
			extremaType = TROUGH;
			break;
	}
}

void ptDetector::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
	case INIT:
	qDebug("INIT is called");
		setState("Time (s)", systime);
		extremaTypeSelect->setCurrentIndex(0);

		setParameter("Record Length (s)", QString::number(maxt));	
		setParameter("Acquire?",acquire);
		setParameter("Cell Number",cellnum);
		setComment("File Path", QString::fromStdString(path));
		setComment("File Prefix", QString::fromStdString(prefix));
		setComment("File Info", QString::fromStdString(info));
		setState("Percent Done", pdone);
		
		break;
		
	case MODIFY:
	qDebug("MODIFY is called");
		extremaType = ext_type_t(getExtremaType(extremaTypeSelect->currentIndex()));

		acquire = getParameter("Acquire?").toUInt();
		tempcell = getParameter("Cell Number").toInt();

	qDebug()<< QString::fromStdString(path);
	qDebug()<< getComment("File Path");

		path = getComment("File Path").toStdString();
		prefix = getComment("File Prefix").toStdString();
		info = getComment("File Info").toStdString();
		maxt = getParameter("Record Length (s)").toDouble();		
	qDebug()<< QString::fromStdString(path);

		data->deleteBuffer();
		data->resetbuffer();
		data->setBufferLen((int)(cols*maxt/dt));
		if (tempcell != cellnum) {
			data->newcell(tempcell);
			cellnum = tempcell;
		}		

		// reset the timers
		pdone = 0.0;
		tcnt = 0.0;
		systime = 0;
		count = 0;
		
		//reset output
		out = 0;
		
	case PERIOD:
		dt = RT::System::getInstance()->getPeriod() * 1e-9; // time in seconds
		break;
	case PAUSE:
		output(0) = 0.0;
		break;
	case UNPAUSE:
		systime = 0;
		count = 0;
		Tp = 0;
		Tn = 0;
		Dp = 0;
		Dn = 0;
		Ap = 0;
		An = 0;
		type = 0;
		lastext = 0;
		lastpeak = 0; 
		lasttrough = 0; 
		laststim = 0;
		refrac = false;
		break;
	default:
		break;
	}
}

void ptDetector::initParameters()
{
	extremaType = PEAK;
	output(0) = 0;
	out = 0;
	dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
	systime = 0;
	count = 0;
	
	type = 0;
    lastext = 0;
    lastpeak = 0; 
    lasttrough = 0; 
	laststim = 0;
	refrac = false;
	
	Tp = 0;
	Tn = 0;
	Dp = 0;
	Dn = 0;
    Ap = 0;
	An = 0;
	
	cellnum = 1;
	acquire = 0;
	cols = 4;
	
	tcnt = 0.0;
	maxt = 0.0;
	path = "/home/ndl/RTXI/data/";
	prefix = "Theta_Stimulation";
	info = "1kHz: t, V, Stim, Mode";

	qDebug()<< QString::fromStdString(path);
	
	data  = new RealTimeLogger((int)(cols*maxt/dt), cols, path);
	data->newcell(cellnum);
	data->setDSRate(1);
	data->setPrint(1);
	
	pdone = 0.0;

}

void ptDetector::customizeGUI(void)
{
	QGridLayout *customlayout = DefaultGUIModel::getLayout();

	QGroupBox *formGroupBox = new QGroupBox("Stimulation Mode");
	QFormLayout *layout = new QFormLayout(formGroupBox);
	
	extremaTypeSelect = new QComboBox;
	extremaTypeSelect->insertItem(0,"Peak");
	extremaTypeSelect->insertItem(1,"Trough");
	QObject::connect(extremaTypeSelect,SIGNAL(activated(int)), this, SLOT(updateType(int)));

	layout->addRow(tr("Stimulation Mode:"), extremaTypeSelect);
	
	formGroupBox->setLayout(layout);

	customlayout->addWidget(formGroupBox,0,0);
	setLayout(customlayout);
}
