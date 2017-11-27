#include "vector.h"

typedef struct Point {
  double x, y, z;
} Point;

// calculates the distance between to points in space
double distance_between_points(Point *point_a, Point *point_b);

void construct_view_plane(Point *view_plane1d, double res_width, double res_height, double cam_width, double cam_height);

double sphere_intersection(Shape *sphere, Vector3d *normal_ray, Vector3d *intersect_strg);

double plane_intersection(Shape *plane, Vector3d *normal_ray, Vector3d *intersect_strg);

//double raycast_primitive(Vector3d *normal_ray, )

double intersection_test_director(Shape *current_shape, /*Point *origin,*/ Vector3d *normal_ray, Vector3d *intersect_strg);

// shifts intersection point toward origin slightly (0.00001 units), result saved
// in result_strg
void scooch(Vector3d *current_ray, Vector3d *result_strg);

// Takes in the current shape, shapes, lights, the intersection point with current shape,
// and a variable to store the final shade
// returns an int code for error checking
int light_intersect_director(Shape *current_shape, Shape *shapes, Light *lights, Vector3d *intersect_ray, Vector3d *shade_strg);
