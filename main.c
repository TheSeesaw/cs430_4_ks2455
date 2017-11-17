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
	Point *origin = malloc(sizeof(Point));
	origin->x = 0;
	origin->y = 0;
	origin->z = 0;
	double ray_length;
	double intersection_test_result;
	double closest_intersection;
	Vector3d *normalized_ray = malloc(sizeof(Vector3d));
	// Loop through all the points in the view plane
	for (int view_plane_index = 0; view_plane_index < view_plane1d_length; view_plane_index++)
	{
		intersection_test_result = INFINITY; // re-initialize intersection_test_result
		closest_intersection = INFINITY;
		closest_intersection_index = 0; // re-initialize the closest shape to the first one
		// normalize the current Pij
		ray_length = distance_between_points(origin, &view_plane[view_plane_index]);
		normalized_ray->x = view_plane[view_plane_index].x / ray_length;
		normalized_ray->y = view_plane[view_plane_index].y / ray_length;
		normalized_ray->z = view_plane[view_plane_index].z / ray_length;
		// raytrace for target point in view plane for all shapes
		for (int s_index = 0; s_index < total_objects[0]; s_index += 1)
		{
			intersection_test_result = intersection_test_director(&shapes_list[s_index], normalized_ray);
			if (intersection_test_result < closest_intersection)
			{
				// update the closest intersection for this pixel
				closest_intersection = intersection_test_result;
				closest_intersection_index = s_index;
			}
		}
		// After testing intersection for all shapes, check result,
		// and save color result in the pixel_plane
		if (closest_intersection == INFINITY) // no intersections
		{ // set pixel color to black
			pixel_plane[view_plane_index].r = 0;
			pixel_plane[view_plane_index].g = 0;
			pixel_plane[view_plane_index].b = 0;
		}
		else // set the pixel's color to the color of the closest shape
		{ // convert color from decimal scale to 24 bit rgb
			// TODO: apply specular light
			pixel_plane[view_plane_index].r = (int)(shapes_list[closest_intersection_index].d_col_r * 255);
			pixel_plane[view_plane_index].g = (int)(shapes_list[closest_intersection_index].d_col_g * 255);
			pixel_plane[view_plane_index].b = (int)(shapes_list[closest_intersection_index].d_col_b * 255);
		}
	}
  // Write results
	write_pixels_to_p6(pixel_plane, argv[4], res_width, res_height);
	// Free memory
	free(normalized_ray);
	free(origin);
	free(pixel_plane);
	free(view_plane);
	free(camera);
	free(lights_list);
	free(shapes_list);
	return 0;
}
