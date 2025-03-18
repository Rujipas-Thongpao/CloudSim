#version 330 core
out vec4 FragColor;


uniform vec3 cameraPosition;
uniform vec3 cameraFront;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;
// uniform float[12*12*12] _voronoi_noise_position;

uniform int _cell_amount;
uniform int _box_width;
uniform float _time;
uniform sampler3D _noiseSampler;

float fov = 45.0;
float d2r = 0.0174532925; 
int STEP = 50;
float min_distance = 0.001;
float walk_in_distance = .03;
float walk_light_distance = .03;
float absorption = 10.0;
float radius = 1.0;
// vec3 lightPosition = vec3(1.0,0.0,0.0);
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

    vec2 t = vec2(.2,.1);
    vec2 q = vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}

float sampleNoise(vec3 _pos){
    // return ((_pos - vec3(-1.0))/2.0);
    float r1 = 3.0;
    float r2 = 0.8;
    float r3 = 0.3;

    float w1 = .8;
    float w2 = .15;
    float w3 = .05;

    vec3 rp1 = 1.0-(_pos - vec3(r1))/(r1*2.0);
    vec3 rp2 = 1.0-(_pos - vec3(r2))/(r2*2.0);
    vec3 rp3 = 1.0-(_pos - vec3(r3))/(r3*2.0);

    float t1 = texture(_noiseSampler, rp1).r;
    float t2 = texture(_noiseSampler, rp2).r;
    float t3 = texture(_noiseSampler, rp3).r;

    return pow(w1*t1+w2*t2+w3*t3,2);
    // return 1.0;
}

float sampleDensity(vec3 p){
    return sampleNoise(p+ vec3(4.0, 4.0, 0.0) * _time);
}

// float Phase(float g, float costh){
//     return (1.0/(4.0 * 3.14)) * ((1.0 - g*g)/pow(1.0 + g*g - 2.0 * g * costh, 1.5));
// }


float map( in vec3 pos )
{
    return sdBox(pos, vec3(radius));
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
    }
    float transmittance = exp(-totalDensity* absorption);
    // float powder = 1.0;
    // powder =  1-exp(-totalDensity * absorption* 2);
    return transmittance;
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

void Cloud(vec3 ro, vec3 rd, out vec3 col, out float alpha){

    alpha = 1.0;
    col = vec3(1.0);

    float t = 0.0;
    float totalDensity = 0.0;
    // col = vec3(lightMarching(ro));
    for( int i=0; i<STEP; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map(pos);
        if( h>min_distance) break;

        float density = 1.0;
        density = sampleDensity(pos) ;
        totalDensity += density * walk_in_distance;

        float lightTransmittance = lightMarching(pos); // light for each point.
        // // lightTransmittance *=  1/pow(length(lightPosition-rayHead),2.0);  // if we care about point light.
        float baseTransmittance = exp(-absorption * totalDensity);
        // col += vec3(lightTransmittance * baseTransmittance);

        t += walk_in_distance;
    }
    float extinction = exp(-absorption * totalDensity) ;
    alpha = 1.0-extinction;

    // float extinction =1.0;
    // int currentStep = 0;
    // // col += lightMarching(rayHead); // light for each point.
    // while(sdf(rayHead) <= min_distance && currentStep < STEP){
    //     // sample density
    //     float density = 1.0;
    //     density = sampleDensity(rayHead) ;
    //     totalDensity += density * walk_in_distance;

    //     float lightTransmittance = lightMarching(rayHead); // light for each point.
    //     // lightTransmittance *=  1/pow(length(lightPosition-rayHead),2.0);  // if we care about point light.
    //     float baseTransmittance = exp(-absorption * totalDensity);

    //     // vec3 lightDir = normalize(lightPosition-rayHead);
    //     // float phase = Phase(0.7,  dot(-rayHead,lightDir));

    //     col += lightTransmittance * baseTransmittance * walk_in_distance;

    //     // iterate ray
    //     rayHead += ray * walk_in_distance;
    //     currentStep++;
    // }
    // extinction = exp(-absorption * totalDensity) ;
    // alpha = 1.0-extinction;
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

    }

    FragColor = vec4(col,alpha);
}