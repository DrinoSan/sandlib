#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include <setjmp.h>

#define T sand_except_t
typedef struct T
{
   const char* reason;
} T;

// exported types
typedef struct sand_except_frame sand_except_frame;
struct sand_except_frame
{
   sand_except_frame* prev;
   jmp_buf            env;
   const char*        file;
   int                line;
   const T*           exception;
};

enum
{
   sand_except_entered = 0,
   sand_except_raised,
   sand_except_handled,
   sand_except_finalized
};

// exported variables
extern sand_except_frame* sand_except_stack;
// ecported functions
void sand_except_raise( const T* e, const char* file, int line );

// exported macros
#define RAISE( e ) sand_except_raise( &( e ), __FILE__, __LINE__ )

#define RERAISE                                                                \
   sand_except_raise( sand_except_frame.exception, sand_except_frame.file,     \
                      sand_except_frame.line )

#define RETURN                                                                 \
   switch ( sand_except_stack = sand_except_stack->prev, 0 )                   \
   default:                                                                    \
      return

#define TRY                                                                    \
   do                                                                          \
   {                                                                           \
      volatile int      sand_except_flag;                                      \
      sand_except_frame sand_except_frame;                                     \
      sand_except_frame.prev = sand_except_stack;                              \
      sand_except_stack      = &sand_except_frame;                             \
      sand_except_flag       = setjmp( sand_except_frame.env );                \
      if ( sand_except_flag == sand_except_entered )                           \
      {

#define EXCEPT( e )                                                            \
   if ( sand_except_flag == sand_except_entered )                              \
      sand_except_stack = sand_except_stack->prev;                             \
   }                                                                           \
   else if ( sand_except_frame.exception == &( e ) )                           \
   {                                                                           \
      sand_except_flag = sand_except_handled;

#define ELSE                                                                   \
   if ( sand_except_flag == sand_except_entered )                              \
      sand_except_stack = sand_except_stack->prev;                             \
   }                                                                           \
   else                                                                        \
   {                                                                           \
      sand_except_flag = sand_except_handled;

#define FINALLY                                                                \
   if ( sand_except_flag == sand_except_entered )                              \
      sand_except_stack = sand_except_stack->prev;                             \
   }                                                                           \
   {                                                                           \
      if ( sand_except_flag == sand_except_entered )                           \
         sand_except_flag = sand_except_finalized;

#define END_TRY                                                                \
   if ( sand_except_flag == sand_except_entered )                              \
      sand_except_stack = sand_except_stack->prev;                             \
   }                                                                           \
   if ( sand_except_flag == sand_except_raised )                               \
      RERAISE;                                                                 \
   }                                                                           \
   while ( 0 )

#undef T
#endif
