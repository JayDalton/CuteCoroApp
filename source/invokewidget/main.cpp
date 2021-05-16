#include "ContextApp.h"

int main(int argc, char* argv[])
{
   ContextApp app(argc, argv);

   qRegisterMetaType<QueueData>();

   return app.exec();
}
