#pragma once

#include <QFileInfo>

class FITSFileListItem
{
public:
  FITSFileListItem(QFileInfo fileInfo, bool showStretched = false);
  ~FITSFileListItem();

private:
  QFileInfo _info;
  bool _showStretched;
};