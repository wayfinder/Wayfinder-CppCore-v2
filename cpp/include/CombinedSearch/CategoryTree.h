/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CATEGORY_TREE_H
#define CATEGORY_TREE_H

#include "arch.h"
#include "CategoryTreeTypedefs.h"
#include "CategoryTreeElem.h"
#include <map>
#include <vector>

namespace CategoryTreeImpl {

/**
 * Simulates a tree of category elements. It's not really a tree
 * but rather a container of category elements where the category
 * elements can contain sub categories.
 *
 * The way we have implmented this is that this tree class contains
 * a map with all category elements recevied from the server with the
 * unique category id as the key.
 *
 * Each CategoryTreeElem object contains members like name, icon name,
 * id etc. They can also have a list of ids for its sub categories.
 * Note that this is only a list of ids, not real objects. When asking
 * for a list of sub categories from a category, we use the list of
 * ids and finds the categories from the map using the ids. This way
 * we won't need to keep any duplicates of objects, which we would
 * have to do if having category objects i each sub category list.
 */
class CategoryTree
{
public:

   /** 
    * Default constructor.
    */
   CategoryTree();

   /** 
    * Constructor that takes in the root node of this tree. The root node
    * is received from the server in the NAV_LOCAL_CATEGORY_TREE_REPLY. The
    * root node should never be visible to the user or the application.
    * 
    * @param rootNode The root node of this tree.
    */
   CategoryTree(std::vector<int32> rootCategories);

   /** 
    * Destructor
    */
   virtual ~CategoryTree();

   /** 
    * Adds a root category to the list of root categories.
    * 
    * @param categoryId Id of the category that should be one of the root categories.
    */
   void addRootCategory(int32 categoryId);

   /** 
    * Sets the root categories.
    * 
    * @param rootCategories The list that contains the new set of root categories.
    */
   void setRootCategories(std::vector<int32> rootCategories);

   /** 
    * Resets the root categories. Should be called
    * before parsing updated categories from server
    */
   void resetRootCategories();

   /** 
    * Adds a category to the map.
    * 
    * @param category The category to add to the list. Its id will be
    *                 the key in the map.
    */
   void addCategory(CategoryTreeElem* category);

   /** 
    * Resets the map containing all the categories. Should be called
    * before parsing updated categories from server
    */
   void resetCategories();

   /** 
    * Getter for retreiving the list of root categories.
    * 
    * @param rootCategories Reference to a vector that will contain the
    *                       root categories when returned.
    */
   void getRootCategories(CTCatElemVec_t& rootCategories);

   /** 
    * Finds the sub categories for the category with the specified
    * id.
    *
    * 1. Find the CategoryTreeElem from the map using the id (key).
    * 2. Get the list of sub categories for the found category.
    * 3. For each id, find the category in the map and put a reference
    *    to it in the vector supplied by the caller.
    * 
    * @param categoryId The id of the category that we should get the
    *                   sub categories for.
    * @param categories Out parameter. Vector that will contain references to to
    *                   CategoryTreeElem objects, sub categories of the
    *                   category with the specified id.
    */
   void getSubCategoriesOf(int32 categoryId,
                           CTCatElemVec_t& categories);
protected:
   
private:

   /// Map containing CategoryTreeElems with the category id as key.
   CTCatTreeMap_t m_categories;

   /// The root node of the tree.
   std::vector<int32> m_rootCategories;
};

} // End namespace CategoryTreeImpl
#endif
