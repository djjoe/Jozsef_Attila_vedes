#include <iostream>
#include <cstddef>
#include <osmium/osm/relation.hpp>
#include <osmium/visitor.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/index/map/sparse_table.hpp>
#include <osmium/io/any_input.hpp>
#include <boost/concept_check.hpp>

class MyHandler : public osmium::handler::Handler {
public:
	osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> locations;
	std::map<osmium::unsigned_object_id_type , const char*> map;
	
	void node( osmium::Node& node ) {
		const char* value= node.tags().get_value_by_key("highway");
		const char* name = node.tags().get_value_by_key("name");
		if(name && value && !strcmp(value, "bus_stop")){
			map[node.positive_id()] = name;
		}
		
	}

	void relation( osmium::Relation& relation ) {
		const char* bus = relation.tags()["route"];
		if(bus && !strcmp(bus, "bus")) {
			std::cout << relation.get_value_by_key("ref");
			osmium::unsigned_object_id_type temp1, temp2;
			int db = 0;
			for( auto &member : relation.members()){
				if(db == 0 && member.type() == osmium::item_type::node) {
					temp1 = member.positive_ref();
					db++;
				} else if(db !=0 && member.type() == osmium::item_type::node){
					temp2 = member.positive_ref();
				}				
			}
			
			std::cout << " (" << map[temp1] << " - " << map[temp2] << ")" << std::endl;
			
			for ( auto &member : relation.members()) {
				if( member.type() == osmium::item_type::node){
					std::cout << " " << locations.get(member.positive_ref()).lat() << ", " << locations.get(member.positive_ref()).lon() << " " << map[member.positive_ref()] << std::endl;
				}
				
			}
			std::cout << std::endl;
		}
	}
};

int main ( int argc, char* argv[] ) {
	if(argc == 2) {
		osmium::io::File infile ( argv[1] );
		osmium::io::Reader reader ( infile, osmium::osm_entity_bits::all );

		MyHandler myhandler;
		osmium::handler::NodeLocationsForWays<osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location>> node_locations ( myhandler.locations );
		
		osmium::apply ( reader, node_locations, myhandler );
		reader.close();
		
		google::protobuf::ShutdownProtobufLibrary();
	}
	return 0;
}