#version 410 core     
#define MAX_LIGHTS 12
#define MAX_MATERIAL 50
#define MAX_TEXTURE 30

uniform mat4 projectionMatrix;                                    
uniform mat4 viewMatrix;                                           
uniform mat4 modelMatrix;  
                                                                   
in vec3 pass_Normal;     
in vec3 pass_Position;   
in vec4 pass_Color; 
in vec2 pass_Texture;  

// The material parameters
uniform struct LightSource {
    vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float cutoff_in; // inner
	float cutoff_out; // outer
	float k1;    //attenuation
	float k2;    //attenuation
	bool used;
	int  type;  //0:point, 1:spot, 2:directional
} light[MAX_LIGHTS];


// The material parameters
uniform struct Material {
    vec3  diffColor;
 	float diffInt;
 	vec3  ambColor;
 	float ambInt;
 	vec3  specColor;
 	float specInt;
 	float shininess;
} mat[MAX_MATERIAL];

uniform int material_index;

// for foveated rendering
uniform vec2 eye_pos;
uniform vec2 screen_res;
uniform float eye_radius;
uniform int with_fr; // to activate or deactivate foveated rendering


uniform struct Texture {
	sampler2D tex_kd;
	int		  with_tex_kd;
	sampler2D tex_ka;
	int		  with_tex_ka;
}tex[MAX_TEXTURE];

uniform int texture_index;
uniform int with_textures;
uniform float texture_multiplier;

out vec4 color;       


bool with_per_vertex_color = true;
bool with_smoothstep = true;
bool with_brightlight = false;

float gAlpha = 1.0;


/*
Per-fragment light. 
Note that all vectors need to be in camera/eye-space. 
@param L - vector between light and fragment position.
@param E - eye point
@param s - the light source
@param m - the material 
*/
vec4 useLight(vec3 L, vec3 E, vec3 N, LightSource s, Material m, Texture t)
{
	vec4 color = vec4(0.0,0.0,0.0,0.0);
	vec4 tex_color_kd = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 tex_color_ka = vec4(1.0, 1.0, 1.0, 1.0);
	float alpha = 0.0;
	
	if(with_textures == 1){
			if(t.with_tex_kd == 1)
				tex_color_kd = texture(t.tex_kd, pass_Texture.xy);
			if(t.with_tex_ka == 1){
				tex_color_ka = texture(t.tex_ka, pass_Texture.xy);
				//tex_color_kd.a = tex_color_ka.r;
				//tex_color_kd = tex_color_ka;
				//tex_color_kd.a = tex_color_ka.x;
				//gAlpha = tex_color_ka.x;
				//alpha = tex_color_ka.x;
				if(tex_color_ka.x == 0)discard;
			}
	}
	

	// diffuse light
	vec3 Idiff =  m.diffInt * m.diffColor * tex_color_kd.rgb *  max(dot(L, N), 0.0); 
	Idiff = clamp(Idiff, 0.0, 1.0); 

	// ambient light
	vec4 Iamb = vec4( m.ambColor, alpha) * m.ambInt;

	// specular light
	vec3 R = reflect(L, N);
	vec3 Ispec =  m.specInt * m.specColor *  pow(max(dot(R, E), 0.0), m.shininess); 

	// calculate color                                     
	color = max(vec4( ( Idiff + Ispec) * s.color, alpha), Iamb)  * s.intensity;  

	// attenuation 
	float dist = length(L);
	float Iatt = 1 / (1 + s.k1 * dist + s.k2*s.k2 * dist);

	if(s.type == 0)//pointlight
	{
		color = color * Iatt;
	}
	else if(s.type == 1)// spotlight
	{
		vec4 K = viewMatrix * vec4(normalize(s.direction - s.position), 0.0);
		float a = dot(-L,K.xyz); // angle between light dir and fragment position. 

		float c = smoothstep(1.0-s.cutoff_out, 1.0-s.cutoff_in, a); 
		Iatt = Iatt * c; // multiple with attenuation to maintain the distance effect. 

		color = color * Iatt; // attenutation
	}


	return color;
}


 	// check radius
vec4 color_add = vec4(1.0,0.0,0.0,0.0);
const vec3 E = vec3(0.0,0.0,0.0);
                       
void main(void)                                                  
{    

	// normalized frag coordinates sc
	float rt = eye_radius  - 0.1;
	vec2 sc = gl_FragCoord.xy /screen_res;

	float d = distance(sc, eye_pos );

	if(with_fr == 1){
		color_add =  vec4(0.8,0.0,0.0,0.0);
		if(d > eye_radius)
		{
			discard;
			//color_add =  vec4(0.8,0.0,0.0,0.0);;
		}
		else
		{
			
			// calc light. 
			vec4 mixed = vec4(0.0,0.0,0.0,1.0);
			for (int i=0; i<MAX_LIGHTS; i++){

				if(light[i].used == false) continue;

				// light position if camera frame
				vec4 L_c = viewMatrix * vec4(light[i].position, 1.0);
			
				// light to fragment 
				vec3 L = normalize( L_c.xyz - pass_Position );
				if(light[i].type == 2) L = light[i].direction;// direct light

				// checks whether the light was set.
				// Multiple lights blend adative
				mixed += useLight( L,  E,  pass_Normal, light[i], mat[material_index], tex[texture_index]);
				
			}
			
			//vec4 tex_color = vec4(0.0, 0.0, 0.0, 0.0);
			//if(tex[texture_index].with_tex_kd == 1){
			//	tex_color = texture(tex[texture_index].tex_kd, pass_Texture.xy);
			//}
			
		
			color_add = mixed + vec4(0.8,0.0,0.0,0.0);
		}

	}else
	{
		// calc light. 
		vec4 mixed = vec4(0.0,0.0,0.0,1.0);
		for (int i=0; i<MAX_LIGHTS; i++){

			if(light[i].used == false) continue;

			// light position if camera frame
			vec4 L_c = viewMatrix * vec4(light[i].position, 1.0);
		
			// light to fragment 
			vec3 L = normalize( L_c.xyz - pass_Position );
			if(light[i].type == 2) L = light[i].direction;// direct light

			// checks whether the light was set.
			// Multiple lights blend adative
			mixed += useLight( L,  E,  pass_Normal, light[i], mat[material_index], tex[texture_index]);
				
		}
		
	///	vec4 tex_color = vec4(0.0, 0.0, 0.0, 0.0);
	//	vec4 tex_color_ka = vec4(1.0, 1.0, 1.0, 1.0);
		
	/*	
		if(with_textures == 1){
			if(tex[texture_index].with_tex_kd == 1)
				tex_color = texture(tex[texture_index].tex_kd, pass_Texture.xy);
			if(tex[texture_index].with_tex_ka == 1)
				tex_color_ka = texture(tex[texture_index].tex_ka, pass_Texture.xy);
				tex_color.a = tex_color_ka.r;
			
		}
		
		*/
			
		
		color_add =  mixed ;
	}
	
	
   //color_add.a = gAlpha;        
	color =  color_add;                         
}                                                      