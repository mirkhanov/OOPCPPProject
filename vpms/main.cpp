#include <QApplication>                                                                                                                                         
  #include <QDir>                                                                                                                                                 
  #include "core/ClinicService.h"                                                                                                                                 
  #include "gui/MainWindow.h"                                                                                                                                     
   
  int main(int argc, char *argv[]) {                                                                                                                              
      QApplication app(argc, argv);                         

      QDir().mkpath("data");                                                                                                                                      
   
      ClinicService service;                                                                                                                                      
      MainWindow window(service);                           
      window.show();

      return app.exec();
  }