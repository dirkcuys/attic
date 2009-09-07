package mySrc.gameE;
import java.util.*;
import java.io.*;
import java.awt.*;

class OPoint implements Serializable{
	
	static int cnt = 0;
	private int id;
	public int x, y;	
	
	public OPoint(int x, int y){
		this.x = x;
		this.y = y;
		id = ++cnt;
	}

	public boolean eqauls(Object o) {
		return ( (o instanceof Point) && (x == ((Point)o).x) 
		       && (y == ((Point)o).y) );
	}	
	public int hashCode() { 
		return id; 
	}	
	public String toString() {
		return "x= " + x + " ; y=" + y;
	}	
}

abstract class Obstacle implements Serializable{
	
	private String description;
	private Set points = new HashSet();
	private OPoint reference;
	
	public Obstacle(int x, int y, String s){
		reference = new OPoint(x,y);
		description = s;
	}
		
	public boolean addPoint(int x, int y){
		return points.add(new OPoint(x - reference.x,y-reference.y));
	}
	
	public boolean hit(int x, int y, int width, int height){
		return false;
	}
	
	abstract void action();
	
	public void draw(Graphics g){
	}

	
}

public class Scenery{
	
}
