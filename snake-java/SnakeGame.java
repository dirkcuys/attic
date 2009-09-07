//package mySrc.games;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
//import mySrc.gameE.*;

interface MoveConst{
	
	public static final int STOP = 0;
	public static final int UP = 1;
	public static final int DOWN = 2;
	public static final int LEFT = 3;
	public static final int RIGHT = 4;
	
}

class Bounds{
	int x,
		y,
		width,
		depth;
	public Bounds(int x, int y, int width, int depth){
		this.x = x;
		this.y = y;
		this.width = width;
		this.depth = depth;
	}
}

abstract class Player implements MoveConst{
	
	protected int direc = STOP;
	protected Rectangle bnds;
	public boolean alive;
	public Point position;
	
	abstract public void draw(Graphics g);
	abstract public void move();
	abstract public void onHit(Obstacle obs);
	abstract public boolean collision(int x, int y);
	public void setDirection(int i){
		if(i < 5)
			direc = i;
	}
	public int getDirection(){
		return direc;
	}
	public void setBounds(Rectangle bnds){
		this.bnds = bnds;
	}
	
}

//abstract class Movable{}

class Snake extends Player{
	
	private java.util.List pointL = new ArrayList();
	// Remember that the last element of the array and list is not part 
	// of the snake, it's used in the draw() method to erase 
	// the last block.
	private Point[] pointA = new Point[1]; 
	private int changedirec;
 	
	public Snake(){
		position = new Point(100,100);
		pointL.add( new Point(100,100) );
		pointA = (Point[]) pointL.toArray(pointA);
		grow();
		grow();
		alive = true;
	}
	
	public void setDirection(int i){
		changedirec = i;
	}
	
	public void onHit(Obstacle obs){
		if (obs instanceof Apple){
			grow();
		}
		if (obs instanceof GameBorder){
			alive = false;
			direc = STOP;
		}
	}
	
	public void grow(){
		pointL.add( new Point(pointA[pointA.length-1]) );
		pointA = (Point[]) pointL.toArray(pointA);
	}
	
	public void draw(Graphics g){
		g.setColor(Color.black);
		int tmp = pointA.length - 1;
		g.clearRect(pointA[tmp].x,pointA[tmp].y,10,10);
		g.fillRect(pointA[0].x,pointA[0].y,10,10);
	}
	
	public boolean tailHit(){
		for(int a = 3; a < pointA.length; a++){
			if ( pointA[0].equals(pointA[a]) ){
				return true;
			}
		}
		return false;
	}
	
	public boolean collision(int x, int y){
		Point pnt = new Point(x,y);
		for(int a = 0; a < pointA.length; a++){
			if ( pnt.equals(pointA[a]) ){
				return true;
			}
		}
		return false;
	}
	
	public void move(){
		if(changedirec != super.getDirection()){
			super.setDirection(changedirec);
		}
		for(int a = pointA.length-1 ;a > 0; a--){
			pointA[a].x = pointA[a-1].x;
			pointA[a].y = pointA[a-1].y;
		} 
		switch (direc) {
			case UP : pointA[0].y -= 10; break;
			case DOWN : pointA[0].y += 10; break;
			case LEFT : pointA[0].x -= 10; break;
			case RIGHT : pointA[0].x += 10; break;
			default : break;
		}
		if(pointA[0].x < bnds.x)
			pointA[0].x = bnds.x + bnds.width;
		else if(pointA[0].y < bnds.y)
			pointA[0].y = bnds.y + bnds.height;
		else if(pointA[0].x > (bnds.x + bnds.width))
			pointA[0].x = bnds.x;
		else if(pointA[0].y > (bnds.y + bnds.height))
			pointA[0].y = bnds.y;
		position.setLocation(pointA[0]);
		if (tailHit()){
			alive = false;
		}
	}
}

abstract class Obstacle{
	
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

class PlayingField extends Canvas{
	
	private Player player;
	private javax.swing.Timer clock = new javax.swing.Timer(200, new TAL());
	private java.util.List obstacles = new ArrayList();
	private ScoreBoard score = null;
	private boolean drawObstacles = false;
	private Rectangle bnds;
	private boolean clearfield = false;
	private int speed = 20;
	private Actions actions;
	
	public PlayingField(Player p, Rectangle b){
		player = p;
		player.setBounds(b);
		bnds = b;
		setSize(b.width,b.height);
		clock.start();
		clock.setRepeats(true);
		actions = new Actions(this);
		//Window scoreboard = new Window(this);
		//scoreboard.add(score);
		//scoreboard.setVisible(true);
	}
	
	public PlayingField(Player p, int x, int y, int width, int depth){   // This constructer should be removed
		this(p,new Rectangle(x,y,width,depth));
		
	}
	
	public void setScoreBoard(ScoreBoard scr){
		score = scr;
	}
	
	public ScoreBoard getScoreBoard(){
		return score;
	}
	
	public Rectangle getBounds(){
		return bnds;
	}
	
	public void setBounds(Rectangle bounds){
		bnds = bounds;
	}
	
	public void addObstacle(Obstacle obs){
		obstacles.add(obs);
		drawObstacles = true;
	}
	
	public boolean removeObstacle(Obstacle obs){
		return obstacles.remove(obs);
		//the obstacle still has to be cleared from the screen
	}
	
	public java.util.List getObstacleList(){
		return obstacles;
	}
	
	public boolean setObstacleList(java.util.List obsL){
		Iterator it = obsL.iterator();
		while (it.hasNext()){
			Object o = it.next();
			if (!(o instanceof Obstacle))
				return false;
		}
		obstacles = obsL;
		return true;
	}
	
	public void pause(){
		clock.stop();
	}
	
	public void resume(){
		clock.start();
	}
	
	public Player getPlayer(){
		return player;
	}
	
	public void setPlayer(Player p){
		//super.repaint();
		//Still needs to erase the old player first;
		player = p;
		player.setBounds(bnds);
		
	}
	
	public void clearField(){
		clearfield = true;
	}
	
	public void setSpeed(int spd){
		speed = spd;
		clock.setDelay(speed);
	}
	
	public int getSpeed(){
		return speed;
	}

	public void newGameActions(){
		actions.newGameActions();
		clearField();
	}
	
	public void update(Graphics g){
		if (drawObstacles) {
			Iterator it = obstacles.iterator();
			while (it.hasNext()){
				Obstacle o = (Obstacle) it.next();
				o.draw(g); 
			}
			//drawObstacles = false;
		}
		if(clearfield){
			g.clearRect(0,0,getWidth(),getHeight());
			clearfield = false;
		}
		player.move();
		player.draw(g);
	}
	

	
	/*public void paint(Graphics g){
		if (drawObstacles) {
			Iterator it = obstacles.iterator();
			while (it.hasNext()){
				Obstacle o = (Obstacle) it.next();
				o.draw(g); 
			}
			drawObstacles = false;
		}
		super.paint(g);
	}*/
	
	class TAL implements ActionListener{
		public void actionPerformed(ActionEvent e){
			/*if(player.alive)
				repaint();
			Iterator it = obstacles.iterator();
			while (it.hasNext()){
				Obstacle o = (Obstacle) it.next();
				if(o.hit(player.position.x,player.position.y,10,10)){
					o.action();
					player.onHit(o);
					if(score != null){
						score.incScore(10);
						score.repaint();
					}
				}
			}*/
			actions.timedActions();
		}
	}
}

class Apple extends Obstacle{
	//private Point position;
	private Rectangle bnds;
	
	public Apple(Rectangle bounds){
		super(	(int)( Math.round(Math.random() * bounds.width/10)*10 + bounds.x ), 
				(int)( Math.round(Math.random() * bounds.height/10)*10 + bounds.y ),
				"Apple"
			 );
		bnds = bounds;
		System.out.println("x= " + reference.x + " y= "+ reference.y);
	}	
	
	public void move(){
		reference = new Point( 
								(int)( Math.round(Math.random() * bnds.width/10)*10 + bnds.x ), 
								(int)( Math.round(Math.random() * bnds.height/10)*10 + bnds.y ) 
							 );
	}
	
	public boolean hit(int x, int y, int width, int height){
		if(new Rectangle(reference.x,reference.y,10,10).intersects(x,y,width,height)){
			return true;
		}
		return false;
	}
	
	public void action(){
		move();
	}
	
	public boolean collision(Obstacle obs){
		return false;
	}
	
	public void draw(Graphics g){
		g.setColor(Color.red);
		g.drawOval(reference.x,reference.y,9,9);
		System.out.println("Drawed at ( " + reference.x + " ; "+ reference.y + " )");
	}
	
}

class GameBorder extends Obstacle{
	private Rectangle rect;
	
	public GameBorder(Rectangle bnds){
		super(1,1,"Border");
		rect = bnds;
	}
	
	public boolean hit(int x, int y, int width, int height){
		if (y > (rect.height + rect.y)) 
			return true;
		if (x > (rect.width + rect.x))
			return true;
		if (x < rect.x) 
			return true;
		if (y < rect.y) 
			return true;
		return false;
	}
	
	public boolean collision(Obstacle obs){
		return false;
	}
	
	public void action(){
	}
	
	public void draw(Graphics g){
		g.drawRect(rect.x, rect.y, rect.width, rect.height);
	}
}

class ScoreBoard extends Canvas{
	private int score = 0;
	
	public ScoreBoard(){
	}
	
	public int getScore(){
		return score;
	}
	
	public void setScore(int newscore){
		score = newscore;
	}
	
	public void incScore(int value){
		score += value;
	}
	
	public void reset(){
		score = 0;
	}

	public void paint(Graphics g){
		g.drawString("Score: " + score,10,10);
	}
}

class Actions {
	protected PlayingField field;
	protected Player player;
	protected java.util.List obstacleL;
	protected ScoreBoard score = null;
	
	public Actions(PlayingField field){
		this.field = field;
		player = field.getPlayer();
		obstacleL = field.getObstacleList();
		score = field.getScoreBoard();
	}
	
	public void timedActions(){
		if(player.alive)
			field.repaint();
		Iterator it = obstacleL.iterator();
		while (it.hasNext()){
			Obstacle o = (Obstacle) it.next();
			if(o.hit(player.position.x,player.position.y,10,10)){
				o.action();
				player.onHit(o);
				/*if(score != null){
					score.incScore(10);
					score.repaint();
				}*/
			}
		}
	}
	
	public void newGameActions(){
		player = field.getPlayer();
		obstacleL = field.getObstacleList();
		score = field.getScoreBoard();
	}
}

class AboutBox extends Frame{
	Button closeB = new Button("Close");
	Panel infopanel = new Panel();

	public AboutBox(String title){
		super(title);
		setBackground(SystemColor.menu);
		addWindowListener(new WindowAdapter(){
								public void windowClosing(WindowEvent e){
									dispose();
						   		}
							  }
						 );
		setLayout(new BorderLayout());
		closeB.addActionListener(new CBL());
		add(closeB,BorderLayout.SOUTH);
		add(infopanel,BorderLayout.CENTER);
	}
	public void paint(Graphics g){
		g.drawString("This game was coded by dc. I give all praise to Jesus Christ my saviour.",10,10);
	}
	
	class CBL implements ActionListener{
		public void actionPerformed(ActionEvent e){
			dispose();
		}
	}
}

class Options implements java.io.Serializable{
	private int gamespeed,
				noofapples;
	private Dimension fieldsize;
	
	public void setGameSpeed(int i){
		gamespeed = i;
	}
	
	public int getGameSpeed(){
		return gamespeed;
	}
	
	public void setNoOfApples(int i){
		noofapples = i;
	}
	
	public int getNoOfApples(){
		return noofapples;
	}
	
	public void setFieldSize(Dimension d){
		fieldsize = d;
	}
	
	public Dimension getFieldSize(){
		return fieldsize;
	}
	
}

public class SnakeGame extends JFrame{
	
	Snake player = new Snake();
	Apple apple = new Apple(new Rectangle(10,10,280,280));
	GameBorder border = new GameBorder(new Rectangle(10,10,289,289));
	PlayingField field = new PlayingField(player,0,0,300,300);
	ScoreBoard scoreboard = new ScoreBoard();
	JPanel panel = new JPanel();
	MenuBar defaultMenu = new MenuBar();
	Menu gameM = new Menu("Game");
	Menu infoM = new Menu("Info");
	MenuItem about = new MenuItem("About");
	MenuItem newGame = new MenuItem("New");
	MenuItem pause = new MenuItem("Pause");
	MenuItem resume = new MenuItem("Resume");
	
	SnakeGame(String title){
		super(title);
		addWindowListener(new WindowAdapter(){
								   public void windowClosing(WindowEvent e){
									    System.exit(0);
								   }
							  }
						 );
		//field.repaint();
		field.addKeyListener(new KL());
		field.addObstacle(apple);
		field.addObstacle(new Apple(new Rectangle(0,0,300,300)));
		field.addObstacle(border);
		field.setScoreBoard(scoreboard);
		addKeyListener(new KL());
		getContentPane().setLayout(new BorderLayout());
		getContentPane().add(panel,BorderLayout.CENTER);
		panel.setLayout(new BorderLayout());
		panel.add(field);
		//Window score = new Window(this);
		//score.add(scoreboard);
		//score.setSize(100,100);
		//score.setVisible(true);
		panel.setBorder(new EtchedBorder(EtchedBorder.LOWERED));
		newGame.addActionListener(new NGL());
		pause.addActionListener(new PL());
		resume.addActionListener(new RL());
		about.addActionListener(new ActionListener(){
										public void actionPerformed(ActionEvent e){
											AboutBox aboutbox = new AboutBox("Snake");
											aboutbox.setSize(200,200);
											aboutbox.setVisible(true);
										}
									}
							   );
		gameM.add(newGame);
		gameM.add(pause);
		infoM.add(about);
		defaultMenu.add(gameM);
		defaultMenu.add(infoM);
		setMenuBar(defaultMenu);
	}
	
	class KL extends KeyAdapter{  
		public void keyPressed(KeyEvent e){
			if((e.getKeyCode() == 38) && (player.getDirection() != MoveConst.DOWN) /*KeyEvent.VK_KP_UP*/){
				player.setDirection(MoveConst.UP);
			}else if ((e.getKeyCode() == 40) && (player.getDirection() != MoveConst.UP) /*KeyEvent.VK_KP_DOWN*/){
				player.setDirection(MoveConst.DOWN);
			}else if ((e.getKeyCode() == 37) && (player.getDirection() != MoveConst.RIGHT) /*KeyEvent.VK_KP_LEFT*/){
				player.setDirection(MoveConst.LEFT);
			}else if ((e.getKeyCode() == 39) && (player.getDirection() != MoveConst.LEFT) /*KeyEvent.VK_KP_RIGHT*/){
				player.setDirection(MoveConst.RIGHT);
			}
		}
	}
	
	class PL implements ActionListener{
		public void actionPerformed(ActionEvent e){
			repaint();
			field.pause();
			gameM.remove(pause);
			gameM.add(resume);				
		}
	}
	
	class RL implements ActionListener{
		public void actionPerformed(ActionEvent e){
			field.resume();
			gameM.remove(resume);
			gameM.add(pause);
							
		}
	}
	
	class NGL implements ActionListener{
		public void actionPerformed(ActionEvent e){
			player = new Snake();
			field.setPlayer(player);
			field.removeObstacle(apple);
			apple = new Apple(new Rectangle(10,10,280,280));
			field.addObstacle(apple);
			scoreboard.reset();
			field.newGameActions();
		}
	}
	
	public static void main(String[] args){
		SnakeGame game = new SnakeGame("Snake v0.00000001");
		game.setSize(350,350);
		game.setVisible(true);
		//game.field.addObstacle(game.apple);
	}
}
