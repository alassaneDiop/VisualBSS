// Convert longitude and latitude to x and y

#include "stdio.h"
#include "math.h"

// height and width are map height and width

int main() {  

	double lat    = 39.955641;
	double lon   = -75.181178;

	double width    = 52000.000000;
	double height   = 52000.000000;

	double PI = 3.14159265359;

	double x = fmod((width * (180 + lon) / 360), (width + (width / 2)));
	printf("x : %lf\n", x);

	double latRad = lat * PI / 180;

	double mercator = log(tan((PI / 4) + (latRad / 2)));
	double y = (height / 2) - (width * mercator / (2 * PI));

	printf("y : %lf\n", y);

	return 0;
}
