#include "fitsfilelistitem.h"

FITSFileListItem::FITSFileListItem(QFileInfo fileInfo,
                                   bool showStretched = false)
    : _info(fileInfo),
      _showStretched(showStretched)
{
}

FITSFileListItem::~FITSFileListItem() {}
