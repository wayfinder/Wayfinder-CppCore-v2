#ifndef FILEREADERTHREAD_H
#define FILEREADERTHREAD_H

#include "Util/NotCopyable.h"
#include "MC2SimpleString.h"
#include <vector>
#include "BufferRequester/CachePolicy.h"

class DBufRequestListener;
class BitBuffer;
class DirectoryDBufRequester;

class FileReaderThread : public NotCopyable {
public:
   struct job_t {
      MC2SimpleString descr;
      DBufRequestListener* caller;
      BitBuffer* res;
      CachePolicy whereFrom;
   };
   
   FileReaderThread(const char* path, DirectoryDBufRequester* parent);
   
   ~FileReaderThread();

   void addWork(job_t j);

   void cancelWork(job_t j);

   void kill();
   
   void run();

   std::vector<job_t> acquireFinished();
private:
   BitBuffer* readFromFile(const MC2SimpleString& descr);   

   struct Impl;
   Impl* m_impl;
};

#endif /* FILEREADERTHREAD_H */
