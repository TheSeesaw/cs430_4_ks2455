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
double sphere_intersection(Shape *sphere, Vector3d *ro, Vector3d *normal_ray, Vector3d *intersect_strg)
{
  // pull values out of structs for calculations
  double rd_x, rd_y, rd_z, ro_x, ro_y, ro_z, s_x, s_y, s_z, r_x, r_y, r_z, quad_b, quad_c, sr2, t0, t1, discriminant, tf;
  rd_x = normal_ray->x;
  rd_y = normal_ray->y;
  rd_z = normal_ray->z;
  ro_x = ro->x;
  ro_y = ro->y;
  ro_z = ro->x;
  s_x = sphere->pos_x;
  s_y = sphere->pos_y;
  s_z = sphere->pos_z;
  // intermediate calculations
  sr2 = 0; // TODO how do I calculate Sr^2?
  quad_b = 2 * ((rd_x * (ro_x - s_x)) + (rd_y * (ro_y - s_y)) + (rd_z * (ro_z - s_z)));
  quad_c = pow((ro_x - s_x), 2) + pow((ro_y - s_y), 2) + pow((ro_z - s_z), 2) - sr2;
  // calculate the discriminant (note quad_a will always be 1)
  discriminant = sqrt(pow(quad_b, 2) - 4 * quad_c);
  if (discriminant < 0) { // no intersection
    return INFINITY; // miss
  }
  // intersection is possible, calculate t0 and t1
  t0 = -1 * quad_b - discriminant;
  t1 = -1 * quad_b + discriminant;
  // switch on which t value to use
  if (t0 >= 0 && t1 >= 0) { // both are positive, use the smaller of the two
    if (t0 < t1) {
      tf = t0;
    }
    else {
      tf = t1;
    }
  }
  else if (t0 >= 0) {
    tf = t0;
  }
  else if (t1 >= 0) {
    tf = t1;
  }
  else { // both negative
    return INFINITY;
  }
  // assign results
  r_x = ro_x + (rd_x * tf);
  r_y = ro_y + (rd_y * tf);
  r_z = ro_z + (rd_z * tf);
  intersect_strg->x = r_x;
  intersect_strg->y = r_y;
  intersect_strg->z = r_z;
  printf("HIT!: %lf\n",tf);
  return tf;
}

double plane_intersection(Shape *plane, Vector3d *ro, Vector3d *normal_ray, Vector3d *intersect_strg)
{
  Vector3d *plane_norm = malloc(sizeof(Vector3d)); // initialize a vector for plane normal
  plane_norm->x = plane->norm_x;
  plane_norm->y = plane->norm_y;
  plane_norm->z = plane->norm_z;
  // get the dot product of the normal ray and the plane's normal (denominator)
  double norm_ray_dot_product = Vector3d_dot_prod(plane_norm, normal_ray);
  Vector3d *plane_pos = malloc(sizeof(Vector3d)); // initialize a vector for plane position
  plane_pos->x = plane->pos_x;
  plane_pos->y = plane->pos_y;
  plane_pos->z = plane->pos_z;
  // get the dot product of the plane's position and normal (d value)
  double pos_norm_dot_product = Vector3d_dot_prod(plane_norm, plane_pos);
  Vector3d *ro_plus_d = malloc(sizeof(Vector3d)); // initialize vector for Ro + D
  ro_plus_d->x = ro->x + pos_norm_dot_product;
  ro_plus_d->y = ro->y + pos_norm_dot_product;
  ro_plus_d->z = ro->z + pos_norm_dot_product;
  // get the dot product of ro_plus_d and the plane's normal (numerator)
  double norm_ro_d_dot_product = Vector3d_dot_prod(plane_norm, ro_plus_d);
  // test for intersection
  double tf = (-1 * norm_ro_d_dot_product) / norm_ray_dot_product;
  if (tf < 0) {
    return INFINITY; // miss
  }
  // calculate intersection point
  double r_x, r_y, r_z;
  r_x = ro->x + (normal_ray->x * tf);
  r_y = ro->y + (normal_ray->y * tf);
  r_z = ro->z + (normal_ray->z * tf);
  intersect_strg->x = r_x;
  intersect_strg->y = r_y;
  intersect_strg->z = r_z;
  printf("HIT!: %lf\n",tf);
  return tf;
}

// takes in a shape and the view plane array,
// and passes them to the appropriate intersection test function
// returns distance to closest intersection if there was a hit,
// or -INFINITY if it was a miss
double intersection_test_director(Shape *current_shape, Vector3d *ro, Vector3d *normal_ray, Vector3d *intersect_strg)
{
  double intersection_test_result = INFINITY;
  if (current_shape->type == Sphere) // sphere intersection test
  {
    intersection_test_result = sphere_intersection(current_shape, ro, normal_ray, intersect_strg);
  }
  else if (current_shape->type == Plane) // plane intersection test
  {
    intersection_test_result = plane_intersection(current_shape, ro, normal_ray, intersect_strg);
  }
  else
  {
    fprintf(stderr, "ERROR: unrecognized shape.");
    return INFINITY;
  }
  return intersection_test_result;
}

// TODO redo scooch to scooch along the normal of the intersection before recursive raytracing
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

double f_ang(Light *light, Shape *current_shape)
{
  // check if spot light
  if (light->theta != 0)
  {
    // check if object in light cone
    return 0;
  }
  else // point light
  {
    return 1.0;
  }
}

double f_rad(Light *light, Shape *current_shape)
{
  // get distance to iso-contour
  Point *light_pos = malloc(sizeof(Point));
  Point *shape_pos = malloc(sizeof(Point));
  light_pos->x = light->pos_x;
  light_pos->y = light->pos_y;
  light_pos->z = light->pos_z;
  shape_pos->x = current_shape->pos_x;
  shape_pos->y = current_shape->pos_y;
  shape_pos->z = current_shape->pos_z;
  double d_l = distance_between_points(light_pos, shape_pos);
  free(light_pos);
  free(shape_pos);
  if (d_l > 1000000) { // effectively 'far away'
    return 1.0;
  }
  else {
    // attenuation calculation
    double f_rad_result = 1 / ((light->radial_a2 * pow(d_l, 2)) + (light->radial_a1 * d_l) + light->radial_a0);
    return f_rad_result;
  }
}

double diff_ref(Light *light, Shape *current_shape)
{
  return 0.1;
}

double spec_ref(Light *light, Shape *current_shape)
{
  return 0.1;
}

void shade(Shape *current_shape, Light *light, Vector3d *shade_strg)
{
  double r_light, g_light, b_light, ang_att, rad_att, diffuse, spec, total_ill;
  // angular attenuation
  ang_att = f_ang(light, current_shape);
  // radial attenuation
  rad_att = f_rad(light, current_shape);
  // diffuse reflection
  diffuse = diff_ref(light, current_shape);
  // specular reflection
  spec = spec_ref(light, current_shape);
  // simple illumination model
  total_ill = ang_att * rad_att * (diffuse + spec);
  // scale light colors by total illumination, apply to shade
  shade_strg->x += light->col_r * total_ill;
  shade_strg->y += light->col_g * total_ill;
  shade_strg->z += light->col_b * total_ill;
}

int light_intersect_director(Shape *current_shape, Shape *shapes, Light *lights, int *obj_count_array, Vector3d *intersect_ray, Vector3d *shade_strg)
{
  // declare working variables
  printf("objects: %d\n", obj_count_array[0]);
  printf("lights: %d\n", obj_count_array[1]);
  int shape_index;
  int intersect_switch;
  int light_index = 0;
  int light_list_length = sizeof(lights) / sizeof(lights[0]);
  int shapes_list_length = sizeof(shapes) / sizeof(shapes[0]);
  double l_x, l_y, l_z, r_light, g_light, b_light, intersect_result;
  Vector3d *new_origin = malloc(sizeof(Vector3d));
  Vector3d *new_rd = malloc(sizeof(Vector3d));
  Vector3d *new_normal_ray = malloc(sizeof(Vector3d));
  Vector3d *intersect_point = malloc(sizeof(Vector3d));
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
    intersect_result = INFINITY; // reset intersection result
    r_light = 0;
    g_light = 0;
    b_light = 0;
    for (; shape_index < shapes_list_length; shape_index += 1)
    {
      printf("shape %d ", shape_index);
      // check for intersections with each shape
      intersect_result = intersection_test_director(&shapes[shape_index], new_origin, new_normal_ray, intersect_point);
      if (intersect_result < INFINITY)// if intersect, contribute to 0 to final shade
      {
        intersect_switch = 1; // set intersection switch for shading
        break; // break from loop as soon as intersection is found
      } // else, continue
    }
    printf("\n");
    if (intersect_switch == 0) // no intersections, light contributes
    {
      printf("no instersections, light contributes\n");
      // call shade function for this light
      shade(&shapes[shape_index], &lights[light_index], shade_strg);
    }
  }
  free(intersect_point);
  free(new_origin);
  free(new_rd);
  free(new_normal_ray);
  return 0;
}
