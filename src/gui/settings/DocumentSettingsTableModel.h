#ifndef DOCUMENTSETTINGSTABLEMODEL_H_
#define DOCUMENTSETTINGSTABLEMODEL_H_

#include "SettingsTableModel.h"
#include "config/DocumentSettings.hpp"

class DocumentSettingsTableModel : public SettingsTableModel
{
private:
  typedef DocumentSettings D_SET;
public:
  DocumentSettingsTableModel(QObject *parent = NULL) :
    SettingsTableModel(parent) {
    addKey(D_SET::TITLE);
    addKey(D_SET::WIDTH);
    addKey(D_SET::HEIGHT);
    addKey(D_SET::RESOLUTION);
    addKey(D_SET::AUTO_FOCUS);
    addKey(D_SET::ENABLE_RASTER);
    addKey(D_SET::ENABLE_VECTOR);
  }
};

#endif /* DOCUMENTSETTINGSTABLEMODEL_H_ */