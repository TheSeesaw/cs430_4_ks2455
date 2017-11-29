// program takes in a CSV file and creates an image based on the drawn objects
// By: Kristoffer Schindele
#include <stdlib.h>
#include <math.h>
#include "file_io.h"
#include "raycast.h"

// GLOBAL VARIABLES //
int res_width, res_height;
int total_objects[2] = {0};
Shape *camera = NULL;
Shape *shapes_list = NULL;
Light *lights_list = NULL;
Point *view_plane = NULL;
Pixel *pixel_plane = NULL;

// main function for the raycaster performs the following:
//  - reads a csv file for objects in a scene
//  - perform raycasting to build pixel buffer
//  - write pixel buffer to a P6 format .ppm file
int main(int argc, char *argv[]) {
	// argument checking
	if (argc != 5) {
		fprintf(stderr, "Error: Invalid number of arguments.\n");
		return 1;
	}
	else if (argv[3] == NULL) {
		fprintf(stderr, "Error: Problem reading input file.\n");
		return 1;
	}
	else if (argv[4] == NULL) {
		fprintf(stderr, "Error: Problem reading output file.\n");
		return 1;
	}
	shapes_list = malloc(128*sizeof(Shape)); // initialize shapes list
	lights_list = malloc(128*sizeof(Light)); // initialize lights list
	camera = malloc(sizeof(Shape)); // initialize camera object
	total_objects[0] = 0; // initialize shape count
	total_objects[1] = 0; // initialize light count
  // Read file
	read_object_file_director(argv[3], camera, shapes_list, lights_list, total_objects);
	// read width and height
  res_width = atoi(argv[1]);
  res_height = atoi(argv[2]);
	view_plane = malloc(res_width * res_height * sizeof(Point));
	pixel_plane = malloc(res_width * res_height * sizeof(Pixel));
  // Construct the view plane coordinates based on image resolution and camera dimensions
	construct_view_plane(view_plane, res_width, res_height, camera->width, camera->height);
	// Perform raycasting
	int view_plane1d_length = res_width * res_height;
	int closest_intersection_index;
	double intersection_test_result;
	double closest_intersection_dist;
	Vector3d *closest_intersection_ray = malloc(sizeof(Vector3d));
	Vector3d *intersection_ray = malloc(sizeof(Vector3d));
	Vector3d *normalized_ray = malloc(sizeof(Vector3d));
	Vector3d *the_origin = malloc(sizeof(Vector3d));
	Vector3d *rd = malloc(sizeof(Vector3d));
	the_origin->x = 0;
	the_origin->y = 0;
	the_origin->z = 0;
	Pixel *final_color = malloc(sizeof(Pixel));
	// Loop through all the points in the view plane
	for (int view_plane_index = 0; view_plane_index < view_plane1d_length; view_plane_index++)
	{
		intersection_test_result = INFINITY; // re-initialize intersection_test_result
		closest_intersection_dist = INFINITY;
		closest_intersection_index = 0; // re-initialize the closest shape to the first one
		// convert view plane point to a vector
		rd->x = view_plane[view_plane_index].x;
		rd->y = view_plane[view_plane_index].y;
		rd->z = view_plane[view_plane_index].z;
		// normalize the current Pij
		normalize_ray(the_origin, rd, normalized_ray);
		// raycast for target point in view plane for all shapes
		for (int s_index = 0; s_index < total_objects[0]; s_index += 1)
		{
			intersection_test_result = intersection_test_director(&shapes_list[s_index],
																														the_origin,
																														normalized_ray,
																														intersection_ray);
			if (intersection_test_result < closest_intersection_dist)
			{
				// update the closest intersection for this pixel
				closest_intersection_dist = intersection_test_result;
				closest_intersection_index = s_index;
				closest_intersection_ray->x = intersection_ray->x;
				closest_intersection_ray->y = intersection_ray->y;
				closest_intersection_ray->z = intersection_ray->z;
			}
		}
		// After testing intersection for all shapes, check result,
		// and save color result in the pixel_plane
		if (closest_intersection_dist == INFINITY) // no intersections
		{ // set pixel color to black
			pixel_plane[view_plane_index].r = 0;
			pixel_plane[view_plane_index].g = 0;
			pixel_plane[view_plane_index].b = 0;
		}
		else // set the pixel's color to the color of the closest shape
		{
			// perform illumination calculations TODO: finish params for following function
			/*int light_intersect_result = light_intersect_director(&shapes_list[closest_intersection_index],
				 																										shapes_list,
																														lights_list,
																														total_objects,
																														normalized_ray,
																														final_color);
																														*/
			// convert color from decimal scale to 24 bit rgb
			// TODO: assign final color to pixel
			pixel_plane[view_plane_index].r = (int)(shapes_list[closest_intersection_index].d_col_r * 255);
			pixel_plane[view_plane_index].g = (int)(shapes_list[closest_intersection_index].d_col_g * 255);
			pixel_plane[view_plane_index].b = (int)(shapes_list[closest_intersection_index].d_col_b * 255);
		}
	}
  // Write results
	write_pixels_to_p6(pixel_plane, argv[4], res_width, res_height);
	// Free memory
	free(rd);
	free(the_origin);
	free(normalized_ray);
	free(closest_intersection_ray);
	free(pixel_plane);
	free(view_plane);
	free(camera);
	free(lights_list);
	free(shapes_list);
	return 0;
}
