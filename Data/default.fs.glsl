#version 330

// Properties the Vertex Shader supplied
in vec2 texcoord;				//Texture coordinates
/*in vec3 position_world;			//Position in world space coordinates
in vec3 normal_cam;				//Normal in Camera space
in vec3 eyedir_cam;				//Vector towards the Camera in Camera space
in vec3 lightdir_cam;			//Vektor towards light in Camera space*/

out vec4 endColor;		//Farbe

//uniform sampler2D diffuseTextureSampler;
/*uniform vec3 lightPos;	//Position of light in world space
uniform vec3 lightColor;
uniform float lightPower;
uniform float ambientLight;
uniform vec3 specularColor;
uniform float specularExponent;
uniform vec3 sunDirection; //in camera space
uniform vec3 sunColor;*/

void main(void) {
    //endColor = vec4(1.0f, 1.0f, 0.f, 1.0f);
    vec4 diffuse = vec4(texcoord.rg, 0.0f, 1.0f);//texture( diffuseTextureSampler, texcoord );
    /*vec3 ambient = ambientLight * diffuse.rgb;

    float lightDistance = length( lightPos - position_world );

    vec3 n = normalize( normal_cam );
    vec3 l = normalize( lightdir_cam );

    float entryangle = clamp( dot( n, l ), 0, 1 );

    vec3 e = normalize( eyedir_cam );

    float reflectionangle = clamp( dot( e, reflect( -l, n ) ), 0, 1 );

    float entryangleSun = clamp( dot( n, sunDirection ), 0, 1 );

    float reflectionangleSun = clamp( dot( e, reflect( -sunDirection, n ) ), 0, 1 );

    endColor = vec4( ambient //Hintergrundbeleuchtung
    + diffuse.rgb * lightColor * lightPower * entryangle / (lightDistance * lightDistance) //Refraktion Lichtquelle
    + specularColor * lightColor * lightPower * pow( reflectionangle, specularExponent ) / (lightDistance * lightDistance) //Spiegelung Lichtquelle
    + diffuse.rgb * sunColor * entryangleSun //Refraktion Sonne
    + specularColor * sunColor * pow( reflectionangleSun, specularExponent ) //Spiegelung Sonne
    , diffuse.a );*/
    endColor = vec4(diffuse.rgb, 1);
}