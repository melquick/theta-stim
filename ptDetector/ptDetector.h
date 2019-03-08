#include <default_gui_model.h>
#include <cstdlib> //abs()
#include "../include/RealTimeLogger.h"
#include <QTextStream>

class ptDetector : public DefaultGUIModel
{
	Q_OBJECT	

	public:
		ptDetector(void);
		virtual ~ptDetector(void);
		void execute(void);
		void customizeGUI(void);
		
		enum ext_type_t {
		PEAK =1, TROUGH =-1
		};
	
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
		
	public slots:	

	signals:

	private:
		void initParameters();
		int getExtremaType(int);
        	int sign(double); // determines sign of sign of number. -1, 0, or 1
        	int peakOrTrough(double); // determines whether data point is at peak or trough

		ext_type_t extremaType; //Stimulation mode (peak or trough)
		
        	int out;	// output value (1 = stimulation, 0 = no stimulation)
		int type;	// peak or trough detected (peak = 1, trough = -1, nothing = 0)
		double lastpeak; // stores time of most recent peak
		double lasttrough; //stores time of most recent trough
		double lastext; // stores time of most recent peak or trough
		double laststim; //time of last stimulus (start of refractory period)

		double Tp; //time of previous data point
		double Tn; //time of current data point (equal to systime)
		double Dp; //derivative of previous data point
		double Dn; //derivative of current data point
       	 	double Ap; //amplitude of previous data point
		double An; //amplitude of current data point
		
		bool refrac; //stimulate when not in refractory period (stimdur)
        
		//Parameters
		const double fullcycle = 0.083; // s, corresponds to 12Hz
		const double halfcycle = 0.041; // s, corresponds to 12 Hz
		const double threshold = 1.5e-4; // in V
		const double stimdur = 0.002; //duration of stimulus in s

		double dt;	// system period
		double systime;	// runtime of command
		long long count;	// counter for samples executed during current command

		//QT components
		QComboBox *extremaTypeSelect;
		
		// DataLogger
		RealTimeLogger *data;
		double maxt, tcnt;
		int acquire, cols;
		int cellnum, tempcell;
		string prefix, info, path;
		double pdone;

	private slots:
		void updateType(int);
};
