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
double sphere_intersection(Shape *sphere, /*Point *origin,*/ Vector3d *normal_ray, Vector3d *intersect_strg)
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
  // store the closest intersection in intersect_strg
  intersect_strg->x = closest_intersection->x;
  intersect_strg->y = closest_intersection->y;
  intersect_strg->z = closest_intersection->z;
  // calculate the distance between the sphere center and the closest intersection
  double distance_from_intersection_to_center = distance_between_points(sphere_center, closest_intersection);
  // update intersect_strg with point of closest intersection
  intersect_strg->x = closest_intersection->x;
  intersect_strg->y = closest_intersection->y;
  intersect_strg->z = closest_intersection->z;
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

double plane_intersection(Shape *plane, /*Point *origin,*/ Vector3d *normal_ray, Vector3d *intersect_strg)
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
    // scale normal ray by t to get point of closest intersection, store for later use
    intersect_strg->x = intersection_test * normal_ray->x;
    intersect_strg->y = intersection_test * normal_ray->y;
    intersect_strg->z = intersection_test * normal_ray->z;
    //printf("HIT: %lf\n", intersection_test);
    return intersection_test;
  }
}

// takes in a shape and the view plane array,
// and passes them to the appropriate intersection test function
// returns distance to closest intersection if there was a hit,
// or -INFINITY if it was a miss
double intersection_test_director(Shape *current_shape, /*Point *origin,*/ Vector3d *normal_ray, Vector3d *intersect_strg)
{
  double intersection_test_result = INFINITY;
  if (current_shape->type == Sphere) // sphere intersection test
  {
    intersection_test_result = sphere_intersection(current_shape, /*origin,*/ normal_ray, intersect_strg);
  }
  else if (current_shape->type == Plane) // plane intersection test
  {
    intersection_test_result = plane_intersection(current_shape, /*origin,*/ normal_ray, intersect_strg);
  }
  else
  {
    fprintf(stderr, "ERROR: unrecognized shape.");
    return INFINITY;
  }
  return intersection_test_result;
}

void scooch(Vector3d *current_ray, Vector3d *result_strg)
{
  double scooch_val = 0.00001;
  // apply schooch value to current_ray
  result_strg->x = current_ray->x - scooch_val;
  result_strg->y = current_ray->y - scooch_val;
  result_strg->z = current_ray->z + scooch_val;
}

void normalize_ray(Vector3d *origin, Vector3d *rd, Vector3d *result)
{
  Point *origin_pt = malloc(sizeof(Point));
  Point *trg_pt = malloc(sizeof(Point));
  double ray_length = 0.0;
  double w_x = rd->x;
  double w_y = rd->y;
  double w_z = rd->z;
  // first, cast vectors to points for distance formula
  origin_pt->x = origin->x;
  origin_pt->y = origin->y;
  origin_pt->z = origin->z;
  trg_pt->x = rd->x;
  trg_pt->y = rd->y;
  trg_pt->z = rd->z;
  // get the distance between origin and target (ray length)
  ray_length = distance_between_points(origin_pt, trg_pt);
  // scale the target vector
  w_x /= ray_length; // workaround for strange behavior when assigning floating
  w_y /= ray_length; // point division to struct fields
  w_z /= ray_length;
  result->x = w_x;
  result->y = w_y;
  result->z = w_z;
  // normalized ray is now stored in result
  free(origin_pt);
  free(trg_pt);
}

void construct_rd(Vector3d *origin, double trg_x, double trg_y, double trg_z, Vector3d *rd_strg)
{
  double o_x, o_y, o_z, rd_x, rd_y, rd_z;
  o_x = origin->x; // store vector values in doubles to avoid strange struct
  o_y = origin->y; // field math
  o_z = origin->z;
  rd_x = trg_x - o_x;
  rd_y = trg_y - o_y;
  rd_z = trg_z - o_z;
  rd_strg->x = rd_x;
  rd_strg->y = rd_y;
  rd_strg->z = rd_z;
}

int light_intersect_director(Shape *current_shape, Shape *shapes, Light *lights, Vector3d *intersect_ray, Pixel *shade_strg)
{
  // declare working variables
  int shape_index;
  int intersect_switch;
  int light_index = 0;
  int light_list_length = sizeof(lights) / sizeof(lights[0]);
  int shapes_list_length = sizeof(shapes) / sizeof(shapes[0]);
  double l_x, l_y, l_z, r_light, g_light, b_light;
  Vector3d *new_origin = malloc(sizeof(Vector3d));
  Vector3d *new_rd = malloc(sizeof(Vector3d));
  Vector3d *new_normal_ray = malloc(sizeof(Vector3d));
  // scooch intersection point to avoid clipping errors
  scooch(intersect_ray, new_origin);
  // loop through all lights
  for (; light_index < light_list_length; light_index += 1)
  {
    // calculate new Rd
    l_x = lights[light_index].pos_x; // pull values out of the struct
    l_y = lights[light_index].pos_y;
    l_z = lights[light_index].pos_z;
    construct_rd(new_origin, l_x, l_y, l_z, new_rd);
    // create a normalized ray between the intersection point and light
    normalize_ray(new_origin, new_rd, new_normal_ray);
    printf("light %d : ", light_index);
    // for each light, loop through all shapes
    shape_index = 0;
    intersect_switch = 0; // 0 for false intersection
    r_light = 0;
    g_light = 0;
    b_light = 0;
    for (; shape_index < shapes_list_length; shape_index += 1)
    {
      printf("shape %d ", shape_index);
      // check for intersections with each shape
      // if intersect, contribute to 0 to final shade
      // break from loop as soon as intersection is found
      // else, continue
    }
    printf("\n");
    if (intersect_switch == 0) // no intersections, light contributes
    {
      // call shade function for this light
      // TODO: shade(r_light, g_light, b_light);
    }
    // else, intersections, no light contribution
    // add light contribution to final color for pixel
    // store final color in shade_strg when all light contribution has been calculated
  }
  free(new_origin);
  free(new_normal_ray);
  return 0;
}
