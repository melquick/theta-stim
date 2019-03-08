
/*
 * Filter module using IIR Chebyshev Type II
 * Filters from 4 to 10 Hz (Theta frequency)
 * Stopband attenuation: 40 dB, passband ripple: 2 dB, and sampling frequency: 1 kHz
 */

#include <default_gui_model.h>
#include <math.h>
#include <string>
#include "../include/RealTimeLogger.h"

class chebyFilter : public DefaultGUIModel
{	
	public:
		chebyFilter(void);
		virtual ~chebyFilter(void);
		void execute(void);

	protected:
		virtual void update(DefaultGUIModel::update_flags_t);
		
	private:
		void initParameters();
		double readFromBuffer(int index);
		void writeToBuffer(double val);
		double conv(double val);
		
		double x; //input voltage
		double y; //filtered voltage
		int start; //start pointer
		static const int N = 969; //buffer size and number of filter taps
		static constexpr double WAIT_TIME = 0.969; //wait time in seconds to fill buffer
		bool wait; //wait flag
		
		double dt;	// system period
		double systime;	// runtime of command
		long long count;	// counter for samples executed during current command
		
		double b[N] = {0}; //circular buffer
		double h[N] = {0}; //impulse response of filter
		
		string path, file;
};

