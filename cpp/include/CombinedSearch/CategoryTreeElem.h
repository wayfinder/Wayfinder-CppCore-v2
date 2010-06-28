/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CATEGORY_TREE_ELEM_H
#define CATEGORY_TREE_ELEM_H

#include "arch.h"
#include <vector>
#ifdef WFAPI_IOSTREAM_AVAILABLE
# include <iostream>
#endif


namespace CategoryTreeImpl {

/**
 * Class that represents a node in the simulated category tree that holds
 * Categories retreived from the server through the get category tree
 * request.
 */
class CategoryTreeElem {
public:

   /** 
    * Default constructor
    */
   CategoryTreeElem();

   /** 
    * Copy constructor. Copies members from other to
    * own members.
    * 
    * @param other The CategoryTreeElem object that we should copy. 
    */
   CategoryTreeElem(const CategoryTreeElem& other);

   /** 
    * Constructor that creates a category with a number of
    * sub categories.
    * 
    * @param id The unique id of this catgory, provided by server.
    * @param name The name of this category.
    * @param imageName The name of the image for this category.
    * @param subCats A list of ids of the sub categories for this category.
    */
   CategoryTreeElem(int32 id,
                    const char* name,
                    const char* imageName,
                    const std::vector<int32>& subCats);

   /** 
    * Destructor
    */
   virtual ~CategoryTreeElem();

   /** 
    * Getter for the id of this category.
    * 
    * @return Returns the id for this category.
    */
   int32 getId() const;

   /** 
    * Getter for the name of this category.
    * 
    * @return Returns the name of this category.
    */
   const char* getName() const;

   /** 
    * Getter for the image name of this category
    * 
    * @return Returns the name of the image for this category
    */
   const char* getImageName() const;

   /** 
    * Getter for the list of sub category ids of this category
    * 
    * @return Returns a list of ids that represents the sub categories
    *         of this category.
    */
   const std::vector<int32>& getSubCats() const;

   /** 
    * Assignment operator, assigns the values from other
    * to this.
    *
    * @param other The CategoryTreeElem to assign the data from.
    * 
    * @return A reference to the new assigned member.
    */
   CategoryTreeElem& operator= (const CategoryTreeElem& other);

//#define WFAPI_IOSTREAM_AVAILABLE
#ifdef WFAPI_IOSTREAM_AVAILABLE
   /**
    * Print on ostream.
    *
    * @param stream The stream to print on.
    * @param coord  The coordinate to print.
    * @return The stream.
    */
   friend std::ostream& operator<< (std::ostream& stream,
                                    const CategoryTreeElem& catTreeElem);
#endif
private:

   /** 
    * Internal swap function, used for exception safe assigment
    * in assigment operator.
    * 
    * @param other The object to swap values from.
    */
   void swap(CategoryTreeElem& other);

   /** 
    * Internal convenience function for use in constructor when
    * initializing this.
    * 
    * @param other The object containing values we should copy.
    */
   void init(const CategoryTreeElem& other);

   /** 
    * Internal convenience function for use in constructor when
    * initializing this.
    * 
    * @param id The unique id of this catgory, provided by server.
    * @param name The name of this category.
    * @param imageName The name of the image for this category.
    * @param subCats A list of ids of the sub categories for this category.
    */
   void init(int32 id,
             const char* name,
             const char* imageName,
             const std::vector<int32>& subCats);

protected:

   /// The id of this category, the id is unique
   int32 m_id;

   /// The name of this category
   char* m_name;

   /// The name of the image for this category
   char* m_imageName;

   /**
    * A list of ids for the sub categories of this category. If empty
    * no sub categories exists.
    */
   std::vector<int32> m_subCats;
};


} // End namespace CategoryTree
#endif // End CATEGORY_TREE_ELEM_H
