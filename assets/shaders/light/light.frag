#version 330 core


in Varyings {
    vec4 color;
    vec2 tex_coord;
    // We will need the vertex position in the world space,
    vec3 world;
    // // the view vector (vertex to eye vector in the world space),
    vec3 view;
    // // and the surface normal in the world space.
    vec3 normal;
} fsin;

// These type constants match their peers in the C++ code.
#define TYPE_DIRECTIONAL    0
#define TYPE_POINT          1
#define TYPE_SPOT           2

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
};

// Now we will use a single struct for all light types.
struct Light {
    // This will hold the light type.
    int type;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    // Position is used for point and spot lights. Direction is used for directional and spot lights.
    vec3 position, direction;
    // Attentuation factors are used for point and spot lights.
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
    // Cone angles are used for spot lights.
    float inner_angle, outer_angle;
};

// This will define the maximum number of lights we can receive.
#define MAX_LIGHT_COUNT 16

struct LitMaterial {
    sampler2D albedo_map;
    vec3 albedo_tint;
    sampler2D specular_map;
    vec3 specular_tint;
    sampler2D roughness_map;
    vec2 roughness_range;
    sampler2D emissive_map;
    vec3 emissive_tint;
    sampler2D ambient_occlusion_map;
};


// // Now we recieve the material, light array, the actual number of lights sent from the cpu and the sky light.
uniform LitMaterial material;
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;

out vec4 frag_color;

void main() {
    // First, we sample the material at the current pixel.
    Material sampled;
    sampled.diffuse =texture(material.albedo_map, fsin.tex_coord).rgb;      // Albedo is used to sample the diffuse
    sampled.specular = texture(material.specular_map, fsin.tex_coord).rgb;
    sampled.emissive = texture(material.emissive_map, fsin.tex_coord).rgb;
    sampled.ambient = sampled.diffuse * texture(material.ambient_occlusion_map, fsin.tex_coord).r;
    // Ambient is computed by multiplying the diffuse by the ambient occlusion factor. This allows occluded crevices to look darker.

    // Roughness is used to compute the shininess (specular power).
    float roughness =  texture(material.roughness_map, fsin.tex_coord).r;

    // It is noteworthy that we clamp the roughness to prevent its value from ever becoming 0 or 1 to prevent lighting artifacts.
    sampled.shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;

    // Then we normalize the normal and the view. These are done once and reused for every light type.
    vec3 normal = normalize(fsin.normal); // Although the normal was already normalized, it may become shorter during interpolation.
    vec3 view = normalize(fsin.view);


    vec3 accumulated_light = vec3(0.0f);
    // // Make sure that the actual light count never exceeds the maximum light count.
    int count = min(light_count, MAX_LIGHT_COUNT);
    vec3 ambient = sampled.ambient;
    // // Now we will loop over all the lights.
    for(int index = 0; index < count; index++){
        Light light = lights[index];
        ambient *= light.ambient;
        vec3 light_direction;
        float attenuation = 1;
        if(light.type == TYPE_DIRECTIONAL)
            light_direction = light.direction; // If light is directional, use its direction as the light direction
        else {
            // If not directional, compute the direction from the position.
            light_direction = fsin.world - light.position;
            float distance = length(light_direction);
            light_direction /= distance;

            // And compute the attenuation.
            attenuation *= 1.0f / (light.attenuation_constant +
            light.attenuation_linear * distance +
            light.attenuation_quadratic * distance * distance);

            if(light.type == TYPE_SPOT){
                // If it is a spot light, comput the angle attenuation.
                float angle = acos(dot(light.direction, light_direction));
                attenuation *= smoothstep(light.outer_angle, light.inner_angle, angle);
            }
        }

        // Now we compute the 2 components of the light separately.
        vec3 diffuse = sampled.diffuse * light.diffuse * max(0.0f, dot(normal, -light_direction));
        vec3 specular = sampled.specular * light.specular * pow(max(0.0f, dot(view, reflect(light_direction, normal))), sampled.shininess);

        // Then we accumulate the light components additively.
        accumulated_light += (diffuse + specular+ ambient) * attenuation ;
    }

    frag_color = fsin.color * vec4(accumulated_light+sampled.emissive, texture(material.albedo_map, fsin.tex_coord).a);
}
