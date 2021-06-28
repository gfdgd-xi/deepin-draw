/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Zhang Hao <zhanghao@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//#ifndef MIANWINDOW_H
//#define MIANWINDOW_H
#ifndef PUBLICAPI_H
#define PUBLICAPI_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#define protected public
#define private public
#include "cgraphicsview.h"
#include <QtTest>
#include <QTestEventList>
#include <qaction.h>



#include "mainwindow.h"
#include "ccentralwidget.h"
#include "application.h"
#include "cgraphicsview.h"
#include "clefttoolbar.h"
#include "toptoolbar.h"
#include "cdrawscene.h"

#include "cpictureitem.h"
#include "cgraphicsrectitem.h"
#include "cgraphicsellipseitem.h"
#include "cgraphicstriangleitem.h"
#include "cgraphicspolygonalstaritem.h"
#include "cgraphicspolygonitem.h"
#include "cgraphicslineitem.h"
#include "cgraphicspenitem.h"
#include "cgraphicstextitem.h"
#include "cgraphicscutitem.h"
#include "cgraphicsitemselectedmgr.h"

#include <DComboBox>
#include <DLineEdit>
#include <DWidget>
#include <DSlider>

#include "calphacontrolwidget.h"
#include <dzoommenucombobox.h>
#include "cspinbox.h"
#include "ccolorpickwidget.h"
#include "ciconbutton.h"
#include "cgraphicsitem.h"
#include "pickcolorwidget.h"

#include <QBrush>
#undef protected
#undef private

//#define ON true
//#define OFF false
//#define TEST_DELETE_ITEM OFF
//#define TEST_BLUR_ITEM OFF
//#define TEST_START_ITEM OFF
//#define TEST_PICTURE_ITEM OFF
//#define TEST_RECT_ITEM ON
//#define TEST_TRIANGLE_ITEM OFF
//#define TEST_POLYGON_ITEM OFF
//#define TEST_LINE_ITEM OFF
//#define TEST_PEN_ITEM OFF
//#define TEST_TEXT_ITEM OFF
//#define TEST_CUT_ITEM OFF
//#define TEST_SCANLE_ITEM OFF
//#define TEST_ELLIPSE_ITEM OFF
//#define TEST_FUNCTION_ITEM OFF

DWIDGET_USE_NAMESPACE

MainWindow *getMainWindow();

CGraphicsView *getCurView();

class DMouseMoveEvent;

class DTestEventList: public QTestEventList
{
public:
    void addMousePress(Qt::MouseButton button, Qt::KeyboardModifiers stateKey = Qt::KeyboardModifiers(),
                       QPoint pos = QPoint(), int delay = -1);

    void addMouseRelease(Qt::MouseButton button, Qt::KeyboardModifiers stateKey = Qt::KeyboardModifiers(),
                         QPoint pos = QPoint(), int delay = -1);

    void addMouseMove(QPoint pos = QPoint(), int delay = -1, Qt::KeyboardModifiers stateKey = Qt::KeyboardModifiers());

    void simulate(QWidget *w);
private:
    Qt::MouseButton _pressedMouseButton = Qt::NoButton;
};


void createNewViewByShortcutKey();

void setPenWidth(CGraphicsItem *item, int width);

void setStrokeColor(CGraphicsItem *item, QColor color);

void setBrushColor(CGraphicsItem *item, QColor color);

void resizeItem();

void createItemByMouse(CGraphicsView *view,
                       bool altCopyItem = false, QPoint topLeft = QPoint(500, 300),
                       QPoint bottomRight = QPoint(600, 400), bool doUndoRedo = true,
                       Qt::KeyboardModifiers stateKey = Qt::NoModifier);

void createItemWithkeyShift(CGraphicsView *view, bool altCopyItem = true, QPoint startPos = QPoint(200, 100)
                                                                                            , QPoint endPos = QPoint(400, 300));

void keyShortCutCopyItem(int addTimes = 1);

QList<CGraphicsItem *> currentSceneBzItems(CDrawScene::ESortItemTp sortTp = CDrawScene::EDesSort);
int                    currentSceneBzCount();

void layerChange();


void groupUngroup();

void selectAllItem();


void itemAlignment();


void itemTextRightClick();


void itemRightClick();

#endif // PUBLICAPI_H
