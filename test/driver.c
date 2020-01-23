#include <stdio.h>

#include <RPS/libvector.h>

double f(double x) {
	return 1/x;
}

double f2(double x) {
	return (x-1) * (x-1) / (x*x - 4);
}

int main(void) {

	Figure *fig, *fig2, *fig3, *fig4, *fig5, *fig6;
	Color c = {.red = 23, .green = 0, .blue = 50};
	Point2D center = {.x =2, .y = 2};
	double radius = 2.0;
	Point2D start_roi, end_roi;

	fig = start_figure(10, 10);
	set_thickness_resolution(fig, 0.1, 1000);
	set_color(fig, c);

	fig2 = start_figure(10, 10);
	set_thickness_resolution(fig2, 0.1, 1000);
	set_color(fig2, c);

	fig3 = start_figure(10, 10);
	set_thickness_resolution(fig3, 0.1, 1000);
	set_color(fig3, c);

	fig4 = start_figure(300, 300);
	set_thickness_resolution(fig4, 5, 1000);
	set_color(fig4, c);

	fig6 = start_figure(300, 300);
	set_thickness_resolution(fig6, 5, 1000);
	set_color(fig6, c);

	draw_fx(fig, f2, -5, 5);
	export_eps(fig, "demo/single-1.eps");	/*expecting 1 figure of function f2*/

	draw_fx(fig2, f, -5, 5);
	export_eps(fig2, "demo/single-2.eps");	/*expecting 1 figure of function f*/

	append_figures(fig, fig2);
	export_eps(fig, "demo/multi-1-2.eps");	/*expecting 2 figure of functions f and f2*/

	draw_circle(fig3, &center, radius);
	export_eps(fig3, "demo/single-3.eps");		/*expecting 1 figure of circle a bit top and right of origin (center (2,2))*/

	append_figures(fig2, fig3);			/*linked fig3 and fig2*/
	export_eps(fig, "demo/multi-1-2-3.eps");		/*since fig2 is already linked with fig, with exporting fig, expecting 3 figure of f, f2 and circle*/

	scale_figure(fig3, 1, 0.5);
	export_eps(fig, "demo/2multi-1-2-3.eps");	/*expecting 3 figure of f, f2 and circle but now circle should have seem like flattened from it is top, like an ellipse*/

	start_roi.x = 0;
	start_roi.y = 0;
	end_roi.x = 2;
	end_roi.y = 2;

	resize_figure(fig3, start_roi, end_roi);
	export_eps(fig, "demo/3multi-1-2-3.eps");	/*expecting 3 figure of f, f2 and circle but now we should only see its left part since it's origin was (2,2) and we resized it to point (0,0) - (2,2) -->(smaller)*/

	start_roi.x = 0;
	start_roi.y = 0;
	end_roi.x = 3.5;
	end_roi.y = 3.5;

	resize_figure(fig3, start_roi, end_roi);
	export_eps(fig, "demo/4multi-1-2-3.eps");	/*expecting 3 figure of f, f2 and circle but now we should see its ending part is not adjacent since it's origin was (2,2) and we resized it to point (0,0) - (3.5,3.5) -->(bigger)*/

	center.x = -60;
	center.y = 20;
	draw_ellipse(fig4, &center, 50, 100);
	export_svg(fig4, "demo/single-4.svg");	/*expecting 1 figure of ellipse that flattened from the sides centered left top side of the image exported to SVG*/

	fig5 = start_figure(1,1);	/*we have to allocate memory for fig5 before importing, this can be done either sending it to start_figure functions with random values except 0s
									or user could malloc it but user doesn't know the struct. I couldn't allocate memory since we would be sending NULL value so we should have send a double pointer to import function*/
	import_svg("demo/single-4.svg", fig5);
	export_svg(fig5, "demo/single-5.svg");	/*since we imported points of fig4 to fig5, we expect the same figure as fig4*/

	draw_fx(fig, f, -5, 5);
	draw_polyline(fig6, fig->point, 901);	/*using points of fig as poly points, i counted how many points they were frome the file myself for demo purpose*/ 
	scale_figure(fig6, 40, 40);		/*i scale it cause 10,10 paper size for svg file seems very small so i scale it to sizes of fig6 */
	export_svg(fig6, "demo/single-6.svg");	/*expecting 1 figure of function f*/

	freeFigure(&fig6);
	freeFigure(&fig5);
	freeFigure(&fig4);
	freeFigure(&fig3);
	freeFigure(&fig2);
	freeFigure(&fig);

	return 0;
}
