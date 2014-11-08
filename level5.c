
#include<stdio.h>
#include<stdlib.h>
#include<eggx.h>
#include<string.h> 

int win, i, a=1;
int type, code;
int time=100;//0.1秒分
int dy=10,dx=5;// 速度変化
int dh=5, dw=5, dr=5;// 大きさ変化
int count=0;// 時間
int kosu=2;//最初の個数
int p=0;//時間の初期値
char k[3];//時間を表記するための文字列
double s, t;// NULL
double x=200.0;//棒人間のx座標の初期値
double x_sq[8],y_sq[8],w_sq[8],h_sq[8];// 四角の座標、縦幅、横幅
double x_circ[8], y_circ[8], r_circ[8];// 円の座標、半径
double harf_y=650;//レベル４で出る画面半分の大きさの円のy座標
double harf_x;//↑の円が右側か左側か
double defence_x, defence_y=570, defence_w=40.0;//防護壁
double defence_h1, defence_h2, defence_h3;  //防護壁
double all_x=200.0, all_y=900;//レベル５で出る画面全体の大きさの円のx,y座標
double explo_x, explo_y1=250.0, explo_y2=250.0;//爆発後っぽく見せる線

//プロトタイプ宣言
void game(void);//ゲーム内容
void STA(void);//スタート画面
void input(void);//入力待ち
void syoki_buttai(void);//物体の初期設定
void human(void);//棒人間
void atari(void);//当たり判定
void bougoheki(void);//防護壁
void level(void);//レベル表記
void timecount(void);//タイムの表記
void lastline(void);//最後の線
void LEVEL2(void);//レベル２以上の動作
void LEVEL4(void);//レベル４の動作
void LEVEL5(void);//レベル５の動作

//--------------------------------メイン--------------------------------------
int main(void){
  int seed;
  
  //ランダム関数設定
  printf("適当に何か入力してreturnを押してください >> ");
  scanf("%d", &seed);
  srand(seed);  
  
  //落ちてくる物体の初期値設定
  syoki_buttai();
    
  //画面表示
  win=gopen(400, 500);
  winname(win, "DODGE GAME");
  
  STA();//スタート画面 
  if(ggetevent(&type, &code, &s, &t)==win){//入力待ち
    if(code==0x073){// sキーを入力した場合
      game();
    }
    else if(code == 0x075){// fキーを押した場合
      return 0;
    }
  }
  return 0;
}

//---------------------------サブルーチン-------------------------

void game(void){//ゲーム内容
  int j;
  
  gsetnonblock(ENABLE);//ノンブロッキングモードに
  while(1){	
    gclr(win);//画面消去
   
    //防護壁のx座標設定（レベル５になるまで値変化）
    defence_x=rand()%400;
    if(defence_x > 320){//x座標が340を超えたら340以下の数を減らす
      defence_x -= rand()%320;
    }
    LEVEL5();//レベル５の動作
    
    newpen(win, 1);
    level();// レベルの表記
    timecount();//時間の表記
    human();// 棒人間
    input();//入力待ち
    
    for(i=0; i<kosu; i++){
      j = rand() % 16;// 色はランダム
      if(j < 3) j = j + rand() % 10 + 3;//色は３〜１５のどれか 
      if(a > 2){	 //レベル３、４でときどき黒になって消えて見える
	if(rand()%5 == 0) j = 0;
      }
      newpen(win, j);
      fillrect(win, x_sq[i], y_sq[i], w_sq[i], h_sq[i]);//四角	 
      fillcirc(win, x_circ[i], y_circ[i], r_circ[i], r_circ[i]);//円	        
      LEVEL4();//レベル４のみ出る円（半分を埋め尽くす円）
      
      //落下速度dx,dy
      y_sq[i] -= dy;
      y_circ[i] -= dy;
      
      LEVEL2();//レベル２以上で変化させる
    }
    atari();//当たり判定
    
    msleep(time);// 0.1秒止める
    
    count++;
    if(count/10 == 20*a){// カウント１０回＝約１秒とする
      dy += rand()%2+1;//速度変化は毎回変わる
      dx += rand()%2;//揺れる大きさ
      a++;//レベル
      kosu+=rand()%2+1;//増やす個数は毎回変わる 
      dh+=3;//通常四角の縦幅変化量
      dw+=3;//通常四角の横幅変化量
      dr+=3;//通常円の半径の変化量
    }
  }    
}

void STA(void){// スタート画面
  drawstr(win, 170, 300, 24, 0.0, "SATRT");
  drawstr(win, 130, 200, 24, 0.0, "start s-key");
  drawstr(win, 130, 100, 24, 0.0, "finish f-key");
}

void input(void){//入力
  if(ggetevent(&type, &code, &s, &t) == win){//キー入力待ち
    if(code == 0x01c){// 右キー
      x += 8.0;
    }
    else if(code == 0x01d){// 左キー
      x -= 8.0;
    }    
    if(x < 5.0 || x > 395.0){// 端に行くと真ん中に強制的に戻る
      x = 200.0;
    }
  }
}

void syoki_buttai(void){//初期値設定
  for(i=0; i<8; i++){
    x_sq[i] = rand() % 400;
    y_sq[i] = rand() % 100+500;
    w_sq[i] = rand() % 20+dw;
    h_sq[i] = rand() % 20+dh;
    x_circ[i] = rand() % 400;
    y_circ[i] = rand() % 150 + 500;
    r_circ[i] = rand() % 20+dr;
  }
  //防護壁の設定
  defence_h1=defence_h3=80.0;
  defence_h2=20.0;
  
  //半分を埋め尽くす円の位置の設定
  if(rand()%2==0){
    harf_x=0;
  }
  else if(rand()%2==1){
    harf_x=400;
  }
}

void human(void){// 棒人間
  newpen(win, 1);
  fillarc(win,x, 25.0, 5.0, 5.0, 0.0, 360.0, 1);
  line(win, x, 5.0, PENUP);
  line(win, x, 20.0, PENDOWN);
  line(win, x, 5.0, PENUP);
  line(win, x-5.0, 0.0, PENDOWN);
  line(win, x, 5.0, PENUP);
  line(win, x+5.0, 0.0, PENDOWN);
  line(win, x, 13.0, PENUP);
  line(win, x-5.0, 17.0, PENDOWN);
  line(win, x, 13.0, PENUP);
  line(win, x+5.0, 17.0, PENDOWN);
}

void atari(void){//当たり判定
  //----------------四角の当たり判定-------------------
  for(i=0; i<kosu; i++){//通常の四角
    if(
       ( (x > x_sq[i] && x < x_sq[i]+w_sq[i]) &&
	 (25 > y_sq[i]-5 && 25 < y_sq[i]+h_sq[i]+5) ) ||
       
       ( (x_sq[i]-5 < x && x < x_sq[i]+w_sq[i]+5) &&
	 (y_sq[i] < 25 && 25 < y_sq[i]+h_sq[i]) ) ||
       
       ( (x_sq[i]-x)*(x_sq[i]-x)+
	 (y_sq[i]+w_sq[i]-25)*(y_sq[i]+w_sq[i]-25) < 25) ||
       
       ( (x_sq[i]+w_sq[i]-x)*(x_sq[i]+w_sq[i]-x)+
	 (y_sq[i]+h_sq[i]-25)*(y_sq[i]+h_sq[i]-25) < 25 ) 
       ){
      gclr(win);
      newpen(win, 1);
      drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
      msleep(time*30);
      exit(0);
    }  
  }
  //------------------ここまで---------------------
  
  //----------------丸の当たり判定--------------------
  for(i=0; i<kosu; i++){//通常の円
    if( (r_circ[i]+5)*(r_circ[i]+5) >
	(x-x_circ[i])*(x-x_circ[i])+(25-y_circ[i])*(25-y_circ[i]) ){
      gclr(win);
      newpen(win, 1);
      drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
      msleep(time*30);
      exit(0);	 
    }
  }
  
  //--------------ここまで-------------------------
  
  if(//左の四角
     ( (x > defence_x && x < defence_x+defence_w) &&
       (25 > defence_y-5 && 25 < defence_y+defence_h1+5) ) ||
     
     ( (defence_x-5 < x && x < defence_x+defence_w+5) &&
       (defence_y < 25 && 25 < defence_y+defence_h1) ) ||
     
     ( (defence_x-x)*(defence_x-x)+(defence_y-25)*(defence_y-25)
       < 25 ) ||
     
     ( (defence_x-x)*(defence_x-x)+
       (defence_y+defence_h1-25)*(defence_y+defence_h1-25) < 25 ) ||
     
     ( (defence_x+defence_w-x)*(defence_x+defence_w-x)+
       (defence_y-25)*(defence_y-25) < 25 ) ||
     
     ( (defence_x+defence_w-x)*(defence_x+defence_w-x)+
       (defence_y+defence_h1-25)*(defence_y+defence_h1-25) < 25 )
     ){
    gclr(win);
    newpen(win, 1);
    drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
    msleep(time*30);
    exit(0);
  } 
  
  if(//真ん中の四角
     ( (x > defence_x+40 && x < defence_x+40+defence_w) &&
       (25 > defence_y+60-5 && 25 < defence_y+60+defence_h2+5) ) ||
     
     ( (defence_x+40-5 < x && x < defence_x+40+defence_w+5) &&
       (defence_y+60 < 25 && 25 < defence_y+60+defence_h2) ) ||
     
     ( (defence_x+40-x)*(defence_x+40-x)+
       (defence_y+60-25)*(defence_y+60-25) < 25 ) ||
     
     ( (defence_x+40-x)*(defence_x+40-x)+
       (defence_y+60+defence_h2-25)*(defence_y+60+defence_h2-25)
       < 25 ) ||
     
     ( (defence_x+40+defence_w-x)*(defence_x+40+defence_w-x)+
       (defence_y+60-25)*(defence_y+60-25) < 25 ) ||
     
     ( (defence_x+40+defence_w-x)*(defence_x+40+defence_w-x)+
       (defence_y+60+defence_h2-25)*(defence_y+60+defence_h2-25)
       < 25 )
     ){
    gclr(win);
    newpen(win, 1);
    drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
    msleep(time*30);
    exit(0);
  } 
  
  if(//右の四角
     ( (x > defence_x+80 && x < defence_x+80+defence_w) &&
       (25 > defence_y-5 && 25 < defence_y+defence_h3+5) ) ||
     
     ( (defence_x+80-5 < x && x < defence_x+80+defence_w+5) &&
       (defence_y < 25 && 25 < defence_y+defence_h3) ) ||
     
     ( (defence_x+80-x)*(defence_x+80-x)+
       (defence_y-25)*(defence_y-25) < 25 ) ||
     
     ( (defence_x+80-x)*(defence_x+80-x)+
       (defence_y+defence_h3-25)*(defence_y+defence_h3-25) < 25 ) ||
     
     ( (defence_x+80+defence_w-x)*(defence_x+80+defence_w-x)+
       (defence_y-25)*(defence_y-25) < 25 ) ||
     
     ( (defence_x+80+defence_w-x)*(defence_x+80+defence_w-x)+
       (defence_y+defence_h3-25)*(defence_y+defence_h3-25) < 25 )
     ){
    gclr(win);
    newpen(win, 1);
    drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
    msleep(time*30);
    exit(0);
  } 
}

void bougoheki(void){//防護壁の表記
  newpen(win,1);
  fillrect(win,defence_x, 0.0, defence_w, defence_h1);
  fillrect(win,defence_x+40.0, 60.0, defence_w, defence_h2);
  fillrect(win,defence_x+80.0, 0.0, defence_w, defence_h3);
}

void level(void){// レベルの表記
  if(a==1) drawstr(win, 5, 485, 18, 0.0, "LV1");// 初期レベル１
  if(a==2) drawstr(win, 5, 485, 18, 0.0, "LV2");
  if(a==3) drawstr(win, 5, 485, 18, 0.0, "LV3");
  if(a==4) drawstr(win, 5, 485, 18, 0.0, "LV4");
  if(a==5) drawstr(win, 5, 485, 18, 0.0, "LV5");// 最高レベル５
}

void timecount(void){//タイムの表記
  if(count % 10 == 0){
    p++;
  }
  sprintf(k, "%d", p);
  newpen(win, 1);
  drawstr(win, 6, 468, 18, 0.0, k);
}

void lastline(void){//最後の線
  for(explo_x=1; explo_x<400; explo_x= explo_x+5){  
    line(win, explo_x, 250, PENUP);
    line(win, explo_x, explo_y1, PENDOWN);
    line(win, explo_x, 250, PENUP);
    line(win, explo_x, explo_y2, PENDOWN);
  }
  
  explo_y1-=5.0;//下に伸びる
  explo_y2+=5.0;//上に伸びる  
}

void LEVEL2(void){//レベル２以上の動作
  if(a>1){ //レベル２以上で左右に揺れる
    if(rand()%4 == 0){
      x_sq[i]-=dx;
      x_circ[i]-=dx; 
    }
    else if(rand()%4 == 2){
      x_sq[i]+=dx;
      x_circ[i]+=dx;
    }
  }
  
  //毎回の値の変化
  if(y_sq[i] < -5){
    x_sq[i] = rand() % 400;
    y_sq[i] = rand() % 100+500;
    w_sq[i] = rand() % 20+5;
    h_sq[i] = rand() % 20+5; 
  }
  if(y_circ[i] < -5){
    x_circ[i] = rand() % 400;
    y_circ[i] = rand() % 150 + 500;
    r_circ[i] = rand() % 20+5;
  }
}


void LEVEL4(void){//レベル４に出る半分を埋め尽くす円
  if(a==4){//半分を埋め尽くす円
    newpen(win, 4);//色は青
    fillcirc(win, harf_x, harf_y, 200.0, 200.0);
    if(harf_x == 400){//右側の場合の当たり判定	   
      if( (200+5)*(200+5) >
	  (x-400)*(x-400)+(25-harf_y)*(25-harf_y) ){
	gclr(win);
	newpen(win, 1);
	drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
	msleep(time*30);
	exit(0);	 
      }
    }
    if(harf_x == 0){//左側の場合の当たり判定
      if( (200+5)*(200+5) >
	  (x-0)*(x-0)+(25-harf_y)*(25-harf_y) ){
	gclr(win);
	newpen(win, 1);
	drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
	msleep(time*30);
	exit(0);
      }
    }
    harf_y-=5;//落下速度
  }
}

void LEVEL5(void){//レベル５の動作
  if(a==5){
    while(1){
      gclr(win);
      human();//棒人間
      input();//入力待ち
      
      newpen(win, 1);//色は白
      //防護壁表記
      fillrect(win,defence_x, defence_y, defence_w, defence_h1);
      fillrect(win,defence_x+40.0, defence_y+60.0, defence_w, defence_h2);
      fillrect(win,defence_x+80.0, defence_y, defence_w, defence_h3);
      defence_y -= 15.0;//速度
      
      atari();//当たり判定
      //一番下まで落ちたら    
      if(defence_y == 0){
	while(1){
	  gclr(win);//画面消去
	  human();//棒人間
	  input();//入力待ち
	  bougoheki();//防護壁	
	  atari();//当たり判定
	  newpen(win, 2);//太陽の色は赤で
	  fillcirc(win,all_x, all_y, 200, 200);//太陽
	  all_y-=5.0;//落下速度
	  
	  //太陽が防護壁に当たったら
	  if(all_y==280.0){
	    while(1){
	      gclr(win);//画面消去
	      human();//棒人間
	      input();//入力待ち
	      bougoheki();//防護壁
	      atari();//当たり判定
	      newpen(win, 2);//線は赤
	      
	      //太陽ぶつかった後の線
	      lastline();
	      msleep(time);
	      
	      if(explo_y1<0){//下に伸びた線が０以下になったら
		if(defence_x+40 < x && x < defence_x+80){//中にいれば
		  newpen(win, 1);
		  drawstr(win, 180, 250, 24, 0, "SAFE");
		  msleep(time*10);
		  gclr(win);
		  newpen(win, 1);
		  drawstr(win, 100, 300, 24, 0.0, "CONGRATULATIONS!");
		  drawstr(win, 170, 250, 24, 0.0, "CLEAR");
		  msleep(time*30);
		  exit(0);
		}
		else {//外にいれば
		  newpen(win, 1);
		  drawstr(win, 190, 250, 24, 0, "OUT");
		  msleep(time*10);
		  gclr(win);
		  newpen(win, 1);
		  drawstr(win, 150, 250, 24, 0.0, "GAMEOVER");
		  msleep(time*30);
		  exit(0);
		}
	      }
	    }
	  }
	  msleep(time);
	} 
      }
      msleep(time);   
    } 
  }
}
