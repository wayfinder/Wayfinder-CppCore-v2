#ifndef APPEVENTLISTENER_H
#define APPEVENTLISTENER_H

/**
 *  This interface is intended to be used
 *  by the application which instantiates the
 *  model(MapModel), controller(for instance SampleController)
 *  and window(GtkAppWindow).
 *
 *  It only contains application wide events, i.e.
 *  that it is time to close the entire application.
 */ 
class AppEventListener {
public:
   /**
    *  Should be called whenever the controller
    *  determines that it is time to exit the application.
    *
    *  @param code The exit code. 0 if no error occured.
    */
   virtual void onExitRequest(int code) = 0;

   /**
    *  Virtual destructor to ensure that implementing classes get their
    *  destructors invoked properly.
    */ 
   virtual ~AppEventListener() {}
};

#endif /* APPEVENTLISTENER_H */
