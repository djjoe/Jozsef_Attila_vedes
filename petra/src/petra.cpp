#include <iostream>
#include <cstddef>
#include <osmium/io/any_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>
#include <osmium/osm/node.hpp>
#include <osmium/osm/way.hpp>
#include <osmium/osm/relation.hpp>
#include <osmium/index/map/sparse_table.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/geom/haversine.hpp>
#include <osmium/geom/coordinates.hpp>
#include <limits>

std::vector<osmium::unsigned_object_id_type> getNodesByDist(std::vector<osmium::unsigned_object_id_type>& nodes);

class Handler : public osmium::handler::Handler
{
public:

    osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> locations;
    
    std::map<osmium::unsigned_object_id_type, std::vector<osmium::unsigned_object_id_type>> map;
    
    void way(const osmium::Way& way)
    {
	std::vector<osmium::unsigned_object_id_type> nodes;
	const char *highway = way.tags().get_value_by_key("highway");
	if(highway)
	{
		for(auto& node : way.nodes())
		    nodes.emplace_back(node.positive_ref());
		
		map[way.id()] = nodes;
		nodes.clear();
	}
    }    
};

Handler handler;
osmium::unsigned_object_id_type k;
osmium::unsigned_object_id_type v;
std::vector<osmium::unsigned_object_id_type> corners;
std::map<osmium::unsigned_object_id_type, std::vector<osmium::unsigned_object_id_type>> corner_to_corner;

double getDist(osmium::unsigned_object_id_type a, osmium::unsigned_object_id_type b)
{
    auto& A = handler.locations.get(a);
    auto& B = handler.locations.get(b);
    return (A.lat() - B.lat()) * (A.lat() - B.lat()) + (A.lon() - B.lon()) * (A.lon() - B.lon());
}

bool isItCorner(osmium::unsigned_object_id_type a, std::vector<osmium::unsigned_object_id_type>& corners)
{
    for (auto& c : corners)
	if (a == c)
	  return true;
    return false;
}

osmium::unsigned_object_id_type getNearest(double lat, double lon)
{
    double min = std::numeric_limits<double>::max();
    osmium::unsigned_object_id_type back = 0;
    for(auto& a : corner_to_corner)
    {
	auto& loc = handler.locations.get(a.first);
	double tmp = (lat - loc.lat()) * (lat - loc.lat()) + (lon - loc.lon()) * (lon - loc.lon());
	if (tmp < min)
	{
	    min = tmp;
	    back = a.first;
	}
    }
    
    return back;
}

std::vector<osmium::unsigned_object_id_type> getCornersByDist(osmium::unsigned_object_id_type be)
{
    double tmp;
    std::vector<double> td;
    std::vector<osmium::unsigned_object_id_type> back;
    auto cc = corner_to_corner.at(be);
    for (auto& c : cc)
	td.push_back(getDist(c, v));

    for(size_t j = 0; j < td.size(); j++)
    {
	size_t min = j;
	for (size_t i = j + 1; i < td.size(); i++)
	    if (td[i] < td[min])
		min = i;
	    
	back.push_back(cc[min]);
	tmp = td[min];
	td[min] = td[j];
	td[j] = tmp;
	cc[min]  = cc[j];
    }   
    
    return back;
}

void goToEnd()
{
    static bool end = false;
    
    auto& be = corners[corners.size() - 1];
    
    if (be == v)
    {
	end = true;
	return;
    }
    
    std::vector<osmium::unsigned_object_id_type> t = getCornersByDist(be);
    for(auto& b : t)
	if (std::find(corners.begin(), corners.end(), b) == corners.end())
	{
	    corners.push_back(b);
	    goToEnd();
	    if (end)
		return;
	}
	
    corners.pop_back();	
}


int main ( int argc, char* argv[] )
{
    if ( argc != 2 )
    {
	return 1;
    }

    osmium::io::File infile ( argv[1] );
    osmium::io::Reader reader ( infile, osmium::osm_entity_bits::all );

    osmium::handler::NodeLocationsForWays<osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location>>
	      node_locations ( handler.locations );

    osmium::apply ( reader, node_locations, handler );
    reader.close();

    for(auto& mf1 : handler.map)
    {
	if (std::find(corners.begin(), corners.end(), mf1.second[0]) == corners.end())
	    corners.push_back(mf1.second[0]);
	if (mf1.second.size() != 1 &&
	  std::find(corners.begin(), corners.end(), mf1.second[mf1.second.size()-1]) == corners.end())
	    corners.push_back(mf1.second[mf1.second.size()-1]);		
      
	for (auto& mf2 : handler.map)
	    if (mf1.first != mf2.first)
		for (auto& nf1 : mf1.second)
		    for (auto& nf2 : mf2.second)
			if (nf1 == nf2 && std::find(corners.begin(), corners.end(), nf1) == corners.end())
			    corners.push_back(nf1);
    }

    bool goodWay;
    
    for (auto& c : corners)
    {
	for (auto& w : handler.map)
	{
	    goodWay = false;
	    for (auto& g : w.second)
		if (g == c)
		      goodWay = true;
		
	    if (goodWay)
	    {
		std::vector<osmium::unsigned_object_id_type> cornersHere;
		for (auto& g : w.second)
		    if(isItCorner(g, corners))
		      cornersHere.push_back(g);
		    
		size_t i;
		for (i = 0; c != cornersHere[i]; i++);
		
		for (size_t j = 0; j < cornersHere.size(); j++)
		    if (i - 1 == j || i + 1 == j)
			corner_to_corner[c].push_back(cornersHere[j]);
	    }
	}      
    }
    
    corners.clear();       
    
    double ka, ko;
    double va, vo;
    
    std::cin >> ka;
    std::cin >> ko;
    std::cin >> va;
    std::cin >> vo;
    
    k = getNearest(ka, ko);
    v = getNearest(va, vo);
    
    corners.push_back(k);
    goToEnd();  
    
    std::vector<osmium::unsigned_object_id_type> ki;
    int j;
    for (size_t i = 1; i < corners.size(); i++)
    {
	double disti = getDist(corners[i - 1], corners[i]);
	for (auto& a : handler.map)
	{
	    j = 0;
	    for (auto& b : a.second)
		if (b == corners[i - 1] || b == corners[i])
		    j++;
		
	    if (j == 2)
	    {
		std::vector<osmium::unsigned_object_id_type> ki2;
		std::vector<double> kit;
		j = 0;
		for (auto& b : a.second)
		{
		    if (b == corners[i - 1] || b == corners[i])
			j++;
		    
		    if (j == 1 && std::find(ki.begin(), ki.end(), b) == ki.end() && getDist(corners[i - 1], b) < disti && getDist(corners[i], b) < disti)
		    {
			ki2.push_back(b);
			kit.push_back(getDist(corners[i - 1], b));
		    }
		    else if (j == 2 && std::find(ki.begin(), ki.end(), b) == ki.end())
		    {
			ki.push_back(corners[i - 1]);
			double tmp;
			for(size_t j = 1; j < kit.size(); j++)
			{
			    size_t min = j;
			    for (size_t m = j + 1; m < kit.size(); m++)
				if (kit[m] < kit[min])
				    min = m;
				
			    ki.push_back(ki2[min]);
			    tmp = kit[min];
			    kit[min] = kit[j];
			    kit[j] = tmp;
			    ki2[min]  = ki2[j];
			}
			
			break;
		    }
		}
	    }
	}
    }
      
    ki.push_back(v);
    for (auto& a : ki)
	std::cout << handler.locations.get(a).lat() << " " <<
	      handler.locations.get(a).lon() << std::endl;
	      
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
