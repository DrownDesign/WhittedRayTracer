//Other Classes
#include "SceneObject.h"
#include "SphereObj.h"
#include "SceneLight.h"
#include "ScenePlane.h"

//Libraries
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <utility>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace glm;

const float INFINTY = numeric_limits<float>::max();

vector<unique_ptr<SceneObject>> sceneObjs;
vector<unique_ptr<SceneLight>> sceneLights;

//Defines the environment objects and lights
void defineScene() {

	//Sphere 1 properties
	SphereObj *sph1 = new SphereObj(vec3(-1, 0, -12), 2);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = vec3(0.6, 0.7, 0.8);

	//Sphere 2 properties
	SphereObj *sph2 = new SphereObj(vec3(0.5, -0.5, -8), 1.5);
	sph2->materialType = REFLECTION_AND_REFRACTION;
	sph2->ior = 1.5;

	sceneObjs.push_back(unique_ptr<SphereObj>(sph1));
	sceneObjs.push_back(unique_ptr<SphereObj>(sph2));

	//Define plane
	vector<vec3> vertices{ vec3(-5, -3, -6), vec3(5,-3,-6), vec3(5,-3,-16), vec3(-5,-3,-16) };
	vector<int> indices{ 0 , 1 , 3 , 1 , 2 , 3 };
	vector<vec2> uvs{ {0,0}, {1,0}, {1,1}, {0,1} };
	ScenePlane *plane = new ScenePlane(vertices, indices, 2, uvs);
	plane->materialType = DIFFUSE_AND_GLOSSY;

	sceneObjs.push_back(unique_ptr<ScenePlane>(plane));

	sceneLights.push_back(unique_ptr<SceneLight>(new SceneLight(vec3(-20, 70, 20), vec3(0.5))));
	sceneLights.push_back(unique_ptr<SceneLight>(new SceneLight(vec3(30, 50, -12), vec3(1))));

	printf("Scene Defined Successfully\n");
}

struct Options {
	int width, height, maxDepth;
	float fov, aspectRatio, bias;
	vec3 backgroundColor;
};

Options options;

void setOptions() {

	options.width = 1920;
	options.height = 1080;
	options.fov = 90.f;
	options.backgroundColor = vec3(0.235294f, 0.67451f, 0.843137f);
	options.maxDepth = 10;
	options.bias = 0.00001f;

	printf("Options Set\n");
}

bool trace(vec3 &orig, vec3 &dir, float &tNear, int &index, vec2 &uv, SceneObject **hitObj) {

	*hitObj = nullptr;

	for (int i = 0; i < sceneObjs.size(); i++) {
		
		float tNearI = INFINITY;
		int indexI;
		vec2 uvI;
		
		if (sceneObjs[i]->intersect(orig, dir, tNearI, indexI, uvI) && tNearI < tNear) {
			*hitObj = sceneObjs[i].get();
			tNear = tNearI;
			index = indexI;
			uv = uvI;
		}
	}
	
	bool hit = *hitObj != nullptr;

	//Prints if an object is hit
	//printf(hit ? "Hit = true\n" : "Hit = false\n");
	return (hit);
}

void fresnel(const vec3 &dir, const vec3 &norm, const float &ior, float &kr) {
	float cosi = clamp(-1.f, 1.f, dot(dir, norm));
	float etai = 1.0f, etat = ior;
	if (cosi > 0) { swap(etai, etat); }
	
	//Compute sinT using Snell's law
	float sinT = etai / etat * sqrt(max(0.f, 1.0f - (cosi * cosi)));

	if (sinT >= 1.0f) { kr = 1.0f; } 
	else {
		float cosT = sqrt(max(0.f, 1.0f - (sinT * sinT)));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cosT)) / ((etat * cosi) + (etai * cosT));
		float Rp = ((etai * cosi) - (etat * cosT)) / ((etai * cosi) + (etai * cosT));
		kr = ((Rs * Rs) + (Rp * Rp)) / 2;
	}
}

vec3 snellRefract(const vec3 &i, const vec3 &N , const float &ior) {
	float cosi = clamp(-1.f, 1.f, dot(i, N));
	float etai = 1, etat = ior;
	vec3 n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);

	vec3 tmp;
	if (k < 0) {
		tmp = vec3(0);
	}
	else {
		tmp = eta * i + (eta * cosi - sqrt(k)) * n;
	}

	return tmp;
}

vec3 castRay(vec3 &orig, vec3 &dir, int depth, bool test = false) {
	
	if (depth > options.maxDepth) {
		return options.backgroundColor;
	}
	
	vec3 hitColor = options.backgroundColor;
	float tnear = INFINITY;
	vec2 uv;
	int index = 0;
	SceneObject *hitObj = nullptr;

	if (trace(orig, dir, tnear, index, uv, &hitObj)) {
		vec3 hitPoint = orig + dir * tnear;
		vec3 N;
		vec2 st;
		hitObj->getSurfaceProperties(hitPoint, dir, index, uv, N, st);
		switch (hitObj->materialType) {
			case REFLECTION_AND_REFRACTION:{
				vec3 reflectionDir = normalize(reflect(dir, N));
				vec3 refractionDir = normalize(snellRefract(dir, N, hitObj->ior));

				vec3 reflectionRayOrig = (dot(reflectionDir, N) < 0.0f) ? 
					hitPoint - N * options.bias : 
					hitPoint + N * options.bias;
				vec3 refractionRayOrig = (dot(refractionDir, N) < 0.0f) ? 
					hitPoint - N * options.bias : 
					hitPoint + N * options.bias;

				vec3 reflectionColor = castRay(reflectionRayOrig, reflectionDir, depth + 1, 1);
				vec3 refractionColor = castRay(refractionRayOrig, refractionDir, depth + 1, 1);

				float kr;
				fresnel(dir, N, hitObj->ior, kr);
				hitColor = reflectionColor * kr + refractionColor * (1.0f - kr);
				break;
			}
			case REFLECTION:{
				float kr;
				fresnel(dir, N, hitObj->ior, kr);
				vec3 reflectionDir = reflect(dir, N);
				vec3 reflectionRayOrig = (dot(reflectionDir, N) < 0.0) ? 
					hitPoint + N * options.bias : 
					hitPoint - N * options.bias;
				hitColor = castRay(reflectionRayOrig, reflectionDir, depth + 1) * kr;
				break;
			}
			default: {
				vec3 lightAmt(0), specular(0);
				vec3 shadowPointOrig = (dot(dir, N) < 0) ? 
					hitPoint + N * options.bias : 
					hitPoint - N * options.bias;

				for (int i = 0; i < sceneLights.size(); ++i) {
					vec3 lightDir = sceneLights.at(i)->pos - hitPoint;
					float lightDist2 = dot(lightDir, lightDir);
					lightDir = normalize(lightDir);
					float LdotN = max(0.f, dot(lightDir, N));
					SceneObject *shadowHitObj = nullptr;
					float tNearShadow = INFINITY;
					bool inShadow = trace(shadowPointOrig, lightDir, tNearShadow, index, uv, &shadowHitObj) && (tNearShadow * tNearShadow) < lightDist2;
					lightAmt += (1 - inShadow) * sceneLights.at(i)->intensity.x * LdotN;
					vec3 reflectionDir = reflect(-lightDir, N);
					specular += pow(max(0.f, -dot(reflectionDir, dir)), hitObj->specular) * sceneLights.at(i)->intensity.x;
				}

				hitColor = lightAmt * hitObj->evalDiffuseColor(st) * hitObj->kd + specular * hitObj->ks;
				break;
				
			}
		}
	}

	//Prints color
	//printf("%f, %f, %f\n" , hitColor.x, hitColor.y, hitColor.z);
	return hitColor;
}

void render() {
	vector<vec3> frameBuffer;
	vec3 pixel;

	float scale = tan(radians(options.fov * 0.5f));
	float imageAspectRatio = options.width / (float)options.height;
	vec3 orig(0);

	for (int j = 0; j < options.height; ++j) {
		for (int i = 0; i < options.width; ++i) {
			float x = (2.0f * (i + 0.5f) / (float)options.width - 1) * imageAspectRatio * scale;
			float y = (1.0f - 2.0f * (j + 0.5f) / (float)options.height) * scale;
			vec3 direction = normalize(vec3(x, y, -1));
			
			pixel = castRay(orig, direction, 0);
			frameBuffer.push_back(pixel);
			//printf("%f, %f, %f\n", pixel.x, pixel.y, pixel.z);
		}
	}

	//Save framebuffer to file
	ofstream ofs;
	ofs.open("./out.ppm");

	ofs << "P3\n" << options.width << " " << options.height << "\n255\n";

	for (int i = 0; i < frameBuffer.size(); i++) {
		int r = (255 * clamp(0.f, 1.f, frameBuffer.at(i).x));
		int g = (255 * clamp(0.f, 1.f, frameBuffer.at(i).y));
		int b = (255 * clamp(0.f, 1.f, frameBuffer.at(i).z));
		ofs << r << " " << g  << " " << b << "\n";
	}

	ofs.close();
	frameBuffer.clear();
}

void renderAA() {
	vector<vec3> frameBuffer;
	vec3 pixel;

	int sampleSize = 4;

	float qWidth = sampleSize * options.width, qHeight = sampleSize * options.height;

	float scale = tan(radians(options.fov * 0.5f));
	float imageAspectRatio = qWidth / (float)qHeight;
	vec3 orig(0);

	for (int j = 0; j < qHeight; ++j) {
		for (int i = 0; i < qWidth; ++i) {
			float x = (2.0f * (i + 0.5f) / (float)qWidth - 1) * imageAspectRatio * scale;
			float y = (1.0f - 2.0f * (j + 0.5f) / (float)qHeight) * scale;
			vec3 direction = normalize(vec3(x, y, -1));

			pixel = castRay(orig, direction, 0);
			frameBuffer.push_back(pixel);
			//printf("%f, %f, %f\n", pixel.x, pixel.y, pixel.z);
		}
	}

	printf("HQ Rendered\n");
	printf("Averaging Pixels\n");

	vector<vec3> AAImage;
	int index = 0;
	for (int i = 0; i <= frameBuffer.size() - sampleSize; i += sampleSize) { // iterate through all pixels
		vector<vec3> avePix;
		for (int x = 0; x < sampleSize; x++) {
			vec3 p1 = frameBuffer.at(i + x);
			vec3 p2 = frameBuffer.at(i + x + qWidth);
			vec3 p3 = frameBuffer.at(i + x + (qWidth * 2));
			vec3 p4 = frameBuffer.at(i + x + (qWidth * 3));

			avePix.push_back(p1);
			avePix.push_back(p2);
			avePix.push_back(p3);
			avePix.push_back(p4);
		}

		vec3 newPixel;
		int count = 0;
		for (vec3 pix : avePix) {
			newPixel += pix;
			count++;
		}

		newPixel /= count;
		AAImage.push_back(newPixel);
		
		if (index == qWidth - sampleSize) {
			i += (qWidth * sampleSize);
			index = 0;
		}
		else {
			index += sampleSize;
		}
	}
	printf("Averaged Pixels\n");

	//Save framebuffer to file
	ofstream ofs;
	ofs.open("./WhittedOutputAA.ppm");

	ofs << "P3\n" << options.width << " " << options.height << "\n255\n";

	for (int i = 0; i < AAImage.size(); i++) {
		int r = (255 * clamp(0.f, 1.f, AAImage.at(i).x));
		int g = (255 * clamp(0.f, 1.f, AAImage.at(i).y));
		int b = (255 * clamp(0.f, 1.f, AAImage.at(i).z));
		ofs << r << " " << g << " " << b << "\n";
	}

	ofs.close();
	frameBuffer.clear();
}

//Runs on startup
int main(int argc, char **argv) {
	printf("Is Running\n");

	defineScene();

	setOptions();

	render();

	exit(0);
}

