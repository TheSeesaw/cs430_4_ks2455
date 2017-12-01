#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "vector.h" // include vectors for shape positions

// enum for object type
enum ObjType { Camera, Sphere, Plane };
// struct representing pixel
typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

typedef struct Shape {
	enum ObjType type;
	double width, height, radius;
	double pos_x, // position
				 pos_y,
				 pos_z,
				 d_col_r, //diffuse color
				 d_col_g,
				 d_col_b,
				 s_col_r, // specular color
				 s_col_g,
				 s_col_b,
				 norm_x, // normal, for planes
				 norm_y,
				 norm_z;
} Shape;

typedef struct Light {
	double col_r,
				 col_g,
				 col_b,
				 theta,
				 radial_a2,
				 radial_a1,
				 radial_a0,
				 pos_x,
				 pos_y,
				 pos_z;
} Light;


void traverse_whitespace_and_comments(FILE *file_to_read);

void read_camera_data(FILE *file_to_read, Shape *camera);

void read_sphere_data(FILE *file_to_read, Shape *output_list, int obj_index);

void read_plane_data(FILE* file_to_read, Shape* output_list, int obj_index);

void read_light_data(FILE* file_to_read, Light* light_list, int light_index);

int read_object_file_director(char *in_file_name, Shape *camera, Shape *output_list, Light *light_list, int *obj_count_array);

int write_pixels_to_p6(Pixel *buffer, char *out_file_name, int res_width, int res_height);
