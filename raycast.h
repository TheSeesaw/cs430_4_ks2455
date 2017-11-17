#include "vector.h"

typedef struct Point {
  double x, y, z;
} Point;

// calculates the distance between to points in space
double distance_between_points(Point *point_a, Point *point_b);

void construct_view_plane(Point *view_plane1d, double res_width, double res_height, double cam_width, double cam_height);

double sphere_intersection(Shape *sphere, Vector3d *normal_ray);

double plane_intersection(Shape *plane, Vector3d *normal_ray);

//double raycast_primitive(Vector3d *normal_ray, )

double intersection_test_director(Shape *current_shape, Vector3d *normal_ray);
