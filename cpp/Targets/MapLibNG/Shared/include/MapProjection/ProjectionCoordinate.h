#ifndef PROJECTIONCOORDINATE_H
#define PROJECTIONCOORDINATE_H

class ProjectionCoordinate {
public:
   friend class ProjectionInterface;
   friend class ProjectionInterfaceTest;
private:
   union {
      int i;
      float f;
      double d;
   } x;

   union {
      int i;
      float f;
      double d;
   } y;
};

#endif /* PROJECTIONCOORDINATE_H */
