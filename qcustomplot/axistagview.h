/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2018 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.06.18                                             **
**          Version: 2.0.1                                                **
****************************************************************************/

#ifndef AXISTAGVIEW_H
#define AXISTAGVIEW_H

#include "qcustomplot.h"
#include "axistag.h"


class axisTagView : public QWidget
{
  Q_OBJECT
  
public:
  explicit axisTagView(QWidget *parent = 0);
  ~axisTagView();
  
private slots:
  void timerSlot();
  
private:
  QCustomPlot *mPlot;
  QPointer<QCPGraph> mGraph1;
  QPointer<QCPGraph> mGraph2;
  AxisTag *mTag1;
  AxisTag *mTag2;
  QTimer mDataTimer;
};


#endif // AXISTAGVIEW_H
