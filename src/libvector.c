#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <RPS/libvector.h>

void set_point(Point2D *figPoint, double x, double y, double passValue) {

	figPoint->x = x;
	figPoint->y = y;
	figPoint->passNext = passValue;
}

/*this function goes to a normal point (non indefinite, infinite)*/
double go_normal_point(double currentX, double lly, double ury, double interval, double f(double x), double real_ex) {

	double tempY, tempInterval = interval / PRECISION2;	/*tempInterval is something for increasing precision of the output*/

	tempY = f(currentX + interval);
	while((isfinite(tempY) == NO || tempY < lly || tempY > ury) && currentX + interval <= real_ex) {	/*goes to a point where the next point is finite, fastly*/
		currentX += interval;
		tempY = f(currentX + interval);
	}

	tempY = f(currentX + tempInterval);
	while((isfinite(tempY) == NO || tempY < lly || tempY > ury) && currentX <= real_ex) {	/*goes to that finite number's first occurence as tempInterval can do, slowly - high precision*/
		currentX += tempInterval;
		tempY = f(currentX + tempInterval);
	}

	return currentX;
}

Figure * start_figure(double width, double height) {

	if(width <= 0.0 || height <= 0.0) {
		printf("Error. Figure width or height can't be less or equal to 0.\n");
		exit(EXIT_FAILURE);
	}

	Figure *fig = NULL;

	fig = (Figure*)malloc(sizeof(Figure));
	if(fig == NULL) {
		perror("\nError. ");
		exit(EXIT_FAILURE);
	}

	fig->width = width;
	fig->height = height;

	fig->llx = -width / 2.0;	/*origin specs*/
	fig->lly = -height / 2.0;
	fig->urx = width / 2.0;
	fig->ury = height / 2.0;

	fig->thickness = 0;
	fig->resolution = 1;	/*default resolution*/

	fig->colorRGB.red = 0;
	fig->colorRGB.green = 0;
	fig->colorRGB.blue = 0;

	fig->point = NULL;
	fig->attachedPoint = NULL;

	return fig;
}

void set_thickness_resolution(Figure *fig, double thickness, double resolution) {

	if(resolution == 0) { //check if resolution can be zero
		printf("\nError. Resolution can't be zero.\n");
		exit(EXIT_FAILURE);
	}

	fig->thickness = thickness;
	fig->resolution = resolution;
}

void set_color(Figure *fig, Color c) {
	if(c.red < 0 || c.red > 100 || c.blue < 0 || c.blue > 100 || c.green < 0 || c.green > 100) {
		printf("Please give the color values between 0-100 inclusively. Default values applied.\n");
	}
	else {
		fig->colorRGB = c;
	}
}

double fEllipseTop(Point2D *center, double major, double minor, double x) {
	return sqrt((minor * minor) * ( 1 - ((x - center->x) * (x - center->x)) / (major * major))) + center->y;
}

double fEllipseBottom(Point2D *center, double major, double minor, double x) {
	return -sqrt((minor * minor) * ( 1 - ((x - center->x) * (x - center->x)) / (major * major))) + center->y;
}

/*sets a y value to figure's boundaries if the value exceeds them*/
void set_boundaries(double lly, double ury, double *figPointY, double tempY) {

	if(tempY < 0.0) {
		*figPointY = lly;
	}
	else if(tempY > 0.0) {
		*figPointY = ury;
	}
}

/*same as go_normal_point func but with just different function calls*/
double go_normal_point_ellipse(double currentX, double lly, double ury, double interval, double f(Point2D *center, double major, double minor, double x), double real_ex, Point2D *center, double major, double minor) {

	double tempY, tempInterval = interval / PRECISION2;

	tempY = f(center, major, minor, currentX + interval);
	while((isfinite(tempY) == NO || tempY < lly || tempY > ury) && currentX + interval <= real_ex) {
		currentX += interval;
		tempY = f(center, major, minor, currentX + interval);
	}

	tempY = f(center, major, minor, currentX + tempInterval);
	while((isfinite(tempY) == NO || tempY < lly || tempY > ury) && currentX <= real_ex) {
		currentX += tempInterval;
		tempY = f(center, major, minor, currentX + tempInterval);
	}

	return currentX;
}

/*main function for non-special function graphics. This function also resizes before getting the points which brings less nodes for linked list which means fewer memory and speed for future operations*/
void draw_fx(Figure *fig, double f(double x), double start_x, double end_x) {

	double interval, currentX, tempX, tempY, tempInterval, real_sx, real_ex;
	Point2D *head = NULL;
	double prevCurrentX;
	int toggle = 0;

	fig->attachedPoint = NULL; /*resets this address if a figure comes to this function again*/
	if(fig->point != NULL) {	/*resets the points that belongs the old shape*/
		freePoints(&(fig->point));
	}

	if(start_x > end_x) {	/*if input is vice-versa*/
		tempX = start_x;
		start_x = end_x;
		end_x = tempX;
	}

	if(fig->llx > start_x) {	/*this function works with real boundaries which means the inner points with appropriate to paper size and user inputs'*/	
		real_sx = fig->llx;
	}
	else {
		real_sx = start_x;
	}

	if(fig->urx < end_x) {
		real_ex = fig->urx;
	}
	else {
		real_ex = end_x;
	}

	interval = (real_ex - real_sx) / fig->resolution;	/*main x change, higher res ---> small x change*/
	tempInterval = interval / PRECISION2;	/*side x change to have better results with higher precision*/
	currentX = real_sx;

	if(interval > 0) {	/*if there will be x points*/
		if(currentX + interval < real_ex) {	/*checks will there be more than 2 points to do appropriate work*/

			if(isfinite(f(currentX)) == NO || f(currentX) < fig->lly || f(currentX) > fig->ury) {	/*this if checks if y values is appropriate or not*/

				currentX = go_normal_point(currentX, fig->lly, fig->ury, interval, f, real_ex);
				if(currentX + tempInterval <= real_ex) currentX += tempInterval; /*get to non-nan or in boundaries value*/
			}

			fig->point = (Point2D *)malloc(sizeof(Point2D));	/*allocate memory for head of figure points*/
			if(fig->point == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			head = fig->point;

			if(isnan(f(currentX)) == NO) {	/*if this if didn't succeed then currentX must have passed real_ex*/

				set_point(fig->point, currentX, f(currentX), draw);
				
				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
				}

				fig->point->next = NULL;
			}
			else {
				set_point(fig->point, currentX, 0.0, pass);
			}
		}
		else { /*if exceeds end point*/
			
			fig->point = (Point2D *)malloc(sizeof(Point2D));
			if(fig->point == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			head = fig->point;

			while(isnan(f(currentX)) != NO && currentX < real_ex) {	/*travel through interval to find a non-nan value*/
				currentX += tempInterval;
			}
			
			if(isnan(f(currentX)) == NO) {
				set_point(fig->point, currentX, f(currentX), draw);

				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) { /*if y value goes to infinity or out of paper -then treat it as it is infinity- 
																												subtract a precision the won't make it inf and set a boundary point appropriate to its non-inf value*/
					currentX += PRECISION;
					set_point(fig->point, currentX, 0.0, draw);
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
				}
			}
			else {
				set_point(fig->point, real_sx, 0.0, draw);
			}

			fig->point->next = (Point2D*)malloc(sizeof(Point2D));
			if(fig->point->next == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			fig->point = fig->point->next;

			tempX = real_ex;	/*to don't change real_ex i use a temp here*/
			while(isnan(f(tempX)) != NO && tempX > real_sx) {	/*travel backwards through interval to find a non value*/
				tempX -= tempInterval;
			}
		
			if(isnan(f(tempX)) == NO) {
				currentX = tempX;
				set_point(fig->point, currentX, f(currentX), draw);

				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {
					currentX -= PRECISION;
					set_point(fig->point, currentX, 0.0, draw);
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
				}
			}
			else {
				currentX = real_ex;
				set_point(fig->point, real_ex, 0.0, draw);
			}

			fig->point->next = NULL;
		}

		prevCurrentX = currentX;	/*save this point as prev*/
		currentX += interval;		/*get to next x value*/
		
		if(currentX <= real_ex) {	/*if there is more than 2 points*/
			do {	/*main loop for middle points*/
				if(currentX < real_ex) {

					if(isnan(f(currentX)) != NO) {	/*if this point is a nan*/
						tempX = currentX;
						currentX -= interval;	/*get back to x value 1 interval before*/
				
						toggle = 0;
						while(currentX + tempInterval < tempX) {	/*search through with smaller x changes to find a non nan value - why? better precision*/
							currentX += tempInterval;
							
							if(isfinite(f(currentX)) == YES && isfinite(f(currentX + tempInterval)) != YES) {
								
								fig->point->next = (Point2D*)malloc(sizeof(Point2D));
								if(fig->point->next == NULL) {
									perror("\nError. ");
									exit(EXIT_FAILURE);
								}
								fig->point = fig->point->next;

								set_point(fig->point, currentX, f(currentX), pass);
								toggle = 1;
							}
						}

						currentX += tempInterval;	/*get rid of infinite loop with passing the point where y was nan*/
						
						if(toggle == 0) {	/*pass to next point of this if we couldn't find a non nan y*/
							fig->point->passNext = pass;
						}
					}
					else {
					
						fig->point->next = (Point2D*)malloc(sizeof(Point2D));	/*this is inside of ifs cause we may not find a non nan value*/
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						set_point(fig->point, currentX, f(currentX), draw);	/*set the point then made arranges to this point*/
					}

					if(isinf(fig->point->y) != NO) {	/*if it is infinity create 2 point where it is non-inf*/

						set_point(fig->point, fig->point->x - PRECISION, 0.0, pass);	/*set point again, y value doesn't matter since we will change it, set passNext value 1, subtract precision from x,
																							this is kinda left limit but a tricky one*/
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));

						fig->point->next = (Point2D*)malloc(sizeof(Point2D));
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						currentX += PRECISION;
						set_point(fig->point, currentX, f(currentX), draw);
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
					}
					else if(fig->point->y < fig->lly || fig->point->y > fig->ury) {	/*i could treat this as infinity but i chose higher precision output, i suppose*/

						tempY = f(currentX - tempInterval);	/*travel backwards to find a appropriate value*/
						while((isfinite(tempY) == NO || tempY < fig->lly || tempY > fig->ury) && currentX - tempInterval > prevCurrentX) {
							currentX -= tempInterval;
							tempY = f(currentX - tempInterval);
						}

						set_point(fig->point, currentX, 0.0, pass);
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));

						currentX = go_normal_point(currentX, fig->lly, fig->ury, interval, f, real_ex);

						if(currentX < real_ex && fig->point->x != currentX) {

							fig->point->next = (Point2D*)malloc(sizeof(Point2D));
							if(fig->point->next == NULL) {
								perror("\nError. ");
								exit(EXIT_FAILURE);
							}
							fig->point = fig->point->next;

							set_point(fig->point, currentX, 0.0, draw);
							set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
						}
					}

					fig->point->next = NULL;
				}
				else if(currentX >= real_ex) { /*if finally it exceeds real_ex after middle points set a appropraite end point*/

					tempX = currentX;
					currentX = real_ex;
					
					while(isnan(f(currentX)) != NO && currentX - tempInterval > prevCurrentX) {
						currentX -= tempInterval;
					}

					if(isnan(f(currentX)) == NO) {

						fig->point->next = (Point2D*)malloc(sizeof(Point2D));
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						set_point(fig->point, currentX, f(currentX), draw);

						if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {

							set_point(fig->point, fig->point->x - PRECISION, 0.0, draw);
							set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(fig->point->x));
						}

						fig->point->next = NULL;	
					}

					currentX = tempX;
				}

				prevCurrentX = currentX;	/*update prev*/
				currentX += interval;	/*increase x*/

			}while(currentX - interval < real_ex);
		}

		fig->point->passNext = shape_end;	/*2 means this  shape's path is ended here*/
		fig->point = head;	/*go back to first point*/

	}
	else if(interval == 0) {
		printf("Error. Interval caused by given boundaries and coordinates or resolution is 0.\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Error. Interval is somehow became negative.\n");
		exit(EXIT_FAILURE);
	}
}

/*this function is same as draw_fx what different is function calls for y values*/
void getEllipse(Figure *fig, double f(Point2D *center, double major, double minor, double x), double start_x, double end_x, Point2D *center, double major, double minor) {

	double interval, currentX, tempX, tempY, tempInterval, real_sx, real_ex;
	Point2D *head = NULL;
	double prevCurrentX;
	int toggle = 0;

	fig->attachedPoint = NULL;
	if(fig->point != NULL) {
		freePoints(&(fig->point));
	}

	if(start_x > end_x) {
		tempX = start_x;
		start_x = end_x;
		end_x = tempX;
	}

	if(fig->llx > start_x) {
		real_sx = fig->llx;
	}
	else {
		real_sx = start_x;
	}

	if(fig->urx < end_x) {
		real_ex = fig->urx;
	}
	else {
		real_ex = end_x;
	}

	interval = (real_ex - real_sx) / fig->resolution;	//X NOKTALARI BOUNDING DIŞINDAYSA DA ÇİZİYOR!!!!!!!!!!!!!!
	tempInterval = interval / PRECISION2;
	currentX = real_sx;

	if(interval > 0) {
		if(currentX + interval < real_ex) {

			if(isfinite(f(center, major, minor, currentX)) == NO || f(center, major, minor, currentX) < fig->lly || f(center, major, minor, currentX) > fig->ury) {

				currentX = go_normal_point_ellipse(currentX, fig->lly, fig->ury, interval, f, real_ex, center, major, minor);
				if(currentX + tempInterval <= real_ex) currentX += tempInterval; /*get to non-nan or in boundaries value*/
			}

			fig->point = (Point2D *)malloc(sizeof(Point2D));
			if(fig->point == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			head = fig->point;

			if(isnan(f(center, major, minor, currentX)) == NO) {	//if this if didn't succeed then currentX must have passed real_ex

				set_point(fig->point, currentX, f(center, major, minor, currentX), draw);
				
				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
				}

				fig->point->next = NULL;
			}
			else {
				set_point(fig->point, currentX, 0.0, pass);
			}
		}
		else { //if exceeds end point
			
			fig->point = (Point2D *)malloc(sizeof(Point2D));
			if(fig->point == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			head = fig->point;

			while(isnan(f(center, major, minor, currentX)) != NO && currentX < real_ex) {
				currentX += tempInterval;
			}
			
			if(isnan(f(center, major, minor, currentX)) == NO) {
				set_point(fig->point, currentX, f(center, major, minor, currentX), draw);

				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {
					currentX += PRECISION;
					set_point(fig->point, currentX, 0.0, draw);
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
				}
			}
			else {
				set_point(fig->point, real_sx, 0.0, draw);
			}

			fig->point->next = (Point2D*)malloc(sizeof(Point2D));
			if(fig->point->next == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			fig->point = fig->point->next;

			tempX = real_ex;
			while(isnan(f(center, major, minor, tempX)) != NO && tempX > real_sx) {
				tempX -= tempInterval;
			}
		
			if(isnan(f(center, major, minor, tempX)) == NO) {
				currentX = tempX;
				set_point(fig->point, currentX, f(center, major, minor, currentX), draw);

				if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {
					currentX -= PRECISION;
					set_point(fig->point, currentX, 0.0, draw);
					set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
				}
			}
			else {
				currentX = real_ex;
				set_point(fig->point, real_ex, 0.0, draw);
			}

			fig->point->next = NULL;
		}
		prevCurrentX = currentX;
		currentX += interval;
		
		if(currentX <= real_ex) {
			do {
				if(currentX < real_ex) {	
					if(isnan(f(center, major, minor, currentX)) != NO) {
				
						tempX = currentX;
						currentX -= interval;
				
						toggle = 0;
						while(currentX + tempInterval < tempX) {
							currentX += tempInterval;
							
							if(isfinite(f(center, major, minor, currentX)) == YES && isfinite(f(center, major, minor, currentX + tempInterval)) != YES) {
								
								fig->point->next = (Point2D*)malloc(sizeof(Point2D));
								if(fig->point->next == NULL) {
									perror("\nError. ");
									exit(EXIT_FAILURE);
								}
								fig->point = fig->point->next;

								set_point(fig->point, currentX, f(center, major, minor, currentX), pass);
								toggle = 1;
							}
						}

						currentX += tempInterval;
						
						if(toggle == 0) {
							fig->point->passNext = pass;
						}
					}
					else {
					
						fig->point->next = (Point2D*)malloc(sizeof(Point2D));
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						set_point(fig->point, currentX, f(center, major, minor, currentX), draw);
					}

					if(isinf(fig->point->y) != NO) {

						set_point(fig->point, fig->point->x - PRECISION, 0.0, pass);
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));

						fig->point->next = (Point2D*)malloc(sizeof(Point2D));
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						currentX += PRECISION;
						set_point(fig->point, currentX, f(center, major, minor, currentX), draw);
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
					}
					else if(fig->point->y < fig->lly || fig->point->y > fig->ury) {

						tempY = f(center, major, minor, currentX - tempInterval);	//kontrol et
						while((isfinite(tempY) == NO || tempY < fig->lly || tempY > fig->ury) && currentX - tempInterval > prevCurrentX) {
							currentX -= tempInterval;
							tempY = f(center, major, minor, currentX - tempInterval);
						}

						set_point(fig->point, currentX, 0.0, pass);
						set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));

						currentX = go_normal_point_ellipse(currentX, fig->lly, fig->ury, interval, f, real_ex, center, major, minor);

						if(currentX < real_ex && fig->point->x != currentX) {

							fig->point->next = (Point2D*)malloc(sizeof(Point2D));
							if(fig->point->next == NULL) {
								perror("\nError. ");
								exit(EXIT_FAILURE);
							}
							fig->point = fig->point->next;

							set_point(fig->point, currentX, 0.0, draw);
							set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
						}
					}

					fig->point->next = NULL;
				}
				else if(currentX >= real_ex) { //if exceeds end Point2D

					tempX = currentX;
					currentX = real_ex;
					
					while(isnan(f(center, major, minor, currentX)) != NO && currentX - tempInterval > prevCurrentX) {
						currentX -= tempInterval;
					}

					if(isnan(f(center, major, minor, currentX)) == NO) {

						fig->point->next = (Point2D*)malloc(sizeof(Point2D));
						if(fig->point->next == NULL) {
							perror("\nError. ");
							exit(EXIT_FAILURE);
						}
						fig->point = fig->point->next;

						set_point(fig->point, currentX, f(center, major, minor, currentX), draw);

						if(isinf(fig->point->y) != NO || fig->point->y < fig->lly || fig->point->y > fig->ury) {

							set_point(fig->point, fig->point->x - PRECISION, 0.0, draw);
							set_boundaries(fig->lly, fig->ury, &(fig->point->y), f(center, major, minor, fig->point->x));
						}

						fig->point->next = NULL;	
					}

					currentX = tempX;
				}

				prevCurrentX = currentX;
				currentX += interval;
			}while(currentX - interval < real_ex);
		}

		fig->point->passNext = shape_end;
		fig->point = head;
	}
	else if(interval == 0) {
		printf("Error. Interval caused by given boundaries and coordinates or resolution is 0.\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Error. Interval is somehow became negative.\n");
		exit(EXIT_FAILURE);
	}
}

/*this function uses draw ellipse why? cause circle is also an ellipse too but with same focus points*/
void draw_circle(Figure *fig, Point2D * center, double radius) {

	Point2D *head = NULL, **middle;

	getEllipse(fig, fEllipseTop, center->x - radius, center->x + radius, center, radius, radius); /*get points of top part*/

	head = fig->point; /*get head of top part*/

	while(fig->point->next != NULL) {	/*get to last point of top part*/
		fig->point = fig->point->next;
	}
	fig->point->passNext = pass;	/*function makes this value 2 cause it is end of path but since this shape result of a 2 function i rearrange this*/

	middle = &(fig->point->next);	/*last point's next value is NULL so i use a double pointer to save it*/

	fig->point = fig->point->next;	/*get to NULL value*/

	getEllipse(fig, fEllipseBottom, center->x - radius, center->x + radius, center, radius, radius); /*get to bottom points of shape*/

	*middle = fig->point;	/*attached top and bottom points*/
	fig->point = head;	/*get to very first head which is top part's first point*/
}

/*same as draw_circle but now focus points differs*/
void draw_ellipse(Figure *fig, Point2D *center, double major, double minor) {

	Point2D *head = NULL, **middle;

	getEllipse(fig, fEllipseTop, center->x - major, center->x + major, center, major, minor);

	head = fig->point;

	while(fig->point->next != NULL) {
		fig->point = fig->point->next;
	}
	fig->point->passNext = pass;

	middle = &(fig->point->next);

	fig->point = fig->point->next;

	getEllipse(fig, fEllipseBottom, center->x - major, center->x + major, center, major, minor);

	*middle = fig->point;
	fig->point = head;
}

/*this function resizes figure to given 2 points, it can resize to a smaller or bigger frame with a trick coordinated with exporting functions
	i resized everything at the end cause in the eps and svg files there are boundaries so i shouldn't do some drawings out of these boundaries*/
void resize_figure(Figure *fig, Point2D start_roi, Point2D end_roi) {

	Point2D *head = fig->point;

	fig->width = end_roi.x - start_roi.x;
	fig->height = end_roi.y - start_roi.y;
	fig->llx = start_roi.x;
	fig->lly = start_roi.y;
	fig->urx = end_roi.x;
	fig->ury = end_roi.y;

	while(fig->point != NULL) {

		if(fig->point->x < fig->llx || fig->point->x > fig->urx || fig->point->y < fig->lly || fig->point->y > fig->ury) {	/*if out of boundaries*/
			if(fig->point->passNext == draw) {	/*if it is a point where it is drawing*/
				fig->point->passNext = scaled;	/*make it scaled(different value) why? cause it means that this point was drawing point but now its scaled and it shouldn't be drawed but don't forget it
													so it is different than value of pass or shape_end*/
			}
		}
		else {
			if(fig->point->passNext == scaled) {	/*if it is already scaled but now it is in boundaries revert it to draw*/
				fig->point->passNext = draw;
			}
		}
		fig->point = fig->point->next;
	}
	fig->point = head;
}

void draw_polyline(Figure *fig, Point2D *poly_line, int n) {

	int counter = 0;
	Point2D *headPoly = poly_line; /*save poly's head*/
	Point2D *headFig = NULL;
	Point2D start_roi, end_roi;

	start_roi.x = fig->llx;	/*i'll use these for resizing*/
	start_roi.y = fig->lly;
	end_roi.x = fig->urx;
	end_roi.y = fig->ury;

	/*just get all of the points and save it to figure, i resize them with my function which handles arrangements*/
	if(poly_line != NULL) {

		fig->point = (Point2D *)malloc(sizeof(Point2D));
		if(fig->point == NULL) {
			perror("\nError. ");
			exit(EXIT_FAILURE);
		}
		headFig = fig->point;
		fig->point->x = poly_line->x;
		fig->point->y = poly_line->y;
		fig->point->passNext = draw;

		poly_line = poly_line->next;
		++counter;

		while(counter < n) {
			fig->point->next = (Point2D *)malloc(sizeof(Point2D));
			if(fig->point->next == NULL) {
				perror("\nError. ");
				exit(EXIT_FAILURE);
			}
			fig->point = fig->point->next;
			fig->point->x = poly_line->x;
			fig->point->y = poly_line->y;
			fig->point->passNext = draw;

			poly_line = poly_line->next;
			++counter;
		}

		fig->point = headFig;
		poly_line = headPoly;

		resize_figure(fig, start_roi, end_roi);
	}
}

/*this function just multiplies the values and resizes the figure since it may exceed boundaries*/
void scale_figure(Figure *fig, double scale_x, double scale_y) {
	
	Point2D *head = fig->point;
	Point2D start_roi, end_roi;

	while(fig->point != NULL) {
		
		fig->point->x *= scale_x;
		fig->point->y *= scale_y;
		fig->point = fig->point->next;
	}

	fig->point = head;

	start_roi.x = fig->llx;
	start_roi.y = fig->lly;
	end_roi.x = fig->urx;
	end_roi.y = fig->ury;

	resize_figure(fig, start_roi, end_roi);
}


void append_figures(Figure *fig1, Figure* fig2) {

	Point2D *head = fig1->point;
	Point2D start_roi, end_roi;

	start_roi.x = fig1->llx;
	start_roi.y = fig1->lly;
	end_roi.x = fig1->urx;
	end_roi.y = fig1->ury;

	while(fig1->point->next != NULL && fig1->point != fig2->point) {	/*if fig2 is already appended don't append it again, if not go to very end of fig1*/
		fig1->point = fig1->point->next;
	}

	if(fig1->point->next == NULL) {

		fig1->point->passNext = shape_end;	/*this shape ends here*/
		if(fig2->attachedPoint == NULL) fig2->attachedPoint = fig1->point; /*save where it is appended*/

		fig1->point->next = fig2->point;	/*append*/
	}

	fig1->point = head;	/*get back*/

	resize_figure(fig1, start_roi, end_roi); /*resize appropriate to fig1 specs*/
}

void export_eps(Figure *fig, char *file_name) {

	FILE *fp;
	double scalefont;
	Point2D *head = fig->point;

	if(fig->height < fig->width) scalefont = fig->height / 20.0;
	else scalefont = fig->width / 20.0;

	if(fig != NULL && fig->point != NULL) {
		fp = fopen(file_name, "w");
		if(fp == NULL) {
			perror("Error. ");
			exit(EXIT_FAILURE);
		}

		fprintf(fp,"%s\n", "%!PS-Adobe-3.0 EPSF-3.0");
		fprintf(fp,"%s %lf %lf %lf %lf\n", "%%BoundingBox: ", fig->llx, fig->lly, fig->urx, fig->ury);

		while(fig->point->next != NULL && fig->point->passNext != draw) fig->point = fig->point->next; /*get to very first point where it is not should be passed*/

		if(fig->point->next != NULL) {

			fprintf(fp, "%s\n", "newpath");
			fprintf(fp, "%lf %s\n", fig->thickness, "setlinewidth");
			fprintf(fp, "%lf %lf %lf %s\n", fig->colorRGB.red / 100.0, fig->colorRGB.green / 100.0, fig->colorRGB.blue / 100.0, "setrgbcolor");	/*eps accepts 0-1 float values*/
			fprintf(fp, "%.6lf %.6lf %s\n", fig->point->x, fig->point->y, "moveto");	/*get imaginary pen to first point*/

			while(fig->point->next != NULL) {
				fig->point = fig->point->next;
				if(fig->point->passNext == draw) {
					fprintf(fp, "%.6lf %.6lf %s\n", fig->point->x, fig->point->y, "lineto");
				}
				else if(fig->point->passNext != draw) {
					fprintf(fp, "%.6lf %.6lf %s\n", fig->point->x, fig->point->y, "lineto"); /* draw a line to this point*/
					while(fig->point->next != NULL && fig->point->next->passNext !=  draw) fig->point = fig->point->next;	/*get to first point where it doesn't have a consecutive passing*/
					if(fig->point->next != NULL) {
						fprintf(fp, "%.6lf %.6lf %s\n", fig->point->next->x, fig->point->next->y, "moveto");	/*don't line to this point and get to here*/
						fig->point = fig->point->next;
					}
				}
			}	
			fprintf(fp, "%s\n", "stroke");	/*draw path*/

			fprintf(fp, "%s\n", "newpath");	/*draw coordinate system*/
			fprintf(fp, "%lf %s\n", fig->thickness / 3.0, "setlinewidth");
			fprintf(fp, "%s\n", "0 0 0 setrgbcolor");
			fprintf(fp, "%lf %lf %s\n",fig->llx, 0.0, "moveto");
			fprintf(fp, "%lf %lf %s\n", fig->urx, 0.0, "lineto");
			fprintf(fp, "%lf %lf %s\n", 0.0, fig->lly, "moveto");
			fprintf(fp, "%lf %lf %s\n", 0.0, fig->ury, "lineto");
			fprintf(fp, "%s\n", "stroke");
		}
		else {	/*if there is no points to draw, or there is no line for path, eps file gives a corrupted image, this text blocks it*/
			fprintf(fp, "%lf %lf %s\n", fig->urx, fig->ury, "translate");
			fprintf(fp, "%s\n", "/Times-Roman findfont");
			fprintf(fp, "%.6lf %s\n", scalefont, "scalefont");
			fprintf(fp, "%s\n", "setfont");
			fprintf(fp, "%.6lf %d %s\n", (fig->llx), 0, "moveto");	/*Reason i stick text to left most is not to exceed paper width*/
			fprintf(fp, "%s\n", "(No Image) show");	/*There was only 1 point because of resolution!*/
			fprintf(fp, "%s\n", "showpage");
		}

		fig->point = head;

		fclose(fp);
	}
}

void export_svg(Figure *fig, char *file_name) {

	FILE *fp;
	Point2D *head = fig->point;

	fp = fopen(file_name, "w");
	if(fp == NULL) {
		perror("Error. ");
		exit(EXIT_FAILURE);
	}

	fprintf(fp,"%s ", "<svg");
	fprintf(fp,"%s%27lf %lf %lf %lf\n%17c\n", "viewBox=\t\"\n", fig->llx, fig->lly, fig->width, fig->height, '"');
	fprintf(fp, "%17c\n", '>');

	if(fig->point->next != NULL) {

		fprintf(fp, "\t%s\t%c\n", "<path d=", '"');
		while(fig->point->next != NULL && fig->point->next->passNext != draw) fig->point = fig->point->next;
		fprintf(fp, "%17c %.6lf %.6lf\n", 'M', fig->point->x, fig->point->y);

		while(fig->point->next != NULL) {
			fig->point = fig->point->next;
			if(fig->point->passNext == draw) {
				fprintf(fp, "%17c %.6lf %.6lf\n", 'L', fig->point->x, fig->point->y);
			}
			else if(fig->point->passNext != draw) {
				fprintf(fp, "%17c %.6lf %.6lf\n", 'L', fig->point->x, fig->point->y);
				while(fig->point->next != NULL && fig->point->next->passNext !=  draw) fig->point = fig->point->next;
				if(fig->point->next != NULL) {
					fprintf(fp, "%17c %.6lf %.6lf\n", 'M', fig->point->next->x, fig->point->next->y);
					fig->point = fig->point->next;
				}
			}
		}
		fprintf(fp, "%17c\n", '"');
		fprintf(fp, "%30s%lf %c%s%s %lf%s %lf%s %lf%s\n", "style=\" stroke-width: ", fig->thickness, ';', "fill: none; ", "stroke: rgb(", fig->colorRGB.red,
																																		"%,", fig->colorRGB.green,
																																		"%,", fig->colorRGB.blue, "%)\"");
		fprintf(fp, "%41s\n", "transform= \"matrix(1 0 0 -1 0 0)\"");	/*make the viewbox's origin to bottom left part, i suppose it is top left corner at the beginning*/
		fprintf(fp, "\t%s\n", "/>");
		fprintf(fp, "%s\n", "</svg>");
	}

	fig->point = head;

	fclose(fp);
}

/*i wrote this function to work with svg files with output of my library*/
void import_svg(char *file_name, Figure *fig) {

	char junk[100];
	FILE *fp;
	int eofTag;
	Point2D *head;

	if(fig != NULL) {

		fp = fopen(file_name, "r");
		if(fp == NULL) {
			perror("Error. ");
			exit(EXIT_FAILURE);
		}

		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "<svg") != 0);

		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "viewBox=") != 0);

		fscanf(fp, "%s", junk);
		fscanf(fp, "%lf", &(fig->llx));
		fscanf(fp, "%lf", &(fig->lly));
		fscanf(fp, "%lf", &(fig->width));
		fscanf(fp, "%lf", &(fig->height));
		
		fig->urx = fig->width / 2.0;
		fig->ury = fig->height / 2.0;

		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "<path") != 0);

		fscanf(fp, "%s", junk);
		fscanf(fp, "%s", junk);
		
		fig->point = (Point2D *)malloc(sizeof(Point2D));
		if(fig->point == NULL) {
			perror("\nError. ");
			exit(EXIT_FAILURE);
		}
		head = fig->point;

		fscanf(fp, "%s", junk);

		do {
			fig->point->passNext = draw;
			fscanf(fp, "%lf", &(fig->point->x));
			eofTag = fscanf(fp, "%lf", &(fig->point->y));

			fscanf(fp, "%s", junk);

			if(strcmp(junk, "\"") != 0) {

				if(strcmp(junk, "M") == 0) {
					fig->point->passNext = pass;
				}

				fig->point->next = (Point2D *)malloc(sizeof(Point2D));
				if(fig->point->next == NULL) {
					perror("\nError. ");
					exit(EXIT_FAILURE);
				}
				fig->point = fig->point->next;
			}

		}while(eofTag != EOF && strcmp(junk, "\"") != 0);

		fig->point->passNext = shape_end;

		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "style=\"") != 0);
	
		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "stroke-width:") != 0);

		fscanf(fp, "%lf", &(fig->thickness));

		do {
			eofTag = fscanf(fp, "%s", junk);
		}while(eofTag != EOF && strcmp(junk, "rgb(") != 0);

		fscanf(fp, "%s", junk);
		junk[strlen(junk) - 2] = 0;
		sscanf(junk, "%lf", &(fig->colorRGB.red));	/*svg file forces me to writing these color values adjacent to % symbol so i read them as strings, parse them string with just numerical values
															then get double values with sscanf*/
		fscanf(fp, "%s", junk);
		junk[strlen(junk) - 2] = 0;
		sscanf(junk, "%lf", &(fig->colorRGB.green));
		fscanf(fp, "%s", junk);
		junk[strlen(junk) - 3] = 0;
		sscanf(junk, "%lf", &(fig->colorRGB.blue));

		fclose(fp);

		fig->point = head;
	}
	else {
		printf("Figure is NULL. You should use start_figure function at first. (You can give any width & height value)\n");
	}
}

/*takes as double pointer to making it NULL after freeing*/
void freeFigure(Figure **fig) {

	if(*fig != NULL) {

		if((*fig)->attachedPoint != NULL) {

			(*fig)->attachedPoint->next = NULL; /*making the point's next NULL which the point is where this figure appenned if it is*/
			(*fig)->attachedPoint = NULL;
		}

		if((*fig)->point != NULL) {

			freePoints(&((*fig)->point));
		}

		free(*fig);
		*fig = NULL; /*why cause if a figure is already appended freeing it doesn't make it's address disappear so i prevent it freeing twice*/
	}
}

/*at first i was doing this recursively but with big resolutions i was getting segmentation fault so i did it iteratively*/
void freePoints(Point2D **headPoint) {

	Point2D *current = *headPoint;  
	Point2D *next;
	int toggle = YES;
	  
	while (current != NULL && toggle == YES)  
	{  
		if(current->passNext == shape_end) {	/*this toggle helps me to handle to only free this fig's points otherwise i would free other figure's points if they are appended to this fig*/
			toggle = NO;
		}

	    next = current->next;
	    free(current);  
	    current = next;  
	}  
	   
	*headPoint = NULL; /*again, to don't free twice i make this address NULL*/ 
}
