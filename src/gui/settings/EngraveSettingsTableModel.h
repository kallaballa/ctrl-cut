#ifndef ENGRAVESETTINGSTABLEMODEL_H_
#define ENGRAVESETTINGSTABLEMODEL_H_

#include "SettingsTableModel.h"
#include "config/EngraveSettings.hpp"

class EngraveSettingsTableModel : public SettingsTableModel
{
private:
  typedef EngraveSettings E_SET;
public:
  EngraveSettingsTableModel(QObject *parent = NULL) :
    SettingsTableModel(parent) {
    addKey(E_SET::EPOS);
    addKey(E_SET::EPOWER);
    addKey(E_SET::ESPEED);
    addKey(E_SET::DIRECTION);
    addKey(E_SET::DITHERING);
  }
};

#endif /* ENGRAVESETTINGSTABLEMODEL_H_ */
