#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <GL/glut.h>

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

class UtHandler : public osmium::handler::Handler {
public:

    osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location> locations;

    std::map<osmium::unsigned_object_id_type, std::vector<osmium::Location>> map;
    
    double minlat = 100, minlon = 100, maxlat = -100, maxlon = -100;
    
    
    void way(const osmium::Way& way)
    {
	std::vector<osmium::Location> nodes;
	//const char *name = way.tags().get_value_by_key("name");
	/*const char *highway = way.tags().get_value_by_key("highway");
	if(highway)
	{*/
		//std::cout << name << "\n";
	      for(auto node : way.nodes())
	      {
		nodes.emplace_back(node.location());
		const osmium::Location l = node.location();
		if (l.lon() < minlon)
		  minlon = l.lon();
		if (l.lon() > maxlon)
		  maxlon = l.lon();
		if (l.lat() < minlat)
		  minlat = l.lat();
		if (l.lat() > maxlat)
		  maxlat = l.lat();
		//std::cout << nodes[nodes.size() - 1].lat() << " " << nodes[nodes.size() - 1].lon() << "\n";
	      }
	      
	      map[way.id()] = nodes;
	//}      
    }
};

const GLsizei WIDTH = 800, HEIGHT = 800;
double zoom = 1;
double width = WIDTH, height = HEIGHT;
double vx = 0, vy = 0, kx = 0, ky = 0;
double x = 0, y = 0;
bool nyom;
int window;
UtHandler uthandler;

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glMatrixMode (GL_PROJECTION);
    gluOrtho2D (-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2);
    //gluOrtho2D (0, WIDTH, 0, HEIGHT);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glPointSize(5.0f);
    glLineWidth(1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
}


void draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0,0,0);
    double halflon = (uthandler.maxlon + uthandler.minlon) / 2;
    double halflat = (uthandler.maxlat + uthandler.minlat) / 2;
    for (const auto& a : uthandler.map)
    {
	glBegin(GL_LINE_STRIP);
	
	for (const auto& b : a.second)
	{
	    glVertex2d(
		zoom * (x + vx + (b.lon() - halflon) * width),
		zoom * (y + vy + (b.lat() - halflat) * height)
	    );
	}
	
	glEnd();
    }
    

    glutSwapBuffers();
}

void keyPressed (unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
}

void processMouse( GLint button, GLint action, GLint xMouse , GLint yMouse )
{
    if (button == 3)
    {
	zoom *= 1.1;
    }
    else if (button == 4)
    {
	zoom /= 1.1;
    }
    
    if (action == GLUT_DOWN)
    {
	nyom = true;
	kx = xMouse;
	ky = HEIGHT - yMouse;
	vx = 0;
	vy = 0;
    }
    else if (action == GLUT_UP)
    {
	nyom = false;
	vx = 0;
	vy = 0;
	x += (xMouse - kx) / zoom;
	y += (height - yMouse - ky) / zoom;
    }

    glutPostRedisplay();
}

void processMouseActiveMotion( GLint xMouse, GLint yMouse )
{
    vx = (xMouse - kx) / zoom;
    vy = (height - yMouse - ky) / zoom;
    glutPostRedisplay();
}

int main (int argc, char** argv)
{
    if (argc != 2)
      return 1;
    
    osmium::io::File infile ( argv[1] );
    osmium::io::Reader reader ( infile, osmium::osm_entity_bits::all );

    osmium::handler::NodeLocationsForWays<osmium::index::map::SparseTable<osmium::unsigned_object_id_type, osmium::Location>>
	      node_locations ( uthandler.locations );

    osmium::apply ( reader, node_locations, uthandler );
    reader.close();
    
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (WIDTH, HEIGHT);
    window = glutCreateWindow("megjelenito");
    
    init();
    glutKeyboardFunc(keyPressed);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseActiveMotion);
    glutDisplayFunc(draw);

    glutMainLoop();
    
    google::protobuf::ShutdownProtobufLibrary();    
    return 0;
}

