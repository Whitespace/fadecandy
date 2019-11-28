// ORB!

OPC opc;

// calm mode
int saturation = 70;
int min_brightness = 30;
int max_brightness = 100;
int percent_on = 60;
float frame_rate = 0.5;

// party mode
//int saturation = 100;
//int min_brightness = 40;
//int max_brightness = 100;
//int percent_on = 35;
//float frame_rate = 16;

void setup()
{
  opc = new OPC(this, "127.0.0.1", 7890);
  frameRate(frame_rate);
  colorMode(HSB, 100);
}

void draw() {
  glome();
  opc.setPixel(0, color(0,0,0)); // always make the first pixel black, as it's not attached to a panel
  opc.writePixels();
}

void glome() {
  for (int i = 0; i < 30; i++) {
    float duration = millis() * 0.001;            // how long to stay around the same hue lower numbers means it stays on the same color longer
    float offset = i * 2/30;                 // how much to vary across the strand.  Lower values means most of the colors are the same
    float hue = (duration + offset) % 100;
    
    // original code
    //hue = (millis() * 0.005 + i * 1.5) % 100;

    // is this light on?
    boolean on = random(100) < percent_on ? true : false;
    // if so, give it a random brightness within the min/max range
    float brightness = on ? random(min_brightness, max_brightness) : 0;

    opc.setPixel(i, color(hue, saturation, brightness));
  }  
}
