//package mySrc.gameE;
import java.util.*;
import java.io.*;
import java.awt.*;

public abstract class Obstacle implements Serializable{
	
	private String description;
	//private Set points = new HashSet();
	protected Point reference;
	
	public Obstacle(int x, int y, String s){
		reference = new Point(x,y);
		description = s;
	}
		
	//public boolean addPoint(int x, int y){
	//	return points.add(new Point(x - reference.x,y-reference.y));
	//}
	
	public abstract boolean hit(int x, int y, int width, int height);
	
	public abstract boolean collision(Obstacle obs);
	
	public abstract void action();
	
	public void draw(Graphics g){
	}

	
}

