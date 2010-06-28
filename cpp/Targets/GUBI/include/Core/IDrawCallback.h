#ifndef IDRAWCALLBACK_H
#define IDRAWCALLBACK_H

namespace gubi {

class IDrawCallback {
public:
   virtual void gubiShouldBeDrawn() = 0;
};

} // End of namespace gubi

#endif /* IDRAWCALLBACK_H */
