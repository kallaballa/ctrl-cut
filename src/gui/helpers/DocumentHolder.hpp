/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef DOCUMENTHOLDER_H_
#define DOCUMENTHOLDER_H_

#include <QList>
#include <QString>
#include <memory>

typedef std::shared_ptr<class Document> DocumentPtr;

class DocumentHolder {
public:
  QList<class AbstractCtrlCutItem*> items;
  DocumentPtr doc;
  QString filename;

  DocumentHolder();
  DocumentHolder(const DocumentHolder& holder);
  void operator=(const DocumentHolder& other);
  void add(class AbstractCtrlCutItem &item);
  void addItem(class AbstractCtrlCutItem &item);
  void remove(AbstractCtrlCutItem &item);
private:
  void addToDocument(class AbstractCtrlCutItem &item);
  void removeFromDocument(AbstractCtrlCutItem &item);
};

typedef std::shared_ptr<DocumentHolder> DocumentHolderPtr;


#endif /* DOCUMENTHOLDER_H_ */
