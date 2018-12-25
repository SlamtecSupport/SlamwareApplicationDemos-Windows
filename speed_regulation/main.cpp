#include <regex>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;
using namespace rpos::features::motion_planner;
using namespace rpos::features::artifact_provider;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";
const double pi = 3.1415927;

void ShowHelp(std::string app_name) {
	std::cout << "SLAMWARE console demo." << std::endl << \
		"Usage: " << app_name << " <slamware_address>" << std::endl;
}

bool ParseCommandLine(int argc, const char *argv[]) {
	bool opt_show_help = false;
	for (int pos = 1; pos < argc; ++pos) {
		const char *current = argv[pos];
		if (strcmp(current, "-h") == 0)
			opt_show_help = true;
	    else 
			ip_address = current;		
	}
	std::regex reg(ip_regex);
	if (!opt_show_help && !std::regex_match(ip_address, reg)) 
		opt_show_help = true;
	if (opt_show_help) {
		ShowHelp("speed_regulation");
		return false;
	}
	return true;
}

int main(int argc, const char *argv[]) {
	if (!ParseCommandLine(argc, argv)) return 1;
	std::cout << "Connecting SDP @ " << ip_address << "..."<< std::endl;
	try {
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
		std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
		std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;
		rpos::actions::MoveAction action = sdp.getCurrentAction();
		//
		rpos::core::Location location1(1,0);
		rpos::core::Location location2(-1,0);
		rpos::core::Location location3(0,1.414);
		while (true)
		{
			if (action)
				action.cancel();

			action = sdp.moveTo(location1, false,true);
			if (action.getStatus() == rpos::core::ActionStatusError)
				std::cout << "Action Failed: " << action.getReason() << std::endl;
			bool bRet2 =sdp.setSystemParameter(SYSPARAM_ROBOT_SPEED, SYSVAL_ROBOT_SPEED_HIGH);
			std::cout <<"Robot is moving to: (" << location1.x() <<" , "<<location1.y()<<") on speed"<< "  HIGH " << std::endl;
			action.waitUntilDone();
		
		
			action = sdp.moveTo(location2, false,true);
			if (action.getStatus() == rpos::core::ActionStatusError)
				std::cout << "Action Failed: " << action.getReason() << std::endl;
			
			bool bRet3 =sdp.setSystemParameter(SYSPARAM_ROBOT_SPEED, SYSVAL_ROBOT_SPEED_LOW);
			std::cout <<"Robot is moving to: (" << location2.x() <<" , "<<location2.y()<<") on speed"<< " LOW " << std::endl;
			action.waitUntilDone();
		
		
			action = sdp.moveTo(location2, false,true);
			if (action.getStatus() == rpos::core::ActionStatusError)
				std::cout << "Action Failed: " << action.getReason() << std::endl;
	
			bool bRet1 =sdp.setSystemParameter(SYSPARAM_ROBOT_SPEED, SYSVAL_ROBOT_SPEED_MEDIUM);
			std::cout <<"Robot is moving to: (" << location3.x() <<" , "<<location3.y()<<") on speed"<< " MEDIUM " << std::endl;
			action.waitUntilDone();
		}
	}
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
	return 0;
} 
	
				