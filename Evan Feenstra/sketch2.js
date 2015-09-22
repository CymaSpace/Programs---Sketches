var x,
	y,
	oldx,
	oldy,
	speed,
	n;
var m=0;
var c=0;
var tri=[];

function setup() {
  // createCanvas(windowWidth, windowHeight);
  createCanvas(801,801)
  background(195)
  colorMode(HSB, 100)
  for(var i=0; i<613; i++) {
  	tri[i]=0
  }
}

function draw() {
  stroke(20,0,88)
  oldx=x
  oldy=y
  x=Math.round((mouseX)/16)
  y=Math.round((mouseY+16)/32)
  speed=Math.abs(x-oldx)+Math.abs(y-oldy) //speed is usually just 1 or 2, goes up to 10
  if(speed*33>m){m=speed*33}
  if(m>100){m=100}

  //main loops
  for(var j=0; j<25; j++) { //columns of 25
  	var odd=j%2
	for(var i=0; i<49; i++) { //rows have 49
		var odd_i=i%2
		n=i+(j*49) //number the pixel 0 to 1124
		var rx=Math.abs(x-i-1)  //radiusx
		var ry=Math.abs(y-j-1)  //radiusy
		if(rx<8 && ry<4){
			for(var h=4; h>2; h--) {  //two outer rings. More efficient to only select ring pixels?
				if(speed>=h+1) {
					if((rx<h*2 && ry<h-1) || (rx<(h-1)*2 && ry<h)){ //rounded corners
						tri[n]=m*0.2*(6-h) //less effect
					}
				}
			}
			if(speed>=2) {
				if(rx<3 && ry<2) {  //middle ring (15 pixels)
					tri[n]=m*0.7
				}
			}
			if(x==i+1 && y==j+1) { //center dot
				tri[n]=m
			}
		}
		if(tri[n]>0) {
			var color = tri[n]-Math.round(c)
			if(color<1){color=100+color}
			fill(color,Math.round(Math.sqrt(tri[n])*10),100) //(looped ramp, curved ramp, 100)
			tri[n]--
		}
		else{
			fill(0,0,100)
		}
		triangle(i*16+16,j*32+32*Math.abs(odd_i-odd), i*16,j*32+32-32*Math.abs(odd_i-odd), i*16+32,j*32+32-32*Math.abs(odd_i-odd))
	}
  }//end loops

  m--     //fade counter
  if(m<0){m=0}
  c=c-0.2 //color counter
  if(c<0){c=99}
}

