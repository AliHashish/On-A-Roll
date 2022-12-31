#version 330

uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;

//create a scanline effect
void main()
{
    vec4 color = texture(tex, tex_coord);
    float scanlineWidth = 0.003;
    float scanlineCount = 800.0;
    vec2 uv = tex_coord;
    uv.y *= scanlineCount;
    float res = mod(uv.y, 1.0);

    if(res < scanlineWidth || res > (1.0 - scanlineWidth)){
        color = vec4(color.r* 0.2,color.g*0.2,color.b* 0.2, color.a);
    } else {
        color = vec4(color.r* 0.8,color.g,color.b* 0.8, color.a);
    }
    frag_color = color;
}
