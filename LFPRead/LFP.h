#include <default_gui_model.h>
#include <string>
#include <fstream>

class LFP : public DefaultGUIModel
{
    public:
        LFP(void);
	      virtual ~LFP(void);
	      void execute(void);
		  
	protected:
		virtual void update(DefaultGUIModel::update_flags_t);

    private:
	void initParameters();
        
	float v1; // holds time data
        float v2; // holds amplitude data
 	std::string line; // holds tab deliminated time and amplitude data from one data point
	std::string path, file;
	std::ifstream fs;
};
