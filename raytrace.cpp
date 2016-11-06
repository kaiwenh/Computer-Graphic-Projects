//
// template-rt.cpp
//

#define _CRT_SECURE_NO_WARNINGS
#include "matm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

int g_width;
int g_height;
string output_name;

struct Ray
{
    vec4 origin;
    vec4 dir;
};

// TODO: add structs for spheres, lights and anything else you may need.
struct Sphere
{
    mat4 transform;
    string name;
    vec3 colors;
    float Ka, Kd, Ks, Kr, n;
    Sphere (string nm, vec3 position, vec3 scale, vec3 color, float a, float d, float s,
            float r, float exponent)
    {
        name = nm;
        colors = color;
        Ka = a;
        Kd = d;
        Ks = s;
        Kr = r;
        n = exponent;
        transform = mat4(scale.x, 0, 0, 0,
                         0, scale.y, 0, 0,
                         0, 0, scale.z, 0,
                         position.x, position.y, position.z, 1);
    }
    
    Sphere() {}
    
};

struct Light
{
    string name;
    vec3 position;
    vec3 intensity;
    Light (string n, float x, float y, float z, float r, float g, float b)
    {
        name = n;
        position.x = x;
        position.y = y;
        position.z = z;
        intensity.x = r;
        intensity.y = g;
        intensity.z = b;
    }
};

vector<vec4> g_colors;
vector<Sphere> g_spheres;
vector<Light> g_lights;
vec3 g_back;
vec3 g_ambient;


float g_left;
float g_right;
float g_top;
float g_bottom;
float g_near;


// -------------------------------------------------------------------
// Input file parsing

vec4 toVec4(const string& s1, const string& s2, const string& s3)
{
    stringstream ss(s1 + " " + s2 + " " + s3);
    vec4 result;
    ss >> result.x >> result.y >> result.z;
    result.w = 1.0f;
    return result;
}

vec3 toVec3 (vec4 v)
{
    return vec3(v.x, v.y, v.z);
}

float toFloat(const string& s)
{
    stringstream ss(s);
    float f;
    ss >> f;
    return f;
}

void parseLine(const vector<string>& vs)
{
    //TODO: add parsing of NEAR, LEFT, RIGHT, BOTTOM, TOP, SPHERE, LIGHT, BACK, AMBIENT, OUTPUT.
    if (vs[0] == "RES")
    {
        g_width = (int)toFloat(vs[1]);
        g_height = (int)toFloat(vs[2]);
        g_colors.resize(g_width * g_height);
    }
    if (vs[0] == "NEAR")
        g_near = toFloat(vs[1]);
    
    if (vs[0] == "LEFT")
        g_left = toFloat(vs[1]);
    
    if (vs[0] == "RIGHT")
        g_right = toFloat(vs[1]);
    if (vs[0] == "BOTTOM")
        g_bottom = toFloat(vs[1]);
    
    if (vs[0] == "TOP")
        g_top = toFloat(vs[1]);
    
    if (vs[0] == "SPHERE")
    {
        vec3 position = toVec3(toVec4(vs[2], vs[3], vs[4]));
        vec3 scale = toVec3(toVec4(vs[5], vs[6], vs[7]));
        vec3 color = toVec3(toVec4(vs[8], vs[9], vs[10]));
        Sphere sphere(vs[1], position, scale, color, toFloat(vs[11]), toFloat(vs[12]),
                      toFloat(vs[13]), toFloat(vs[14]), toFloat(vs[15]));
        g_spheres.push_back(sphere);
    }
    if (vs[0] == "LIGHT")
    {
        Light light(vs[1], toFloat(vs[2]), toFloat(vs[3]), toFloat(vs[4]),
                           toFloat(vs[5]), toFloat(vs[6]), toFloat(vs[7]));
        g_lights.push_back(light);
    }
    if (vs[0] == "BACK")
    {
        g_back.x = toFloat(vs[1]);
        g_back.y = toFloat(vs[2]);
        g_back.z = toFloat(vs[3]);
    }
    if (vs[0] == "AMBIENT")
    {
        g_ambient.x = toFloat(vs[1]);
        g_ambient.y = toFloat(vs[2]);
        g_ambient.z = toFloat(vs[3]);
    }
    if (vs[0] == "OUTPUT")
        output_name = vs[1];
    
}

void loadFile(const char* filename)
{
    ifstream is(filename);
    if (is.fail())
    {
        cout << "Could not open file " << filename << endl;
        exit(1);
    }
    string s;
    vector<string> vs;
    while(!is.eof())
    {
        vs.clear();
        getline(is, s);
        istringstream iss(s);
        while (!iss.eof())
        {
            string sub;
            iss >> sub;
            vs.push_back(sub);
        }
        parseLine(vs);
    }
}


// -------------------------------------------------------------------
// Utilities

void setColor(int ix, int iy, const vec4& color)
{
    int iy2 = g_height - iy - 1; // Invert iy coordinate.
    g_colors[iy2 * g_width + ix] = color;
}


// -------------------------------------------------------------------
// Intersection routine

// TODO: add your ray-sphere intersection routine here.

/* calculate the t values for the quadratic equation: return calculated value only if: > 1
 if there are two, return the smaller one; otherwise return 0*/

float calculateI1(vec3 S, vec3 c)
{
    float t = 0;
    float t1 = - dot(S, c) / dot(c, c) +
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    float t2 = - dot(S, c) / dot(c, c) -
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    if (t1 <= 1 && t2 <= 1)
        return 0;
    else if (t1 > 1 && t2 > 1)
    {
        t = t1 > t2 ? t2 : t1;
        return t;
    }
    else
        return t1;
    
}

float calculateI2(vec3 S, vec3 c)
{
    float t = 0;
    float t1 = - dot(S, c) / dot(c, c) +
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    float t2 = - dot(S, c) / dot(c, c) -
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    if (t1 <= 0.0001 && t2 <= 0.0001)
        return 0;
    else if (t1 > 0.0001 && t2 > 0.0001)
    {
        t = t1 > t2 ? t2 : t1;
        return t;
    }
    else
        return t1;
}

/* drop the 4th component of a vec4 to form a vec3 */


/*Find the smallest t value greater than 1 for a ray intersecting with a sphere; if none, return 0*/
float intersection(Ray ray, Sphere sphere, int depth)
{
    const mat4 M = sphere.transform;
    mat4 inverse_M;
    if (InvertMatrix(M, inverse_M))
    {
        /* find S' c' and and drop 0, 1 to get vec3 */
        vec3 inverse_origin = toVec3(inverse_M * ray.origin);
        vec3 inverse_dir = toVec3(inverse_M * ray.dir);
        
        /*check B^2 - AC >= 0 */
        if (pow(dot(inverse_origin, inverse_dir), 2) -
            dot(inverse_dir, inverse_dir) * (dot(inverse_origin, inverse_origin) - 1) >= 0)
        {
            if (depth == 3)
                return calculateI1(inverse_origin, inverse_dir);
            else
                return calculateI2(inverse_origin, inverse_dir);
        }
        
        else
            return 0;
    }
    else
        return 0;
}

/* check if any of the t values are between epsilon(0.0001) and 1 */
bool calculateS(vec3 S, vec3 c)
{
    float t1 = - dot(S, c) / dot(c, c) +
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    float t2 = - dot(S, c) / dot(c, c) -
    sqrt(pow(dot(S, c), 2) - dot(c, c) * (dot(S, S) - 1)) / dot(c, c);
    if (0.0001 < t1 && t1 < 1)
        return true;
    else if (0.0001 < t2 && t2 < 1)
        return true;
    /*if (((0.0001 < t1) && (t1 < 1)) || ((0.0001 < t2) && (t2 < 1)))
        return true;*/
    else
        return false;
}

bool checkShadowing(Ray ray, Sphere sphere)
{
    const mat4 M = sphere.transform;
    mat4 inverse_M;
    if (InvertMatrix(M, inverse_M))
    {
        /* find S' c' and and drop 0, 1 to get vec3 */
        vec3 inverse_origin = toVec3(inverse_M * ray.origin);
        vec3 inverse_dir = toVec3(inverse_M * ray.dir);
        
        /*check B^2 - AC >= 0 */
        if (pow(dot(inverse_origin, inverse_dir), 2) -
            dot(inverse_dir, inverse_dir) * (dot(inverse_origin, inverse_origin) - 1) >= 0)
            return calculateS(inverse_origin, inverse_dir);
        else
            return false;
    }
    else
        return false;
}


vec3 getHitpoint(vec3 S, vec3 c, float t)
{
    return (S + c * t);
}

bool checkInside(vec3 S, vec3 c, float t)
{
    float result = dot(c, c) * pow(t, 2) + 2 * dot(S, c) * t + dot(S, S);
    if (result < 1)
        return true;
    else
        return false;
}

            


// -------------------------------------------------------------------
// Ray tracing

vec4 trace(const Ray& ray, int depth)
{
    // TODO: implement your ray tracing routine here.
    float pixel_t = 0;
    Sphere target;
    for (int i = 0; i < g_spheres.size(); i++)
    {
        
        float tmp;
        if (depth == 3)
            tmp = intersection(ray, g_spheres[i], 3);
        else
            tmp = intersection(ray, g_spheres[i], 0);
        if (tmp != 0)
        {
            /*sort the t values for different spheres*/
            if (pixel_t == 0 || pixel_t > tmp)
            {
                pixel_t = tmp; /*update t value if the new t is smaller than current t*/
                target = g_spheres[i]; /*record the sphere with the lowest t value*/
            }
        }
    }
    
    
    
    /*no t value for this pixel (no intersection)*/
    vec4 whiteBack(1, 1, 1 ,1);
    if (pixel_t == 0)
    {
        if (depth < 3 && depth != 0)
            return vec4(0, 0, 0, 1);
        else
            return vec4(g_back);
    }
    
    
    else
    {
        
        vec3 diffuse(0, 0, 0);
        vec3 specular(0, 0, 0);
        /* P: intersection point on the transformed sphere*/
        vec3 P = getHitpoint(toVec3(ray.origin), toVec3(ray.dir), pixel_t);
        
        /* Get the normal */
        /*get the target sphere's information */
        mat4 inv_M;
        InvertMatrix(target.transform, inv_M);
        vec3 inv_origin = toVec3(inv_M * ray.origin);
        vec3 inv_dir = toVec3(inv_M * ray.dir);
         
        vec3 nor = getHitpoint(inv_origin, inv_dir, pixel_t);//normal on the untransformed sphere
        mat4 inv_transpose = transpose(inv_M);
        vec3 N = normalize(toVec3(inv_transpose * vec4(nor.x, nor.y, nor.z, 0))); //normal on the transformed sphere
        
        /* STEP 3: iterate through light sources to get ILLUMINATION */
        
        for (int j = 0; j < g_lights.size(); j++)
        {
            /*STEP 4.1: check SHADOWING RAYS */
            /*set up a ray from P to light souce*/
            Ray pointToLight;
        
            pointToLight.origin = vec4(P);
            pointToLight.dir = vec4((g_lights[j].position - P).x,
                                    (g_lights[j].position - P).y,
            
                                    (g_lights[j].position - P).z, 0);
            
            
            int k;
            for (k = 0; k < g_spheres.size(); k++)
            {
                if (checkShadowing(pointToLight, g_spheres[k]) == true)
                    break;
            }
            
            vec3 L = normalize(g_lights[j].position - P); /* from hitpoint to light source */
            
            vec3 R = normalize(2 * N * dot(N, L)- L);
            
            if (dot(N,L) > 0 && k == g_spheres.size())
                diffuse += g_lights[j].intensity * dot(N, L) * target.colors;
            else
                continue;
            
            if (dot(R, normalize(toVec3(ray.origin)-P)) > 0
                && k == g_spheres.size())
                specular += g_lights[j].intensity * pow(dot(R, normalize(toVec3(ray.origin)-P)), target.n);
            else
                continue;
        }
        diffuse *= target.Kd;
        
        specular *= target.Ks;
        
        /* STEP 4.2: reflection */
        Ray reflectedRay;
        reflectedRay.origin = vec4(P);
        reflectedRay.dir = vec4(-2 * dot(N, ray.dir)* N, 0) + ray.dir;
        
        if (depth == 0)
            return vec4(diffuse + specular + target.Ka * g_ambient * target.colors);
        
        return vec4(diffuse + specular + target.Ka * g_ambient * target.colors)
        + target.Kr * trace(reflectedRay, depth - 1);
        //return vec4(diffuse + specular + target.Ka * g_ambient * target.colors);

    }
}

vec4 getDir(int ix, int iy)
{
    // TODO: modify this. This should return the direction from the origin
    // to pixel (ix, iy), normalized.
    vec4 dir;
    dir.x = g_left + (float(ix) / g_width) * (g_right - g_left);
    dir.y = g_bottom + (float(iy) / g_height) * (g_top - g_bottom);
    dir.z = - g_near;
    dir.w = 0;
    return dir;
}

void renderPixel(int ix, int iy)
{
    Ray ray;
    ray.origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    ray.dir = getDir(ix, iy);
    vec4 color = trace(ray, 3);
    setColor(ix, iy, color);
}

void render()
{
    for (int iy = 0; iy < g_height; iy++)
        for (int ix = 0; ix < g_width; ix++)
            renderPixel(ix, iy);
}


// -------------------------------------------------------------------
// PPM saving

void savePPM(int Width, int Height, char* fname, unsigned char* pixels) 
{
    FILE *fp;
    const int maxVal=255;

    printf("Saving image %s: %d x %d\n", fname, Width, Height);
    fp = fopen(fname,"wb");
    if (!fp) {
        printf("Unable to open file '%s'\n", fname);
        return;
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", Width, Height);
    fprintf(fp, "%d\n", maxVal);

    for(int j = 0; j < Height; j++) {
        fwrite(&pixels[j*Width*3], 3, Width, fp);
    }

    fclose(fp);
}

void saveFile()
{
    // Convert color components from floats to unsigned chars.
    // TODO: clamp values if out of range.
    for (int i = 0; i < g_colors.size(); i++)
    {
        if (g_colors[i].x > 1)
            g_colors[i].x = 1;
        if (g_colors[i].y > 1)
            g_colors[i].y = 1;
        if (g_colors[i].z > 1)
            g_colors[i].z = 1;
    }
    
    unsigned char* buf = new unsigned char[g_width * g_height * 3];
    for (int y = 0; y < g_height; y++)
        for (int x = 0; x < g_width; x++)
            for (int i = 0; i < 3; i++)
                buf[y*g_width*3+x*3+i] = (unsigned char)(((float*)g_colors[y*g_width+x])[i] * 255.9f);
    
    // TODO: change file name based on input file name.
    char *cstr = new char [output_name.length() + 1];
    strcpy(cstr, output_name.c_str());
    savePPM(g_width, g_height, cstr, buf);
    delete[] buf;
}

/*void test()
{
    ofstream outfile("test.txt");
    outfile << "NEAR: " << g_near << endl;
    outfile << "LEFT: " << g_left << endl;
    outfile << "RIGHT: " << g_right << endl;
    outfile << "BOTTOM: " << g_bottom << endl;
    outfile << "TOP: " << g_top << endl;
    outfile << "RES: " << g_width << g_height << endl;
    vector<Sphere>::iterator sph;
    vector<Light>::iterator li;
    for (sph = g_spheres.begin(); sph != g_spheres.end(); sph++)
    {
        outfile << "SPHERES: ";
        outfile << sph->name << " " << sph->transform._m[0].w << " " <<
        
    }
}*/

void testTrans()
{
    ofstream outfile("test.txt");
    vector<Sphere>::iterator sph;
    vector<Light>::iterator li;
    for (sph = g_spheres.begin(); sph != g_spheres.end(); sph++)
    {
        outfile << "SPHERES: ";
        mat4 M = sph->transform;
        outfile << sph->name << " " << M[0].x << " " << M[1].y << " " << " "
                                    << M[2].z << " " << M[3].w << " " << " "
                                    << M[0].w << " " << M[1].w << " " << " "
        << M[2].w << " " << endl;
    }

}


// -------------------------------------------------------------------
// Main

int main(int argc, char* argv[])
{
    
    if (argc < 2)
    {
        cout << "Usage: template-rt <input_file.txt>" << endl;
        exit(1);
    }
    loadFile(argv[1]);
    render();
    saveFile();
	return 0;
}

