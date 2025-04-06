#version 330 core
out vec4 FragColor;


uniform vec3 cameraPosition;
uniform vec3 cameraFront;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;
uniform int _cell_amount;
uniform int _box_width;
uniform float _time;
uniform sampler3D _noiseSampler;
uniform float _radius;
uniform float _absorption;
uniform float _scatter;
uniform float _cutTexture;
uniform bool _powderFlag;
uniform float _powderFactor;
uniform float _powderOffset;
uniform float _initialFbmAmplitude;
uniform int _FbmOctave;
uniform float _noiseSize;
uniform float _innerEdge;
uniform float _outerEdge;

uniform vec4 _skyColor;
uniform vec4 _topColor;
uniform vec4 _buttomColor;

float fov = 45.0;
float d2r = 0.0174532925; 
int STEP = 150;
float min_distance = 0.001;
float walk_in_distance = .03;
float walk_light_distance = .03;
// TODO : add light color varince with the change of sky color
vec3 lightColor = vec3(236.0, 235.0, 229.0)/255.0;
vec3 skyColor = vec3(30.0, 137.0, 192.0)/255.0;
float ambientFactor = 0.3;
float scatteringFactor = .3;
float extinctionFactor = 1.5;

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
float sdRoundBox( vec3 p, vec3 b)
{
    float r = 0.1;
    vec3 q = abs(p) - b + r;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float sdTorus( vec3 p)
{
    vec2 t = vec2(_radius,_radius/2.);
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}

float sampleNoise(vec3 _pos){
	float value = 0.0;
    float amplitude = _initialFbmAmplitude;
    float st = _noiseSize;
    float octave = _FbmOctave; 

    // Loop of octaves
    for (int i = 0; i < octave; i++) {
		vec3 rp = 1.0 - (_pos- vec3(st))/(st*2.0);
		float t = texture(_noiseSampler, rp).r;
        value += amplitude * t;

        st *= .5;
        amplitude *= .5;
    }
    value = pow(value, _cutTexture) ;
    // TODO: controller for this.
    return smoothstep(0.01, 0.5, value)* value;
}

float sampleDensity(vec3 p){
    return sampleNoise(p+ vec3(1.0, 1.0, 0.0) * _time);
}



// TODO : select shape
float map( in vec3 pos )
{
    return sdBox(pos, vec3(_radius));
}

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773;
    const float eps = 0.0005;
    return normalize( e.xyy*map( pos + e.xyy*eps ) + 
					  e.yyx*map( pos + e.yyx*eps ) + 
					  e.yxy*map( pos + e.yxy*eps ) + 
					  e.xxx*map( pos + e.xxx*eps ) );
}

float Powder(float totalDensity){
    float powder =  1-exp(-totalDensity * _absorption* _powderFactor) + _powderOffset;
    return _powderFlag ? powder : 1.0;
}

// TODO : Phase function
float Phase(float g, float costh){
    return (1.0/(4.0 * 3.14)) * ((1.0 - g*g)/pow(1.0 + g*g - 2.0 * g * costh, 1.5));
}

float lightMarching(vec3 ro){
    vec3 lightDir = normalize(lightPosition);

    float t = 0.0;
    float totalDensity = 0.0;
    for( int i=0; i<STEP; i++ )
    {
        vec3 pos = ro + t*lightDir;
        float h = map(pos);
        if( h>min_distance) break;

        float density = 1.0;
        density = sampleDensity(pos);

        totalDensity += density * walk_light_distance;
        t += walk_light_distance;
        i++;
    }
    float transmittance = exp(-totalDensity* _scatter);
    transmittance *= Powder(totalDensity);
    return transmittance;
}



void Cloud(vec3 ro, vec3 rd, out vec3 col, out float alpha){

    alpha = 1.0;
    col = vec3(0.0);

    float transmittance= 1.0;
    vec3 lightEnergy = vec3(0.0);


    float t = 0.0;
    for( int i=0; i<STEP; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map(pos);
        if( h>min_distance) break;

        float density = 0.0;
		density = sampleDensity(pos) ;

        if(density > 0.0){
			float lightTransmittance = lightMarching(pos); // light for each point.
            lightEnergy += density * transmittance * lightTransmittance;
            transmittance *= exp(-density * walk_in_distance * _absorption);
        }
        t += walk_in_distance;
        i++;
    }
    vec3 cloudCol = lightEnergy;
    col = _skyColor.xyz * transmittance + cloudCol;
    alpha = 1.0 - transmittance;
}

void main()
{ 
    // camera
	vec3 ro = vec3(cameraPosition);
    vec3 ta = vec3( 0.0, 0.0, 0.0 );

    // camera matrix
    vec3 ww = normalize( cameraFront);
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));


    // screen position 
    vec2 p  = vec2(
        gl_FragCoord.x/800.0, 
        gl_FragCoord.y/800.0
    );
    p = (p* 2.0) - 1.0;

    // create view ray
    vec3 rd = normalize( p.x*uu + p.y*vv + 1.5*ww );

    // raymarch
    const float tmax = 10.0;
    float t = 0.0;
    for( int i=0; i<STEP; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map(pos);
        if( h<min_distance || t>tmax ) break;
        t += h;
    }

    // define
    vec3 col = vec3(0.0);
    float alpha = 0.0;
    if(t<tmax){
        col = vec3(1.0);
        alpha = 1.0;

        vec3 pos = ro + t*rd;

        Cloud(pos, rd, col, alpha);
		col = mix(_buttomColor, _topColor, col.x).xyz;
    }

    FragColor = vec4(col,alpha);
}