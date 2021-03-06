/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
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
		ShowHelp("move_to_spot");
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
		if (action)
			action.cancel();		
		//move to location (2, 0), not on virtual track
		rpos::features::motion_planner::MoveOptions options;
		options.flag = MoveOptionFlag(MoveOptionFlagMilestone | MoveOptionFlagPrecise);
		action = sdp.moveTo(rpos::core::Location(2, 0), options);
		action.waitUntilDone();
		if (action.getStatus() == rpos::core::ActionStatusError)
			std::cout << "Action Failed: " << action.getReason() << std::endl;
		//draw a virtual track from (0, 0) to (2, 0), then move to (0, 0) via virtual track
		rpos::core::Line line(rpos::core::Point(0,0),rpos::core::Point(2,0));
		sdp.addLine(ArtifactUsageVirtualTrack, line);
		options.flag = MoveOptionFlag(MoveOptionFlagKeyPoints | MoveOptionFlagPrecise);
		action = sdp.moveTo(rpos::core::Location(0, 0), options);
		action.waitUntilDone();
		if (action.getStatus() == rpos::core::ActionStatusError)
			std::cout << "Action Failed: " << action.getReason() << std::endl;
	}
	catch (const rpos::system::detail::ExceptionBase& e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
	return 0;
} 