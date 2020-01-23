#ifndef _LIBVECTOR_H_
#define _LIBVECTOR_H_

	#define PRECISION 0.0000001
	#define PRECISION2 10000
	#define PRECISION3 0.001
	#define NO 0
	#define YES 1

	enum {
		scaled = -1, draw, pass, shape_end
	};

	typedef struct {
		double red;
		double green;
		double blue;
	}Color;

	typedef struct p2D{
		double x, y;
		int passNext;
		struct p2D *next;
	}Point2D;

	typedef struct figure_s{
		double width, height;
		double llx, lly, urx, ury;
		double thickness, resolution;
		Color colorRGB;
		Point2D *attachedPoint;
		Point2D *point;
	}Figure;


	/*Public functions*/
	Figure * start_figure(double width, double height);
	void set_thickness_resolution(Figure *fig, double thickness, double resolution);
	void set_color(Figure *fig, Color c);
	void draw_fx(Figure *fig, double f(double x), double start_x, double end_x);
	void draw_polyline(Figure *fig, Point2D *poly_line, int n);
	void draw_circle(Figure *fig, Point2D * center, double radius);
	void draw_ellipse(Figure *fig, Point2D *center, double major, double minor);
	void scale_figure(Figure *fig, double scale_x, double scale_y);
	void resize_figure(Figure *fig, Point2D start_roi, Point2D end_roi);
	void append_figures(Figure *fig1, Figure* fig2);
	void export_eps(Figure *fig, char *file_name);
	void export_svg(Figure *fig, char *file_name);
	void import_svg(char *file_name, Figure *fig);


	/*Private functions*/
	double fEllipseTop(Point2D *center, double major, double minor, double x);
	double fEllipseBottom(Point2D *center, double major, double minor, double x);
	void set_boundaries(double lly, double ury, double *figPointY, double tempY);
	void set_point(Point2D *figPoint, double x, double y, double passValue);
	double go_normal_point(double currentX, double lly, double ury, double interval, double f(double x), double real_ex);
	double go_normal_point_ellipse(double currentX, double lly, double ury, double interval, double f(Point2D *center, double major, double minor, double x), double real_ex, Point2D *center, double major, double minor);
	void getEllipse(Figure *fig, double f(Point2D *center, double major, double minor, double x), double start_x, double end_x, Point2D *center, double major, double minor);
	void freeFigure(Figure **fig);
	void freePoints(Point2D **headPoint);

#endif
