/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */

#include <regex>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <rpos/robot_platforms/slamware_core_platform.h>

using namespace rpos::robot_platforms;
using namespace rpos::features::artifact_provider;
using namespace rpos::core;

std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";

void ShowHelp(std::string app_name) {
	std::cout << "SLAMWARE console demo." << std::endl << 
        "Usage: " << app_name << " <slamware_address>" << std::endl;
}

bool ParseCommandLine(int argc, const char * argv[]) {
    bool opt_show_help = false;
    for (int pos = 1; pos < argc; ++pos) {
        const char *current = argv[pos];
        if (strcmp(current, "-h") == 0) 
            opt_show_help = true;
		else 
            ip_address = current;        
    }
	std::regex reg(ip_regex);
    if(!opt_show_help && !std::regex_match(ip_address, reg)) 
        opt_show_help = true;
    if (opt_show_help){
        ShowHelp("artifacts_demo");
        return false;
    }
    return true;
}

int main(int argc, const char * argv[]) {
	if (!ParseCommandLine(argc, argv)) return 1;
	std::cout << "Connecting SDP @ " << ip_address << "..."<< std::endl;
	try{
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
        std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
        std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;

		std::cout << "Clearing existing tracks and walls..." << std::endl;
		sdp.clearLines(ArtifactUsageVirtualTrack);
		sdp.clearLines(ArtifactUsageVirtualWall);

		std::cout << "Adding virtual walls..." << std::endl;
		std::vector<Line> walls;
		//add a 8 * 8 virtual wall square
		walls.push_back(Line(Point(-4, -4), Point(-4, 4)));
		walls.push_back(Line(Point(-4, 4), Point(4, 4)));
	    walls.push_back(Line(Point(4, 4), Point(4, -4)));
		walls.push_back(Line(Point(4, -4), Point(-4, -4)));	
		sdp.addLines(ArtifactUsageVirtualWall, walls);	

		std::cout << "Adding virtual tracks..." << std::endl;
		std::vector<Line> tracks;
		//add a 2 * 2 virtual track square
		tracks.push_back(Line(Point(-1, -1), Point(-1, 1)));
		tracks.push_back(Line(Point(-1, 1), Point(1, 1)));
		tracks.push_back(Line(Point(1, 1), Point(1, -1)));
		tracks.push_back(Line(Point(1, -1), Point(-1, -1)));	
		sdp.addLines(ArtifactUsageVirtualTrack, tracks);	

		std::cout << "Moving vitual walls..." << std::endl;
		//sleep 5 seconds for displaying purpose only, not necessary
		boost::this_thread::sleep_for(boost::chrono::milliseconds(5000)); 
		std::vector<Line> get_walls = sdp.getLines(ArtifactUsageVirtualWall);
		//shrink virtual wall square from 8 * 8 to 6 *6
		for (std::vector<Line>::iterator it = get_walls.begin() ; it != get_walls.end(); ++it) {	
            it->startP().x() *= 0.75f; 
            it->startP().y() *= 0.75f;
		}
		sdp.moveLines(ArtifactUsageVirtualWall, get_walls);

		std::cout << "Get all tracks..." << std::endl;
		std::vector<Line> get_tracks = sdp.getLines(ArtifactUsageVirtualTrack);
		for (std::vector<Line>::iterator it = get_tracks.begin() ; it != get_tracks.end(); ++it) {
			std::cout << "ID: " << it->id() << std::endl;
			std::cout << "Start from (" << it->startP().x() << " , " << it->startP().y() << ") " << "to (" 
				      << it->endP().x() << " , " << it->endP().y() << " ) " << std::endl;		
		}

		std::cout << "Delete track by ID, please enter track ID:" << std::endl;
		int id;
	    bool is_found = false;
		std::cin >> id ;
		for (std::vector<Line>::iterator it = get_tracks.begin() ; it != get_tracks.end(); ++it) {
			if (id == it->id()) {
				sdp.removeLineById(ArtifactUsageVirtualTrack, id);
				is_found = true;
				break;
			}
		}
		if(!is_found)
			std::cout << "Wrong ID" << std::endl;
		return 0;     
	} catch(const rpos::system::detail::ExceptionBase &e) {		
		std::cout << e.what() << std::endl;
		return 1;	
	}
}