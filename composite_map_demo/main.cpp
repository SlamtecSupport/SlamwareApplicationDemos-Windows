/*
 * main.cpp
 *
 * Created by Slamtec at 2018-02-07
 * Copyright 2018 (c) Shanghai Slamtec Co., Ltd.
 */
#include <regex>
#include <rpos/features/location_provider/map.h>
#include <rpos/robot_platforms/slamware_core_platform.h>
#include <rpos/robot_platforms/objects/composite_map.h>
#include <rpos/robot_platforms/objects/composite_map_reader.h>
#include <rpos/robot_platforms/objects/composite_map_writer.h>
#include <rpos/robot_platforms/objects/grid_map_layer.h>
#include <rpos/robot_platforms/objects/line_map_layer.h>
#include <rpos/robot_platforms/objects/pose_map_layer.h>
#include <rpos/robot_platforms/objects/points_map_layer.h>
#include "bitmap_image.hpp"

using namespace rpos::robot_platforms;
using namespace rpos::robot_platforms::objects;
using namespace rpos::features::location_provider;
using namespace rpos::features::artifact_provider;


std::string ip_address = "";
const char *ip_regex = "\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}";
const char *opt_file_name = "map.stcm";
const float map_resolution = 0.05;
bool opt_show_help = false;
bool opt_get_stcm = false;
bool opt_set_stcm = false;
bool opt_modify_stcm = false;

void ShowHelp(std::string app_name) {
    std::cout << "SLAMWARE console demo." << std::endl << "Usage: " << std::endl << 
		app_name << " [OPTS] [filename] <SDP IP Address>" << std::endl << 
		"SDP IP Address      The ip address string of the SLAMWARE SDP." << std::endl << 
		"getstcm filename    download compositeMap" << std::endl << 
		"                    If not specified, the default name map.stms will be used" << std::endl << 
		"setstcm filename    upload compositeMap" << std::endl << 
		"modify bmp_file     modify map data with bmp and artifacts APIs" << std::endl <<
		"-h                  Show this message" << std::endl;
}

bool ParseCommandLine(int argc, const char *argv[]) {
    bool opt_show_help = false;
    for (int pos = 1; pos < argc; ++pos) {
        const char *current = argv[pos];
        if (strcmp(current, "-h") == 0) {
            opt_show_help = true;       
		} 
		else if (strcmp(current, "getstcm") == 0) {
		    opt_get_stcm = true;
			opt_set_stcm = false;
			opt_modify_stcm = false;
			if (++pos < argc) 
				opt_file_name = argv[pos];			
		}
		else if (strcmp(current, "setstcm") == 0) {
		    opt_set_stcm = true;
			opt_get_stcm = false;
			opt_modify_stcm = false;
			if (++pos < argc) 
				opt_file_name = argv[pos];			
		}
		else if (strcmp(current, "modify") == 0) {
			opt_modify_stcm = true;
		    opt_set_stcm = false;
			opt_get_stcm = false;
			if (++pos < argc) 
				opt_file_name = argv[pos];			
		}
		else {
            ip_address = current;
        }
    }
    std::regex reg(ip_regex);
    if (!opt_show_help && !std::regex_match(ip_address, reg) && !opt_get_stcm && !opt_set_stcm && !opt_modify_stcm) 
        opt_show_help = true;    
    if (opt_show_help) {
        ShowHelp("composite_map_demo");
        return false;
    }
    return true;
}

bool StcmMapWriter(const std::string file_name, SlamwareCorePlatform platform) {
	CompositeMap composite_map = platform.getCompositeMap();
    CompositeMapWriter composite_map_writer;
    std::string error_message;
    bool result = composite_map_writer.saveFile(error_message, file_name, composite_map);
    return result;
}

bool StcmMapReader(const std::string file_path, rpos::core::Pose pose, SlamwareCorePlatform platform) {
    CompositeMapReader composite_map_reader;
    std::string error_message;
    boost::shared_ptr<CompositeMap> composite_map(composite_map_reader.loadFile(error_message, file_path));
    if (composite_map) {
        platform.setCompositeMap((*composite_map), pose);	
        return true;
    }
    return false;
}

bool DisplayMapLayerInfo(const std::string file_path) {
	bool result = false;
    CompositeMapReader composite_map_reader;
    std::string error_message;
    boost::shared_ptr<CompositeMap> composite_map(composite_map_reader.loadFile(error_message, file_path));
    if (composite_map) {
        for (auto it = composite_map->maps().begin(); it != composite_map->maps().end(); ++it) {
            auto layer = *it;
            std::string usage = layer->getUsage();
			std::string type = layer->getType();
			std::cout << "Layer Usage : " << usage << std::endl;
			//get grid map layer
            if (type == GridMapLayer::Type) {
				auto grid_map = boost::dynamic_pointer_cast<GridMapLayer>(layer);
				std::cout << "Map Position : (" << grid_map->getOrigin().x() << " , " << 
					grid_map->getOrigin().y() << ")" <<std::endl;
				std::cout << "Map Resolution : (" << grid_map->getResolution().x() <<  
					" , " << grid_map->getResolution().y() << ")" <<std::endl;
				std::cout << "Map Dimension: (" << grid_map->getDimension().x() << 
					" , " << grid_map->getDimension().y() << ")" <<std::endl;
				std::cout << "Map Data:" << std::endl;
				for (auto it = grid_map->mapData().begin(); it != grid_map->mapData().end(); ++it) {
					std::cout << (int)*it << " " ;				
				}
				std::cout << std::endl << std::endl;			
            }
			//get line map layer
            else if (type == LineMapLayer::Type) {
                auto line_map = boost::dynamic_pointer_cast<LineMapLayer>(layer);
				for (auto it = line_map->lines().begin(); it != line_map->lines().end(); ++it) {
					auto line = it->second;
					std::cout << "start: (" << line.start.x() << " , " << line.start.y() << ")" << std::endl;
					std::cout << "end: (" << line.end.x() << " , " << line.end.y() << ")" << std::endl;
				}
				std::cout << std::endl;
            }
			//get pose map layer
			else if (type == PoseMapLayer::Type) {
				auto pose_map = boost::dynamic_pointer_cast<PoseMapLayer>(layer);
				for (auto it = pose_map->poses().begin(); it != pose_map->poses().end(); ++it) {					
					auto pos = it->second;
					std::cout << "Position : (" << pos.pose.x() << " , " << pos.pose.y() << ")" << std::endl;
				}	
				std::cout << std::endl;
			}
			else if (type == PointsMapLayer::Type) {                
				//TODO: get Points map layer
				std::cout << std::endl;
            }
			else {
				//TODO: get unknown map layer
				std::cout << std::endl;
            }
        }
		return true;
    }	
	return true;
}

void LoadMapDataFromBMP(SlamwareCorePlatform platform,const std::string file_path) {

	bitmap_image map_bitmap(file_path);  //read .bmp from file
	int image_width = map_bitmap.width();
	int image_height = map_bitmap.height();
    std::vector<uint8_t> grid_cell_data;
    grid_cell_data.resize(image_width * image_height);
	//get map buffer
	for(size_t pos_y = 0; pos_y< image_height; ++ pos_y) {
		for(size_t pos_x = 0; pos_x< image_width; ++ pos_x) {
			rpos::system::types::_u8 cell_value;
			map_bitmap.get_pixel(pos_x, pos_y, cell_value, cell_value, cell_value);
			grid_cell_data[ pos_x + (image_height - pos_y - 1) * image_width] = cell_value - 128;
		}	
	}
	platform.clearMap();
	platform.setMapUpdate(false);
	BitmapMap map = BitmapMap::createMap();
	map.setMapData(-image_width*map_resolution/2, -image_height*map_resolution/2, image_width, image_height, map_resolution, grid_cell_data);
	platform.setMap(map, MapType::MapTypeBitmap8Bit, MapKind::EXPLORERMAP);
    return;
}

void AddVirtualTracks(SlamwareCorePlatform platform){

	std::vector<rpos::core::Line> tracks;
	//add a 2 * 2 virtual track square
	tracks.push_back(rpos::core::Line(rpos::core::Point(-1, -1), rpos::core::Point(-1, 1)));
	tracks.push_back(rpos::core::Line(rpos::core::Point(-1, 1), rpos::core::Point(1, 1)));
	tracks.push_back(rpos::core::Line(rpos::core::Point(1, 1), rpos::core::Point(1, -1)));
	tracks.push_back(rpos::core::Line(rpos::core::Point(1, -1), rpos::core::Point(-1, -1)));	
	platform.addLines(ArtifactUsageVirtualTrack, tracks);	
}

void AddVirtualWalls(SlamwareCorePlatform platform){

	std::vector<rpos::core::Line> walls;
	//add a 8 * 8 virtual wall square
	walls.push_back(rpos::core::Line(rpos::core::Point(-4, -4), rpos::core::Point(-4, 4)));
	walls.push_back(rpos::core::Line(rpos::core::Point(-4, 4), rpos::core::Point(4, 4)));
	walls.push_back(rpos::core::Line(rpos::core::Point(4, 4), rpos::core::Point(4, -4)));
	walls.push_back(rpos::core::Line(rpos::core::Point(4, -4), rpos::core::Point(-4, -4)));	
	platform.addLines(ArtifactUsageVirtualWall, walls);	
}

int main(int argc, const char * argv[]) {
    if (!ParseCommandLine(argc, argv)) return 1;
	std::cout << "Connecting to SDP @ " << ip_address << std::endl;
	try {
		SlamwareCorePlatform sdp = SlamwareCorePlatform::connect(ip_address, 1445);
        std::cout <<"SDK Version: " << sdp.getSDKVersion() << std::endl;
        std::cout <<"SDP Version: " << sdp.getSDPVersion() << std::endl;	

		if(opt_get_stcm == true) {
			std::cout << "Saving CompositeMap to Local File..." << std::endl;
			StcmMapWriter(opt_file_name, sdp);	
			std::cout << "Display MapLayer Info..." << std::endl << std::endl;
		    DisplayMapLayerInfo(opt_file_name);	
		} 
		else if (opt_set_stcm == true) {
			std::cout << "Loading CompositeMap from Local File..." << std::endl;
		    rpos::core::Pose pose = sdp.getPose();
            StcmMapReader(opt_file_name, pose, sdp);	
		    std::cout << "Display MapLayer Info..." << std::endl << std::endl;
		    DisplayMapLayerInfo(opt_file_name);	
		} 
		else if (opt_modify_stcm == true){
			std::cout << "Load BMP for Local File and Add virtual walls and tracks" << std::endl;
			LoadMapDataFromBMP(sdp, opt_file_name);
			AddVirtualTracks(sdp);
			AddVirtualWalls(sdp);				
		}
		else {
			return 1;		  
		}

	}
	catch (const rpos::system::detail::ExceptionBase &e) {
		std::cout << e.what() << std::endl;
		return 1;	
	}
	return 0;
}