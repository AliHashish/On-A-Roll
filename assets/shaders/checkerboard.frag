#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader [DONE].

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    // Obtain the relative position according to the size
    vec2 pos = floor(gl_FragCoord.xy / size);
    
    // If position (x,y) are even or odd both of them, then they will have the same color
    if((mod(pos.x,2.0)==0.0 && mod(pos.y,2.0)==0.0) || (mod(pos.x,2.0)!=0.0 && mod(pos.y,2.0)!=0.0)){
        frag_color=vec4(colors[0],1.0);
    }else{
        frag_color=vec4(colors[1],1.0);
    }
}