var x,
	y,
	oldx,
	oldy,
	speed,
	speedx,
	speedy, 
	n;
var m=0;
var c=0;
var p=0; //particle #
var tri=[];
var parts=[]; //particles

function setup() {
  // createCanvas(windowWidth, windowHeight);
  createCanvas(801,801)
  background(195)
  colorMode(HSB, 100)
}

function draw() {
  stroke(20,0,88)
  oldx=x
  oldy=y
  x=Math.round((mouseX)/16)
  y=Math.round((mouseY+16)/32)
  speedx=x-oldx
  speedy=y-oldy
  speed=Math.abs(speedx)+Math.abs(speedy) //speed is usually just 1 or 2, goes up to 10
  if(speed*33>m){m=speed*33}
  if(m>100){m=100}

  if(speedy<0) { //upward momentum
  	parts[p] = new Part(x,y,speedx,speedy)
  	p++
  	if(p>49){p=0} //50 max particles
  }

  //main rendering loops
  for(var j=0; j<25; j++) { //columns of 25
  	var odd=j%2
	for(var i=0; i<49; i++) { //rows have 49
		var odd_i=i%2
		n=i+(j*49) //number the pixel 0 to 1124

		//particles
		for(var k=0; k<parts.length; k++) {
	  	  if(parts[k]){
	  	  	if(parts[k].x==i+1 && parts[k].y==j+1) {
	  	  		tri[n]=m
	  	  	}
	  	  }
		}

		//main rings
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
		var switcher = 32*Math.abs(odd_i-odd); //switches orientation of triangles
		triangle(i*16+16,j*32+switcher, i*16,j*32+32-switcher, i*16+32,j*32+32-switcher)
	}
  }//end loops

  m--     //fade counter
  if(m<0){m=0}
  c=c-0.2 //looped color counter
  if(c<0){c=99}

  //move particles
  for(var i=0; i<parts.length; i++) {
  	if(parts[i]){
		parts[i].x = parts[i].x + parts[i].sx
		parts[i].y = parts[i].y + parts[i].sy
		if(parts[i].y<0 || parts[i].x<0 || parts[i].x>49){parts[i]=null}
	}
  }
}

function Part(x,y,sx,sy) {
	this.x=x
	this.y=y
	this.sx=sx
	this.sy=sy
}