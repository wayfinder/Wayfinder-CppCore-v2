#ifndef CANCELALGORITHM_H
#define CANCELALGORITHM_H

#include <vector>
#include <set>
#include "MapParam.h"

// r = current req, o = current outstanding

// At any point, either r is smaller than o, or o is smaller than r, or o == r

// Idea is to increment smallest of the two until they are either equal or the smallest becomes the largest

template<typename Requester, typename ForwardIterator>
void cancel(ForwardIterator req,
            ForwardIterator reqEnd,
            ForwardIterator pending,
            ForwardIterator pendingEnd,
            Requester& requester)
{

   while(req < reqEnd && pending < pendingEnd) {
      while(*pending < *req && pending < pendingEnd) {
         requester.cancel(*pending);
         pending++;
      }

      while(*pending == *req && pending < pendingEnd && req < reqEnd) {
         pending++;
         req++;
      }

      while(*req < *pending && req < reqEnd) {
         requester.request(*req);
         req++;
      }      
   }

   while(req < reqEnd) {
      requester.request(*req);
      req++;
   }

   while(pending < pendingEnd) {
      requester.cancel(*pending);
      pending++;
   }      
}

template<typename Requester, typename ForwardIterator>
void cancelSlow(ForwardIterator req,
                ForwardIterator reqEnd,
                ForwardIterator pending,
                ForwardIterator pendingEnd,
                Requester& requester)
{
   static std::set<MapParam> inserted;
   inserted.clear();
   inserted.insert(req, reqEnd);
   
   for(; req != reqEnd; req++)
   {
      std::set<MapParam>::iterator searchItr = inserted.find(*req);
      
      if(searchItr == inserted.end()) {
         requester.cancel(*req);
      } else {
         inserted.erase(searchItr);
      }
   }

   if(!inserted.empty()) {   
      std::vector<MapParam> vec(inserted.begin(), inserted.end());
   
      for(std::vector<MapParam>::iterator itr = vec.begin(); itr != vec.end();
          itr++)
      {
         requester.request(*itr);
      }      
   }
}

#endif /* CANCELALGORITHM_H */
