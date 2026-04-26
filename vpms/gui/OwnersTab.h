 #pragma once                                                                                                                                                                       
  #include <QWidget>                                                                                                                                                                 
  #include <QTableWidget>                                                                                                                                                            
  #include <QPushButton>
  #include <QHBoxLayout>                                                                                                                                                             
  #include <QVBoxLayout>                                    
  #include "../core/ClinicService.h"
                                                                                                                                                                                     
  class OwnersTab : public QWidget {
      Q_OBJECT                                                                                                                                                                       
                                                            
  public:
      explicit OwnersTab(ClinicService& service, QWidget* parent = nullptr);
                                                                                                                                                                                     
  private slots:
      void onAdd();                                                                                                                                                                  
      void onEdit();                                        
      void onDelete();

  private:
      ClinicService&  _service;
      QTableWidget*   _table;                                                                                                                                                        
      QPushButton*    _addBtn;
      QPushButton*    _editBtn;                                                                                                                                                      
      QPushButton*    _deleteBtn;                           
                                                                                                                                                                                     
      void setupUI();
      void loadData();                                                                                                                                                               
  };                   