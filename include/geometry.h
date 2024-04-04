#ifndef GEOMETRY
#define GEOMETRY


#include "grid.h"

class Geometry{
private:
	Grid g;
public:
	Geometry(){};

	virtual bool construct_geometry() = 0;


	Grid& get_grid() {return g;}

	bool test();
};



#endif //GEOMETRY
