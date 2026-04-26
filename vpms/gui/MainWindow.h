 #pragma once                                                                                                                                                    
  #include <QMainWindow>                                    
  #include <QTabWidget>
  #include "../core/ClinicService.h"
                                                                                                                                                                  
  class MainWindow : public QMainWindow {
      Q_OBJECT                                                                                                                                                    
                                                            
  public:
      explicit MainWindow(ClinicService& service, QWidget* parent = nullptr);

  private:
      ClinicService& _service;
      QTabWidget*    _tabs;
                                                                                                                                                                  
      void setupUI();
  }; 