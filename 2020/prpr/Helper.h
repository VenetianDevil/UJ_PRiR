#ifndef HELPER_H_
#define HELPER_H_

#include<math.h>

class Helper {
   public:
      static int toIdx( int size, int row, int col );

      static double getValue( double *array, int size, int row, int col );

      static void setValue( double *array, int size, int row, int col, double newValue );

      static void updateValue( double *array, int size, int row, int col, double delta );
};

#endif

