package koppany;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.Vector;
import java.util.Map;
import java.util.HashMap;

import java.awt.ItemSelectable;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import java.awt.BorderLayout;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.util.Collections;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JComboBox;
import javax.swing.event.MouseInputListener;

import org.jxmapviewer.JXMapViewer;
import org.jxmapviewer.OSMTileFactoryInfo;
import org.jxmapviewer.input.CenterMapListener;
import org.jxmapviewer.input.PanKeyListener;
import org.jxmapviewer.input.PanMouseInputListener;
import org.jxmapviewer.input.ZoomMouseWheelListenerCursor;
import org.jxmapviewer.viewer.DefaultTileFactory;
import org.jxmapviewer.viewer.GeoPosition;
import org.jxmapviewer.viewer.LocalResponseCache;
import org.jxmapviewer.viewer.TileFactoryInfo;
import org.jxmapviewer.viewer.DefaultWaypoint;

import org.jxmapviewer.viewer.Waypoint;
import org.jxmapviewer.viewer.WaypointPainter;
import org.jxmapviewer.viewer.WaypointRenderer;
import org.jxmapviewer.painter.CompoundPainter;
import org.jxmapviewer.painter.Painter;

import java.awt.Image;
import javax.swing.ImageIcon;
import java.util.Scanner;
import java.awt.geom.Point2D;
import java.awt.Graphics2D;


/**
 * A simple sample application that shows
 * a OSM map of Europe
 * @author Martin Steiger
 */
public class Koppany
{
    /**
     * @param args the program args (ignored)
     */

    public static void main(String[] args)
    {
            // Create a TileFactoryInfo for OpenStreetMap
            TileFactoryInfo info = new OSMTileFactoryInfo();
            DefaultTileFactory tileFactory = new DefaultTileFactory(info);
            tileFactory.setThreadPoolSize(8);

            // Setup local file cache
            File cacheDir = new File(System.getProperty("user.home") + File.separator + ".jxmapviewer2");
            LocalResponseCache.installResponseCache(info.getBaseURL(), cacheDir, false);

            // Setup JXMapViewer
            final JXMapViewer mapViewer = new JXMapViewer();
            mapViewer.setTileFactory(tileFactory);

            List<GeoPosition> track = new ArrayList<GeoPosition>();
            List<Waypoint> trackt = new ArrayList<Waypoint>();

            Scanner input = new Scanner(System.in);  

            final Map<String, Vector<Double[]>> t = new HashMap<String, Vector<Double[]>>();

            String nev = "";
            Vector<Double[]> m = null;
            while(input.hasNext())
            {
                String line = input.nextLine();
                String [] st = line.split(";");

                if (st.length == 2)
                {
                    Double[] tmp = new Double[2];
                    tmp[0] = Double.parseDouble(st[0]);
                    tmp[1] = Double.parseDouble(st[1]);

                    m.add(tmp);
                }
                else
                {
                    if (m != null)
                    {
                        if (t.containsKey(nev))
                            nev += "v";
                        t.put(nev, m);
                    }
                    
                    m = new Vector<Double[]>();
                    nev = line;
                }
            }          

            if (m != null)
            {
                if (t.containsKey(nev))
                    nev += "v";
                t.put(nev, m);               
                
                for(Double[] tmp : m)
                {
                    trackt.add(new DefaultWaypoint(tmp[0], tmp[1]));
                    track.add(new GeoPosition(tmp[0], tmp[1]));
                }
            }

            // Create a track from the geo-positions
            final RoutePainter routePainter = new RoutePainter(track);

            // Set the focus
            mapViewer.zoomToBestFit(new HashSet<GeoPosition>(track), 0.9);
            mapViewer.setZoom(2);
            mapViewer.setAddressLocation(trackt.get(0).getPosition());

            // Create waypoints from the geo-positions
            Set<Waypoint> waypoints = new HashSet<Waypoint>(trackt);

            final WaypointPainter waypointPainter = new WaypointPainter();
            waypointPainter.setWaypoints(waypoints);		
            
            
            final JFrame frame = new JFrame();
            
            //ClassLoader classLoader = frame.getClass().getClassLoader();
            //final Image wp = new ImageIcon(Koppany.class.getResource("ikon.png")).getImage();
            
            final Image wp = new ImageIcon(Koppany.class.getResource("ikon.png")).getImage();
            
            waypointPainter.setRenderer(new WaypointRenderer<Waypoint>() {
                //final Image wp = new ImageIcon(getClass().getResource("resources/ikon.png")).getImage();
                @Override
                public void paintWaypoint(java.awt.Graphics2D g2d, JXMapViewer jXMapV, Waypoint w) {
                    Point2D point = jXMapV.getTileFactory().geoToPixel(
                            w.getPosition(), jXMapV.getZoom());

                    g2d.drawImage(wp, (int) point.getX() - wp.getWidth(jXMapV) / 2,
                            (int) point.getY() - wp.getHeight(jXMapV), null);
                }
            });
            
            
            List<Painter<JXMapViewer>> painters = new ArrayList<Painter<JXMapViewer>>();
            painters.add(routePainter);
            painters.add(waypointPainter);

            CompoundPainter<JXMapViewer> painter = new CompoundPainter<JXMapViewer>(painters);
            mapViewer.setOverlayPainter(painter);


            // Add interactions
            MouseInputListener mia = new PanMouseInputListener(mapViewer);
            mapViewer.addMouseListener(mia);
            mapViewer.addMouseMotionListener(mia);

            mapViewer.addMouseListener(new CenterMapListener(mapViewer));
            mapViewer.addMouseWheelListener(new ZoomMouseWheelListenerCursor(mapViewer));
            mapViewer.addKeyListener(new PanKeyListener(mapViewer));

            mapViewer.setOverlayPainter(painter);            
            
            Vector<String> items = new Vector<String>();

            for (String a : t.keySet())
                items.add(a);
            
            Collections.sort(items);

            final JComboBox combobox = new JComboBox(items);
            combobox.setMaximumRowCount(10);            

            // Display the viewer in a JFrame
            frame.setLayout(new BorderLayout());
            frame.setSize(800, 600);

            frame.add(mapViewer);
            frame.add(combobox, BorderLayout.NORTH);

            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setVisible(true);               
            
            combobox.addItemListener(new ItemListener()
            {
                @Override
                public void itemStateChanged(ItemEvent itemEvent)
                {
                    if (ItemEvent.SELECTED == itemEvent.getStateChange())
                    {
                    //System.out.println("Item: " + itemEvent.getItem());
                        String nev = (String) itemEvent.getItemSelectable().getSelectedObjects()[0];
                        
                        List<Waypoint> trackt = new ArrayList<Waypoint>();
                        List<GeoPosition> track = new ArrayList<GeoPosition>();
                        
                        for(Double[] tmp : t.get(nev))
                        {
                            trackt.add(new DefaultWaypoint(tmp[0], tmp[1]));
                            track.add(new GeoPosition(tmp[0], tmp[1]));
                        }
                        
                        
                        Set<Waypoint> waypoints = new HashSet<Waypoint>(trackt);
                        
                        waypointPainter.setWaypoints(waypoints);
                        routePainter.setTrack(track);
                        
                       frame.revalidate();
                       frame.repaint();
                    }
                }
            });            
            

            mapViewer.addPropertyChangeListener("zoom", new PropertyChangeListener()
            {
                    @Override
                    public void propertyChange(PropertyChangeEvent evt)
                    {
                            updateWindowTitle(frame, mapViewer);
                    }
            });

            mapViewer.addPropertyChangeListener("center", new PropertyChangeListener()
            {
                    @Override
                    public void propertyChange(PropertyChangeEvent evt)
                    {
                            updateWindowTitle(frame, mapViewer);
                    }
            });

            updateWindowTitle(frame, mapViewer);
    }

    protected static void updateWindowTitle(JFrame frame, JXMapViewer mapViewer)
    {
            double lat = mapViewer.getCenterPosition().getLatitude();
            double lon = mapViewer.getCenterPosition().getLongitude();
            int zoom = mapViewer.getZoom();

            frame.setTitle(String.format("JXMapviewer2 Example 3 (%.2f / %.2f) - Zoom: %d", lat, lon, zoom)); 
    }	
	
}
