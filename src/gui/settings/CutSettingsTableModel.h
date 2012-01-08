#ifndef CUTSETTINGSTABLEMODEL_H_
#define CUTSETTINGSTABLEMODEL_H_

#include "SettingsTableModel.h"
#include "config/CutSettings.hpp"


class CutSettingsTableModel : public SettingsTableModel
{
private:
  typedef CutSettings C_SET;
public:
  CutSettingsTableModel(QObject *parent = NULL) :
    SettingsTableModel(parent) {
    addKey(C_SET::CPOS);
    addKey(C_SET::CPOWER);
    addKey(C_SET::CSPEED);
    addKey(C_SET::FREQUENCY);
    addKey(C_SET::REDUCE);
  }
};


#endif /* CUTSETTINGSTABLEMODEL_H_ */
