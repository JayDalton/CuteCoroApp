#include "ContextApp.h"

#include "commoncoro.h"

int main(int argc, char* argv[])
{
   ContextApp app(argc, argv);

   return app.exec();
}
