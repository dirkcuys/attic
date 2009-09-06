import java.awt.*;
import java.awt.event.*;

class Slot extends Canvas{
	public int color = 0;
	
	public Slot(){
		setSize(40,40);
	}
	
	public void paint(Graphics g){
		int w = getWidth()-10,
			h = getHeight()-10;
		g.setColor(Color.black);
		g.drawOval(5,5,w,h);
		if(color == 1){
			g.setColor(Color.black);			
			g.fillOval(5,5,w,h);
		}else if(color == 2){
			g.setColor(Color.red);			
			g.fillOval(5,5,w,h);
		}
	}
}

class GameBoard extends Panel {
	public static final int BLACK = 1,
							RED = 2,
							H_WIN = 3,
							V_WIN = 4,
							D_WIN = 5;
	private int activecolor = RED;
	private boolean gameactive;
							
	Slot[][] slots;
	private Dimension size;
	
	public GameBoard(int x , int y){
		super(new GridLayout(y +1,x));		
		gameactive = true;
		size = new Dimension(x,y);
		slots = new Slot[x][y];
		for(int cntX = 0; cntX < slots.length;cntX++){
			for(int cntY= 0;cntY < slots[cntX].length;cntY++){
				slots[cntX][cntY] = new Slot();
			}
		}
		
		for(int cnt = 0; cnt < x; cnt ++){
			add(new CButton(cnt));
		}
		
		for(int cntY=slots[0].length-1;cntY >= 0 ;cntY--){
			for(int cntX = 0; cntX < slots.length;cntX++){
				add(slots[cntX][cntY]);
			}
		}
		setSize(400,400);
		
	}
	
	public boolean putCoin(int x){
		if (gameactive != true)
			return false;
		for (int cnt = 0; cnt < slots[x].length; cnt++){
			if(slots[x][cnt].color == 0){
				slots[x][cnt].color = activecolor;
				repaint();
				System.out.println("Coin put at (" + x + ";" + cnt +")");
				if (checkWin()){
					gameactive = false;
					if (activecolor == RED){
						Frame winw = new WinWindow("Red");
					}else{
						Frame winw = new WinWindow("Black");
					}
				}
				if (activecolor == RED){
					activecolor = BLACK;
				}else{
					activecolor = RED;
				}
				return true;
			}
		}
		new IllegalMoveWindow();
		return false;
	}
	
	private boolean vWin(int x, int y){
		if(y+3 > slots[x].length-1)
			return false;
		int color = slots[x][y].color;
		for(int ty = y+1; ty < y+4; ty++){
			if(slots[x][ty].color != color)
				return false;
		}
		System.out.println("Vertical win");
		return true;
	}
	
	private boolean hWin(int x, int y){
		if(x+3 > slots.length-1)
			return false;
		int color = slots[x][y].color;
		for(int tx = x+1; tx < x+4; tx++){
			if(slots[tx][y].color != color)
				return false;
		}
		System.out.println("Horizontal win");
		return true;
	}
	
	private boolean dUpWin(int x, int y){
		if((x+3 > slots.length-1) || (y+3 > slots[x].length-1))
			return false;
		int color = slots[x][y].color;
		for (int cnt = 1; cnt < 4; cnt++){
			if(slots[x+cnt][y+cnt].color != color)
				return false;
		}
		System.out.println("Diagnal Upwards win");
		return true;
	}
	
	private boolean dDownWin(int x, int y){
		if((x+3 > slots.length-1) || (y-3 < 0))
			return false;
		int color = slots[x][y].color;
		for (int cnt = 1; cnt < 4; cnt++){
			if(slots[x+cnt][y-cnt].color != color)
				return false;
		}
		System.out.println("Diagnal Downwards win");
		return true;
	}
	
	public boolean checkWin(){
		for(int cntX = 0; cntX < slots.length;cntX++){
			for(int cntY= 0;cntY < slots[cntX].length;cntY++){
				if(slots[cntX][cntY].color != 0){
					//check for a vertical win
					if (vWin(cntX,cntY))
						return true;
					//check for a horizontal win
					if (hWin(cntX,cntY))
						return true;
					//check for a diagnal win
					if (dUpWin(cntX,cntY))
						return true;
					if (dDownWin(cntX,cntY))
						return true;
				}
			}
		}
		return false;	
	}
	
	public void paint(Graphics g){
		drawBoard(g);
	}
	
	public void drawBoard(Graphics g){
		for(int cntX = 0; cntX < slots.length;cntX++){
			for(int cntY= 0;cntY < slots[cntX].length;cntY++){
				slots[cntX][cntY].repaint();
			}
		}
	}
	
	public void reset(){
		int x = size.width;
		int y = size.height;
		removeAll();
		setLayout(new GridLayout(y +1,x));
		gameactive = true;
		//slots = new Slot[x][y];
		for(int cntX = 0; cntX < slots.length;cntX++){
			for(int cntY= 0;cntY < slots[cntX].length;cntY++){
				slots[cntX][cntY] = new Slot();
			}
		}
		
		for(int cnt = 0; cnt < x; cnt ++){
			add(new CButton(cnt));
		}
		
		for(int cntY=slots[0].length-1;cntY >= 0 ;cntY--){
			for(int cntX = 0; cntX < slots.length;cntX++){
				add(slots[cntX][cntY]);
			}
		}
		repaint();
		validate();
	}
	
	class CButton extends Button{
		int bx;
		public CButton(int px){
			bx = px;
			addActionListener(new ActionListener(){
									public void actionPerformed(ActionEvent e){
										putCoin(bx);
										
									}
								});
		}									
	}
	
}

class Checkers extends Frame{
	MenuBar mb = new MenuBar();
	Menu game = new Menu("Game");
	MenuItem newgame = new MenuItem("New game");
	public GameBoard gb = new GameBoard(7,7);
	
	public Checkers(){
		super("Checkers");
		newgame.addActionListener(new ActionListener(){
										public void actionPerformed(ActionEvent e){
											gb.reset();
											repaint();
										}
									});
		game.add(newgame);
		mb.add(game);
		
		setMenuBar(mb);
		add(gb);
		addWindowListener(	new WindowAdapter(){
								public void windowClosing(WindowEvent e){
									System.exit(0);
								}
							}
						);
		setSize(400,500);
	}

	public static void main(String[] args){
		Checkers chk = new Checkers();
		
		chk.setVisible(true);
	}
}

class WinWindow extends Frame{
	Frame self = this;
	String winner;
	public WinWindow(String who){
		super("Game won!");
		winner = who;
		setLayout(new FlowLayout());
		addWindowListener(	new WindowAdapter(){
								public void windowClosing(WindowEvent e){
									self.dispose();
								}
							}
						);
		Button cb = new Button("Close");
		cb.addActionListener(new ActionListener(){
								public void actionPerformed(ActionEvent e){
									self.dispose();
								}
							});
		add(new Label(who + " won the game"));
		add(cb);
		setSize(150,100);
		setBackground(SystemColor.menu);
		setVisible(true);
	}
	
	public void paint(Graphics g){
	}
}

class IllegalMoveWindow extends Frame{
	Frame self = this;
	public IllegalMoveWindow(){
		super("IllegalMove");
		setLayout(new FlowLayout());
		addWindowListener(	new WindowAdapter(){
								public void windowClosing(WindowEvent e){
									self.dispose();
								}
							}
						);
		Button cb = new Button("Close");
		cb.addActionListener(new ActionListener(){
								public void actionPerformed(ActionEvent e){
									self.dispose();
								}
							});
		add(new Label("That move is not allowed"));
		add(cb);
		setSize(170,90);
		setBackground(SystemColor.menu);
		setVisible(true);
	}
	
	public void paint(Graphics g){
	}
}
