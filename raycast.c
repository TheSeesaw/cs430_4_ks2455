#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "file_io.h"
#include "raycast.h"


// calculates the distance between to points in space
double distance_between_points(Point *point_a, Point *point_b)
{
	// return the distance formula applied to points
  return sqrt(pow(point_a->x - point_b->x, 2)
         + pow(point_a->y - point_b->y, 2)
         + pow(point_a->z - point_b->z, 2));
}

// constructs an array of points through which rays can be cast to build a scene
void construct_view_plane(Point *view_plane1d, double res_width, double res_height, double cam_width, double cam_height)
{
  int index1d = 0;
  double pij_x = 0;
  double pij_y = 0;
  double pij_z = -1;
  // derive the Pij
  for (int i = 0 ; i < res_height; i++)
  {
    // calculate y coord
    pij_y = ((-0.5 * cam_height) + (i * (cam_height / res_height)) + (0.5 * (cam_height / res_height)));
    for (int j = 0; j < res_width; j++)
    {
      index1d = ((i * res_width) + j);
      // calculate x coord
      pij_x = ((-0.5 * cam_width) + (j * (cam_width / res_width)) + (0.5 * (cam_width / res_width)));
      // assign coordinates to view plane
      view_plane1d[index1d].x = pij_x;
      view_plane1d[index1d].y = pij_y;
      view_plane1d[index1d].z = pij_z;
    }
  }
}

// raytraces through a single point on the view plane to check intersection
// returns the distance to closest intersection
double sphere_intersection(Shape *sphere, Vector3d *normal_ray)
{
  Vector3d *sphere_pos = malloc(sizeof(Vector3d)); // initialize a vector for sphere position
  sphere_pos->x = sphere->pos_x; // save sphere position data in vector
  sphere_pos->y = sphere->pos_y;
  sphere_pos->z = sphere->pos_z;
  double dot_product = 0.0; // initialize variable to hold dot product result
  // compute dot product between sphere position and Rd
  dot_product = Vector3d_dot_prod(sphere_pos, normal_ray);
  free(sphere_pos); // done with sphere position vector
  if (dot_product <= 0) { // if closest intersection is 0 or negative
    return INFINITY; // return a miss
  } // else, a possible hit
  // create points for sphere center and point of closest intersection
  Point *sphere_center = malloc(sizeof(Point));
  Point *closest_intersection = malloc(sizeof(Point));
  // assign sphere center values
  sphere_center->x = sphere->pos_x;
  sphere_center->y = sphere->pos_y;
  sphere_center->z = sphere->pos_z;
  // scale each value of the the normal_ray by the dot product to get point of closest intersection
  closest_intersection->x = (normal_ray->x * dot_product);
  closest_intersection->y = (normal_ray->y * dot_product);
  closest_intersection->z = (normal_ray->z * dot_product);
  // calculate the distance between the sphere center and the closest intersection
  double distance_from_intersection_to_center = distance_between_points(sphere_center, closest_intersection);
  free(sphere_center); // done with intermediate structs
  free(closest_intersection);
  // if distance is less than sphere radius, it's a hit
  if (distance_from_intersection_to_center < sphere->radius)
  {
    //printf("HIT! Distance: %lf\n", distance_from_intersection_to_center);
    return distance_from_intersection_to_center; // return the distance
  }
  else // else, it's a miss
  {
    //printf("MISS!\n");
    return INFINITY;
  }
}

double plane_intersection(Shape *plane, Vector3d *normal_ray)
{
  Vector3d *plane_norm = malloc(sizeof(Vector3d)); // initialize a vector for plane normal
  plane_norm->x = plane->norm_x;
  plane_norm->y = plane->norm_y;
  plane_norm->z = plane->norm_z;
  // get the dot product of the normal ray and the plane's normal
  double norm_ray_dot_product = Vector3d_dot_prod(plane_norm, normal_ray);
  //printf("Plane normal and Normal Ray dot product result: %lf\n", norm_ray_dot_product);
  if (norm_ray_dot_product == 0) { // if that dot product results in a 0, return a miss
    //printf("MISS!\n");
    return INFINITY;
  }
  Vector3d *plane_pos = malloc(sizeof(Vector3d)); // initialize a vector for plane position
  plane_pos->x = plane->pos_x;
  plane_pos->y = plane->pos_y;
  plane_pos->z = plane->pos_z;
  // get the dot product of the plane's position and normal
  double pos_norm_dot_product = Vector3d_dot_prod(plane_norm, plane_pos);
  //printf("Plane normal and Plane Pos dot product result: %lf\n", pos_norm_dot_product);
  double intersection_test = pos_norm_dot_product / norm_ray_dot_product;
  //printf("Intersection test result: %lf\n", intersection_test);
  free(plane_pos); // done with intermediate structs
  free(plane_norm);
  if (intersection_test < 0) // miss
  {
    //printf("MISS!\n");
    return INFINITY;
  }
  else // else, a hit
  {
    //printf("HIT: %lf\n", intersection_test);
    return intersection_test;
  }
}

// takes in a shape and the view plane array,
// and passes them to the appropriate intersection test function
// returns distance to closest intersection if there was a hit,
// or -INFINITY if it was a miss
double intersection_test_director(Shape *current_shape, Vector3d *normal_ray)
{
  double intersection_test_result = INFINITY;
  if (current_shape->type == Sphere) // sphere intersection test
  {
    intersection_test_result = sphere_intersection(current_shape, normal_ray);
  }
  else if (current_shape->type == Plane) // plane intersection test
  {
    intersection_test_result = plane_intersection(current_shape, normal_ray);
  }
  else
  {
    fprintf(stderr, "ERROR: unrecognized shape.");
    return INFINITY;
  }
  return intersection_test_result;
}
