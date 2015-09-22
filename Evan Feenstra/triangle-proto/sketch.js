var x,
	y,
	oldx,
	oldy,
	speed,
	n;
var m=0;
var tri=[];

function setup() {
  // createCanvas(windowWidth, windowHeight);
  createCanvas(801,801)
  background(195)
  colorMode(HSB, 100);
  for(var i=0; i<613; i++) {
  	tri[i]=0;
  }
}



function draw() {
  stroke(50,0,50)
  oldx=x;
  oldy=y;
  x=Math.round((mouseX+16)/32);
  y=Math.round((mouseY+16)/32);
  speed=Math.abs(x-oldx)+Math.abs(y-oldy) //speed is usually just 1 or 2, goes up to 10
  if(speed*10>m){m=speed*10}
  if(m>100){m=100;}

  //fill(0,0,100)
  //main loops
  for(var j=0; j<25; j++) { //columns
  	var odd=j%2;
	for(var i=0; i<25-odd; i++) { //rows have either 13 or 12
		//number the pixel
		n=i+(j*25)-odd;
		if(Math.abs(x-i-1)<4 && Math.abs(y-j-1)<4){
			tri[n]=m
			if(Math.abs(x-i-1)<3 && Math.abs(y-j-1)<3){
				tri[n]=m*2
				if(Math.abs(x-i-1)<2 && Math.abs(y-j-1)<2){
					tri[n]=m*3
					if(x==i+1 && y==j+1) {
						tri[n]=m*4
					}
				}
			}
		}
		else {
			//fill(tri[n],tri[n],100)
		}
		fill(tri[n],tri[n],100)
		triangle(i*32+16+16*odd,j*32, i*32+16*odd,j*32+32, i*32+32+16*odd,j*32+32)
		tri[n]--
		if(tri[n]<0){tri[n]=0;}
	}
  }//end loops

  m--;
  if(m<0){m=0;}
}

