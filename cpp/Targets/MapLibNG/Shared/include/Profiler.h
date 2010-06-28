#ifndef PROFILER_H
#define PROFILER_H

#include "MC2SimpleString.h"
#include <utility>
#include <stack>
#include <vector>

/**
 * This file and its corresponding .cpp-file is a self-contained
 * external profiler. As such it is a complicated piece of software
 * with its own intricacies, so don't look below :-)
 *
 * See manual at http://silverspaceship.com/src/iprof/. Based on
 * modified version from oolong engine.
 */

// comment out next line to entirely disable profiler
// #define Prof_ENABLED

#ifdef __cplusplus
#define Prof_C               "C"
#define Prof_extern_C        extern "C"
#define Prof_dummy_declare
#else
#define Prof_C
#define Prof_extern_C
#define Prof_dummy_declare   int Prof_dummy_dec =
#endif

#ifdef Prof_ENABLED

/**
 * Timing related functions used internally by the profiler.
 */ 

typedef int64 Prof_Int64;
void Prof_get_timestamp(Prof_Int64 *result);
double Prof_get_time(void);

#endif 

#ifdef Prof_ENABLED

void Prof_set_input_enabled(bool enabled);
bool Prof_input_enabled();

void Prof_set_frozen(bool frozen);
bool Prof_is_frozen();

extern Prof_C void Prof_move_cursor(int delta);
extern Prof_C void Prof_select(void);
extern Prof_C void Prof_select_parent(void);
extern Prof_C void Prof_move_frame(int delta);

extern Prof_C int  Prof_get_smoothing();
extern Prof_C void Prof_set_smoothing(int smoothing_mode);
extern Prof_C void Prof_set_frame(int frame);
extern Prof_C void Prof_set_cursor(int line);

typedef struct
{
   char * name;
   void * highlevel;
   char   initialized;
   char   visited;
   char   pad0,pad1;
} Prof_Zone;

typedef struct Prof_Zone_Stack
{
   Prof_Int64               t_self_start;

   Prof_Int64               total_self_ticks;
   Prof_Int64               total_hier_ticks;

   unsigned int             total_entry_count;

   struct Prof_Zone_Stack * parent;
   Prof_Zone              * zone;
   int                      recursion_depth;

   void                   * highlevel;      // used arbitrarily by the analysis pass
} Prof_Zone_Stack;


extern Prof_C Prof_Zone_Stack * Prof_stack; // current Zone stack
extern Prof_C Prof_Zone_Stack   Prof_dummy; // parent never matches

extern Prof_C Prof_Zone_Stack * Prof_StackAppend(Prof_Zone *zone);
// return the zone stack created by pushing 'zone' on the current


static Prof_Int64 Prof_time;

#define Prof_Begin_Cache(z)                           \
   /* declare a static cache of the zone stack */     \
   static Prof_Zone_Stack *Prof_cache = &Prof_dummy

#define Prof_Begin_Raw(z)                       \
   Prof_Begin_Cache(z);                         \
   Prof_Begin_Code(z)

#define Prof_Begin_Code(z)                                     \
   Prof_dummy_declare (                                        \
                                                               \
      /* check the cached Zone_Stack and update if needed */   \
      (Prof_cache->parent != Prof_stack                        \
       ? Prof_cache = Prof_StackAppend(&z)                     \
       : 0),                                                   \
                                                               \
      ++Prof_cache->total_entry_count,                         \
      Prof_get_timestamp(&Prof_time),                          \
                                                               \
      /* stop the timer on the parent zone stack */            \
      (Prof_stack->total_self_ticks +=                         \
       Prof_time - Prof_stack->t_self_start),                  \
                                                               \
      /* make cached stack current */                          \
      Prof_stack = Prof_cache,                                 \
                                                               \
      /* start the timer on this stack */                      \
      Prof_stack->t_self_start = Prof_time)

#define Prof_End_Raw()                          \
                                                \
   (Prof_get_timestamp(&Prof_time),             \
                                                \
    /* stop timer for current zone stack */     \
    Prof_stack->total_self_ticks +=             \
    Prof_time - Prof_stack->t_self_start,       \
                                                \
    /* make parent chain current */             \
    Prof_stack = Prof_stack->parent,            \
                                                \
    /* start timer for parent zone stack */     \
    Prof_stack->t_self_start = Prof_time)


#define Prof_Declare(z)  Prof_Zone Prof_region_##z
#define Prof_Define(z)   Prof_Declare(z) = { #z }
#define Prof_Region(z)   Prof_Begin_Raw(Prof_region_##z);
#define Prof_End         Prof_End_Raw();

#define Prof_Begin(z)    static Prof_Define(z); Prof_Region(z)
#define Prof_Counter(z)  Prof_Begin(z) Prof_End

#ifdef __cplusplus

#define Prof(x)        static Prof_Define(x); Prof_Scope(x)
 
#define Prof_Scope(x)                                             \
   Prof_Begin_Cache(x);                                           \
   Prof_Scope_Var Prof_scope_var(Prof_region_ ## x, Prof_cache)

struct Prof_Scope_Var {
   inline Prof_Scope_Var(Prof_Zone &zone, Prof_Zone_Stack * &Prof_cache);
   inline ~Prof_Scope_Var();
};

inline Prof_Scope_Var::Prof_Scope_Var(Prof_Zone &zone, Prof_Zone_Stack * &Prof_cache) {
   Prof_Begin_Code(zone);
}

inline Prof_Scope_Var::~Prof_Scope_Var() {
   Prof_End_Raw();
}

#endif


#else  // !Prof_ENABLED

#ifdef __cplusplus
#define Prof(x)
#define Prof_Scope(x)
#endif

#define Prof_Define(name)
#define Prof_Begin(z)
#define Prof_End
#define Prof_Region(z)
#define Prof_Counter(z)

#endif


/*
 *  Prof_update
 *
 *  Pass in true (1) to accumulate history info; pass
 *  in false (0) to throw away the current frame's data
 */
typedef enum
{
   PROF_discard   =0,
   PROF_accumulate=1,
} ProfUpdateMode;

#ifdef Prof_ENABLED

/*
 *  Prof_report_writefile
 *
 *  This writes a report for the current frame out to a
 *  text file. This is useful since (a) you can't cut and
 *  paste from the graphical version and (b) to give a
 *  demonstration of how to parse the report (which might
 *  make a graphical D3D version a little easier to write).
 */
extern Prof_C void Prof_report_writefile(FILE* f);

extern Prof_C void Prof_update(ProfUpdateMode accumulate);

#else // !Prof_ENABLED
  #define Prof_update(c)
  #define Prof_report_writefile(c)
#endif

#ifdef Prof_ENABLED

typedef enum
{
   Prof_SORT_NAME,
   Prof_SORT_SELF_TIME,
   Prof_SORT_HIERARCHICAL_TIME,
   Prof_CALL_GRAPH,
   Prof_CALL_GRAPH_DESCENDENTS,
} Prof_Report_Mode;

extern Prof_C int Prof_get_report_mode();
extern Prof_C void Prof_set_report_mode(Prof_Report_Mode e);

#define NUM_VALUES 4
#define NUM_TITLE 2
#define NUM_HEADER (NUM_VALUES+1)

typedef struct {
   int indent;
   char *name;
   int number;
   char prefix;
   int value_flag;
   double values[NUM_VALUES];
   double heat;

   // used internally
   void *zone;
} Prof_Report_Record;

typedef struct
{
   char *title[NUM_TITLE];
   char *header[NUM_HEADER];
   int num_record;
   int hilight;
   Prof_Report_Record *record;
} Prof_Report;

extern void         Prof_free_report(Prof_Report *z);
extern Prof_Report *Prof_create_report(void);

// really internal functions

extern void Prof_graph(int num_frames, 
                       void (*callback)(int id, int x0, int x1, float *values, void *data),
                       void *data);

#ifdef Prof_ENABLED
extern void Prof_init_highlevel();

extern int        Prof_num_zones;
extern Prof_Zone *Prof_zones[];

extern Prof_C Prof_Declare(_global);
#endif

#endif

#endif // PROFILER_H
