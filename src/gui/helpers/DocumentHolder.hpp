
#ifndef DOCUMENTHOLDER_H_
#define DOCUMENTHOLDER_H_

#include <QList>
#include <QString>
#include <memory>

class DocumentHolder {
public:
  QList<class AbstractCtrlCutItem*> items;
  class Document* doc;
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
