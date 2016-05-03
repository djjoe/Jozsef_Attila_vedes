package gedeon;

import br.zuq.osm.parser.*;
import br.zuq.osm.parser.model.*;
import java.util.HashMap;
import java.util.Map;

public class Gedeon {
    
    public static void main(String[] args) throws Exception {
	if (args.length != 1)
		return;
	OSM osm = OSMParser.parse(args[0]);        
        Map<String, String> map = new HashMap<String, String>();
        Map<String, String> lats = new HashMap<String, String>();
        Map<String, String> lons = new HashMap<String, String>();
        for (OSMNode node : osm.getNodes()) {
            String bus = node.tags.get("highway");
            String name = node.tags.get("name");
            if(name != null && bus != null && bus.equals("bus_stop")) {
                map.put(node.id, name);
                lats.put(node.id, node.lat);
                lons.put(node.id, node.lon);
            }
        }
        
        for (Relation relation : osm.getRelations()) {
            String bus = relation.tags.get("route");
            if(bus != null && bus.equals("bus")) {
                
                String temp1 = null;
                String temp2 = null;
                int db = 0;
                for(Member member:relation.members) {
                    if(db == 0 && member.type.equals("node")){
                        temp1 = member.ref;
                        db++;
                    }
                    else if(db != 0 && member.type.equals("node")) temp2 = member.ref;
                }
                
                String name = relation.tags.get("ref");
                if(name == null) name = relation.tags.get("name");
                System.out.println(name + " (" + map.get(temp1) + "-" + map.get(temp2) + ")");
                
                for(Member member : relation.members)
                    if(member.type.equals("node")) System.out.println(" " + lats.get(member.ref) + ", " + lons.get(member.ref) + "  " + map.get(member.ref));
                System.out.println();
            }
        }
    }
}
