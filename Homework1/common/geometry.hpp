#ifndef GEOMETRY_H
#define GEOMETRY_H
#define My_PI 3.14159265358979323846


#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/model.hpp>

using namespace glm;

vec3 vertices[8] = {
	vec3(-0.5, -0.5, 0.5),
	vec3(-0.5, 0.5, 0.5),
	vec3(0.5, 0.5, 0.5),
	vec3(0.5, -0.5, 0.5),
	vec3(-0.5, -0.5, -0.5),
	vec3(-0.5, 0.5, -0.5),
	vec3(0.5, 0.5, -0.5),
	vec3(0.5, -0.5, -0.5)
};

// input: a_model (model's reference), a_a (positon of a), a_b (postion of b), a_c (position of c)
void ComputeNormal(Model &a_model, vec3 a_a, vec3 a_b, vec3 a_c)
{
	a_model.AddNormal(normalize(cross(a_b - a_a, a_c - a_a)));
	a_model.AddNormal(normalize(cross(a_b - a_a, a_c - a_a)));
	a_model.AddNormal(normalize(cross(a_b - a_a, a_c - a_a)));
}

// input: a_model (model's reference), a_a (positon of a), a_b (postion of b), a_c (position of c), a_d (position of d), a_color (color of quad)
void Quad(Model &a_model, int a_a, int a_b, int a_c, int a_d, vec3 a_color)
{
	/* a -- d
	 * |    |
	 * b -- c
	 */

	a_model.AddPosition(vertices[a_a]);
	a_model.AddPosition(vertices[a_b]);
	a_model.AddPosition(vertices[a_c]);
	a_model.AddPosition(vertices[a_a]);
	a_model.AddPosition(vertices[a_c]);
	a_model.AddPosition(vertices[a_d]);

	ComputeNormal(a_model, vertices[a_a], vertices[a_b], vertices[a_c]);
	ComputeNormal(a_model, vertices[a_a], vertices[a_c], vertices[a_d]);

	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
}
void Quad2(Model &a_model, int a_a, int a_b, int a_c, int a_d, vec3 a_color,vec3* positions)
{
	/* a -- d
	* |    |
	* b -- c
	*/

	a_model.AddPosition(positions[a_a]);
	a_model.AddPosition(positions[a_b]);
	a_model.AddPosition(positions[a_c]);
	a_model.AddPosition(positions[a_a]);
	a_model.AddPosition(positions[a_c]);
	a_model.AddPosition(positions[a_d]);

	ComputeNormal(a_model, positions[a_a], positions[a_b], positions[a_c]);
	ComputeNormal(a_model, positions[a_a], positions[a_c], positions[a_d]);

	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
}
// input: a_model (model's reference), a_a (positon of a), a_b (postion of b), a_c (position of c), a_d (position of d), a_color (color of quad)
void Tri(Model &a_model, int a_a, int a_b, int a_c,  vec3 a_color, vec3* positions)
{
	/*    a	
	*   / |
	* b - c
	*/

	a_model.AddPosition(positions[a_a]);
	a_model.AddPosition(positions[a_b]);
	a_model.AddPosition(positions[a_c]);
	
	ComputeNormal(a_model, positions[a_a], positions[a_b], positions[a_c]);
	
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
}
void circle(Model &a_model, vec3 centerpoint, int a_b, int a_c, vec3 a_color, vec3* positions) {
	// circle 
}
// input: a_model (model's reference), a_color (color of cube)
void InitDataCube(Model &a_model, vec3 a_color)
{
   /*   5 -- 6
    *  /|   /|
    * 1(4)-2 7
	* |    |/
	* 0 -- 3
	*/

	Quad(a_model, 1, 0, 3, 2, a_color);
	Quad(a_model, 2, 3, 7, 6, a_color);
	Quad(a_model, 3, 0, 4, 7, a_color);
	Quad(a_model, 6, 5, 1, 2, a_color);
	Quad(a_model, 4, 5, 6, 7, a_color);
	Quad(a_model, 5, 4, 0, 1, a_color);
}
vec3 positions[112];
// input: a_model (model's reference), a_colors (color of each face of cube)
void InitDataRubic(Model& a_model, vec3* a_colors)
{
	/*   5 -- 6
    *  /|   /|
    * 1(4)-2 7
	* |    |/
	* 0 -- 3
	*/
	Quad(a_model, 1, 0, 3, 2, a_colors[0]);
	Quad(a_model, 2, 3, 7, 6, a_colors[4]);
	Quad(a_model, 3, 0, 4, 7, a_colors[5]);
	Quad(a_model, 6, 5, 1, 2, a_colors[3]);
	Quad(a_model, 4, 5, 6, 7, a_colors[1]);
	Quad(a_model, 5, 4, 0, 1, a_colors[2]);
}


// input: a_model (model's reference), a_color (color of sphere)
void InitDataSphere(Model &a_model, vec3 a_color,float radius)
{
	// TODO: Fill the vertex position, normal, color for your sphere object
	// Reference: Four ways to create a mesh for a sphere
	// https://medium.com/game-dev-daily/four-ways-to-create-a-mesh-for-a-sphere-d7956b825db4
	int n1, n2, total_point, vps;
	n1 = 40; // number of horizontal lines
	n2 = 40; // number of vertical lines
	total_point = n1 * (n2 + 1) + 2;

	vec2* texcoords = (vec2*)malloc(sizeof(vec2)*total_point);
	vec3* positions = (vec3*)malloc(sizeof(vec3)*total_point);
	// North pole.
	//int radius = 1;
	positions[total_point - 2] = vec3(0, radius, 0);
	texcoords[total_point - 2] = vec2(0, 1);

	// South pole.
	positions[total_point - 1] = vec3(0, -radius, 0);
	texcoords[total_point - 1] = vec2(0, 0);

	// +1.0f because there's a gap between the poles and the first parallel.
	float latitudeSpacing = 1.0f / (n1 + 1.0f);
	float longitudeSpacing = 1.0f / (n2);

	// start writing new vertices at position 1
	int v = 0;
	int latitude, longitude;
	for (latitude = 0; latitude < n1; latitude++) {
		for (longitude = 0; longitude <= n2; longitude++) {

			// Scale coordinates into the 0...1 texture coordinate range,
			// with north at the top (y = 1).
			texcoords[v] = vec2(
				longitude * longitudeSpacing,
				1.0f - (latitude + 1) * latitudeSpacing
			);

			// Convert to spherical coordinates:
			// theta is a longitude angle (around the equator) in radians.
			// phi is a latitude angle (north or south of the equator).
			float theta = texcoords[v].x * 2.0f * My_PI;
			float phi = (texcoords[v].y - 0.5f) * My_PI;

			// This determines the radius of the ring of this line of latitude.
			// It's widest at the equator, and narrows as phi increases/decreases.
			float c = cos(phi);

			// Usual formula for a vector in spherical coordinates.
			// You can exchange x & z to wind the opposite way around the sphere.
			positions[v] = vec3(
				c * cos(theta),
				sin(phi),
				c * sin(theta)
			) * float(radius);
			//printf("%dth  %f %f %f \n", v, c*cos(theta), sin(phi), c*sin(theta));

			// Proceed to the next vertex.
			v++;
		}
	}
	int iternum = 0;
	for (vps = 0; vps < n1 - 1; vps++) {
		//for (vps = 0; vps<10-1; vps++) {
		for (int vis = 0; vis < n2; vis++) {
			//for (int vis = 0; vis < 10; vis++) {
			int a, b, c, d;
			if (vps == (n1 - 2) && vis == n2 - 1) {
				a = vps*(n2 + 1) + vis;
				b = vps*(n2 + 1) + vis + n2 + 1;
				c = vps*(n2 + 1) + vis + 1 + n2 + 1;
				d = vps*(n2 + 1) + vis + 1;
				Quad2(a_model, a, b, c, d, a_color, positions);
				// generating polse..;
				a = total_point - 2; //north pole
				b = total_point - 1; //south pole

				for (int vts = 0; vts < n2; vts++) {
					c = (vps + 1)*(1 + n2) + vts;
					d = (vps + 1)*(1 + n2) + vts + 1;
					Tri(a_model, c, b, d, a_color, positions);
					c = vts;
					d = vts + 1;
					Tri(a_model, d, a, c, a_color, positions);
					//printf("%d %d \n", c, d);
				}

			}
			else {
				a = vps*(n2 + 1) + vis;
				b = vps*(n2 + 1) + vis + n2 + 1;
				c = vps*(n2 + 1) + vis + 1 + n2 + 1;
				d = vps*(n2 + 1) + vis + 1;
				//printf("(%d %d %d %d) %d th iter / %d %d %f\n", a, b, c, d, iternum, total_point - 2,v,positions[v-1][0]);
				Quad2(a_model, a, b, c, d, a_color, positions);
				
				iternum++;
			}

		}

	}
	free(positions);
	free(texcoords);
	return;
	//Quad(a_model, 1, 0, 3, 2, a_color);
	
}
void InitDataPyramid(Model &a_model, vec3 a_color) {
	// ¹Ù´Ú ¸¸µé±â
	/*   v
	*  (4)\ -7
	* /    \/
	* 0 -- 3
	*/
	vec3 top = vec3(0.0f, 0.5f, 0.0f);
	vec3 vertices2[9] = {
		vec3(-0.5, -0.5, 0.5),
		vec3(-0.5, 0.5, 0.5),
		vec3(0.5, 0.5, 0.5),
		vec3(0.5, -0.5, 0.5),
		vec3(-0.5, -0.5, -0.5),
		vec3(-0.5, 0.5, -0.5),
		vec3(0.5, 0.5, -0.5),
		vec3(0.5, -0.5, -0.5),
		top
	};
	Quad(a_model, 3, 0, 4, 7, a_color);
	Tri(a_model, 3, 7, 8, a_color, vertices2);
	Tri(a_model, 0, 3, 8, a_color, vertices2);
	Tri(a_model, 7, 4, 8, a_color, vertices2);
	Tri(a_model, 4, 0, 8, a_color, vertices2);
}
#endif
void InitDataLine(Model &a_model, vec3 start, vec3 end, vec3 a_color)
{

	a_model.AddPosition(start);
	a_model.AddPosition(end);
	a_model.AddColor(a_color);
	a_model.AddColor(a_color);
}
